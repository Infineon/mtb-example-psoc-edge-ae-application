/******************************************************************************
* File Name : tune_mic_data_feed.c
*
* Description :
* Feeds data received via PDM ISR/USB ISR to the audio pipeline on CM55
********************************************************************************
* Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "audio_conv_utils.h"
#include "audio_enhancement_interface.h"
#include "cybsp_types.h"
#include "audio_usb_send_utils.h"
#include "app_logger.h"
#include "audio_input_configuration.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cyabs_rtos.h"
#include "ae_application.h"
#include "cy_afe_audio_speech_enh.h"

/*******************************************************************************
* Macros
*******************************************************************************/

/*For Speech Quality check for AEC where the test stream will have audio data
* in left channel and AEC reference in right channel - Set to zero unless benchmarking*/
#define AEC_QUALITY_MODE                            (0)

#define MAX_SIZE_FOR_NON_INTERLEAVE_STEREO_IN_BYTES (320)
/* Number of samples in an audio frame */
#define FRAME_SIZE                                  (320u)
#define PLAYBACK_DATA_FRAME_SIZE                    (2*FRAME_SIZE)

#define BULK_DELAY_10MS                             (10u)
#define BULK_DELAY_MIN_FRAME                        (0)

/*******************************************************************************
* Global Variables
*******************************************************************************/
int16_t non_interleaved_audio_ping[MAX_SIZE_FOR_NON_INTERLEAVE_STEREO_IN_BYTES] = {0};
int16_t non_interleaved_audio_pong[MAX_SIZE_FOR_NON_INTERLEAVE_STEREO_IN_BYTES] = {0};
int16_t aec_ref_buffer[MONO_AUDIO_DATA_IN_BYTES] = {0};
int16_t usb_buffer[MONO_AUDIO_DATA_IN_BYTES*2] = {0};

unsigned int bdm_aec_ref_sent_len = 0;
char stereo_pdm[PLAYBACK_DATA_FRAME_SIZE] = {0};

int16_t* actual_aec_ref = aec_ref_buffer;
int8_t buff_toggle_flag = 0;
int16_t* non_interleaved_audio = NULL;

extern int16_t* usb_aec_ref;
extern int8_t aec_ref_flag;
extern uint32_t initial_buffer_count;
extern unsigned int bdm_aec_ref_len;
extern char *bdm_aec_ref_buffer;

extern volatile int8_t ae_toggle_flag;
uint8_t bulk_delay = AFE_CONFIG_BULK_DELAY;
uint8_t bulk_delay_equivalent_frames = 0;


/*******************************************************************************
* Function Name: ae_audio_data_feed
********************************************************************************
* Summary:
* Receive PDM mic frames and USB frames and feed it to the audio pipeline.
*
* Parameters:
*  audio_data - Pointer to audio buffer.
*  length - length of audio data.
* Return:
*  None
*
*******************************************************************************/
void ae_audio_data_feed(int16_t *audio_data,uint16_t length)
{
    int16_t* aec_reference = NULL;
    cy_rslt_t ret_val  = CY_RSLT_SUCCESS;
    uint16_t *stereo = (uint16_t*)&stereo_pdm[0];

#ifndef ENABLE_IFX_AEC
    aec_reference = NULL;
#endif /* ENABLE_IFX_AEC */
/* For Bulk Delay measurement via Calibrate option*/
    if(NULL != bdm_aec_ref_buffer)
    {
        aec_reference = (int16_t* )((char *)bdm_aec_ref_buffer+bdm_aec_ref_sent_len);
        convert_mono_to_stereo_interleaved(stereo,(uint16_t *)aec_reference);
        usb_mic_push( (short *)(stereo_pdm));
        bdm_aec_ref_sent_len = bdm_aec_ref_sent_len + FRAME_SIZE;
        if(bdm_aec_ref_len == bdm_aec_ref_sent_len)
        {
             bdm_aec_ref_sent_len = 0;
        }
        ret_val=usb_aec_pop(usb_buffer,0);
        if (ret_val==CY_RSLT_SUCCESS)
        {
        /* Convert Stereo AEC to Mono AEC reference */
            convert_stereo_interleaved_to_mono((uint16_t*)usb_buffer,(uint16_t*)aec_ref_buffer);
            aec_reference = aec_ref_buffer;
        } else {
            aec_reference = NULL;
        }
    }
 /* End of Bulk Delay measurement */   
    else 
    {
        bulk_delay_equivalent_frames =(bulk_delay/BULK_DELAY_10MS);
    
        ret_val=usb_aec_pop(usb_buffer,bulk_delay_equivalent_frames);
        if (ret_val==CY_RSLT_SUCCESS)
        {
        /* Convert Stereo AEC to Mono AEC reference */
            convert_stereo_interleaved_to_mono((uint16_t*)usb_buffer,(uint16_t*)aec_ref_buffer);
            aec_reference = aec_ref_buffer;
        }
        else
        {
            aec_reference = NULL;
        }
    }

    if (buff_toggle_flag == 0)
    {
        non_interleaved_audio = non_interleaved_audio_ping;
        buff_toggle_flag = 1;

    } else if (buff_toggle_flag == 1)
    {
        non_interleaved_audio = non_interleaved_audio_pong;
        buff_toggle_flag = 0;
    }
#ifdef ENABLE_STEREO_INPUT_FEED


        
/* Mic is configured in STEREO mode */
    convert_interleaved_to_stereo_non_interleaved((uint16_t *)audio_data,
            (uint16_t *)non_interleaved_audio);
            
  
    if (ae_toggle_flag==0)
    {
        usb_send_out_dbg_put(USB_CHANNEL_1,(int16_t *)non_interleaved_audio);
    }
    
   
/* Feed the data to Audio Enhancement */
    ae_interface_feed(non_interleaved_audio, aec_reference);

#else
/* Mic is configured in MONO mode */
    if (ae_toggle_flag==0)
    {
        usb_send_out_dbg_put(USB_CHANNEL_1,(int16_t *)audio_data);
    }
/* Feed the data to Audio Enhancement */
    ae_interface_feed(audio_data, aec_reference);
#endif /* ENABLE_STEREO_INPUT_FEED */


}


/*******************************************************************************
* Function Name: ae_audio_data_feed_usb
********************************************************************************
* Summary:
* Receive USB frames and feed it to the audio pipeline.
*
* Parameters:
*  audio_data - Pointer to audio buffer.
* Return:
*  None
*
*******************************************************************************/
void ae_audio_data_feed_usb(int16_t *audio_data)
{
    int16_t* aec_reference = NULL;

/* Mic is configured in STEREO mode */

    if (buff_toggle_flag == 0)
    {
        non_interleaved_audio = non_interleaved_audio_ping;
        buff_toggle_flag = 1;

    } else if (buff_toggle_flag == 1)
    {
        non_interleaved_audio = non_interleaved_audio_pong;
        buff_toggle_flag = 0;
    }

    convert_interleaved_to_stereo_non_interleaved((uint16_t *)audio_data,
            (uint16_t *)non_interleaved_audio);

#if AEC_QUALITY_MODE
    /* Used for Quality benchmarking of AEC with L channel - Audio+echo and R channel - Echo reference
     * Used with mono mic settings in AFE configurator.
     */
    aec_reference=non_interleaved_audio+160;
#endif /* AEC_QUALITY_MODE */
/* Feed the data to Audio Enhancement */
    ae_interface_feed(non_interleaved_audio, aec_reference);

}

/* [] END OF FILE */


