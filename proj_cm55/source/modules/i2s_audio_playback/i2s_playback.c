/******************************************************************************
* File Name : i2s_playback.c
*
* Description :
* Source file for Audio Playback via I2S.
********************************************************************************
* (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
* 
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cybsp.h"
#include "cy_pdl.h"

#include "i2s_playback.h"

#include "audio_enhancement.h"
#include "cy_pdl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cyabs_rtos.h"

#include "cy_afe_configurator_settings.h"
#include "app_i2s.h"
#include "app_logger.h"
#include "audio_usb_send_utils.h"
#include "audio_conv_utils.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* I2S hardware FIFO size to be filled per I2S TX transaction for 2 channels */
#define HW_FIFO_SIZE                        (I2S_HW_FIFO_SIZE / 2)

/* Number of samples (for 2 channels) in the audio frame in the queue */
#define FRAME_SIZE                          (2 * 160u)

/* Number of I2S TX transactions per audio frame */
#define NUM_I2S_BUFFERS_IN_AUDIO_FRAME      (FRAME_SIZE / HW_FIFO_SIZE)

/*******************************************************************************
* Global Variables
*******************************************************************************/
extern uint32_t initial_buffer_count;
int16_t i2s_usb_buffer[FRAME_SIZE] = {0};
uint32_t i2s_tx_frame_count = 0;
int8_t i2s_write_flag = 0;
int8_t valid_audio_frame =0;


/*******************************************************************************
* Function Name: i2s_init
********************************************************************************
* Summary:
* Initialize I2S block and Hardware codec.
*
* Parameters:
* None
*
* Return:
*  None
*
*******************************************************************************/
void i2s_init(void)
{
    /* TLV codec initiailization */
    app_tlv_codec_init();

    /* I2S initialization */
    app_i2s_init();
    app_i2s_enable();

    /* Write zeros to both left and right channel to initiate I2S TX */
    for (uint32_t i = 0; i < (HW_FIFO_SIZE / 2); i++)
    {
        Cy_AudioTDM_WriteTxData(TDM_STRUCT0_TX, (uint32_t) 0);
        Cy_AudioTDM_WriteTxData(TDM_STRUCT0_TX, (uint32_t) 0);
    }

    app_i2s_activate();

    i2s_tx_frame_count = 0;
}

/*******************************************************************************
* Function Name: i2s_tx_interrupt_handler
********************************************************************************
* Summary:
*  I2S ISR handler.
* Return:
*  None
*******************************************************************************/
void i2s_tx_interrupt_handler(void)
{
    cy_rslt_t ret_val  = CY_RSLT_SUCCESS;
    int16_t* i2s_tx_ptr = NULL;

    /* Get interrupt status and check for tigger interrupt and errors */
    uint32_t intr_status = Cy_AudioTDM_GetTxInterruptStatusMasked(TDM_STRUCT0_TX);

    if(CY_TDM_INTR_TX_FIFO_TRIGGER & intr_status)
    {
        i2s_write_flag = 1;
        if (i2s_write_flag)
        {
            if (0 == i2s_tx_frame_count)
            {
                ret_val = usb_mic_pop(i2s_usb_buffer);
                if (ret_val!= CY_RSLT_SUCCESS)
                {
                    initial_buffer_count = 0;
                    memset(i2s_usb_buffer, 0, sizeof(i2s_usb_buffer));
                    valid_audio_frame=0;
                }
                else
                {
                    valid_audio_frame=1;
                }
            }

            /* Write the data from the buffer to I2S */
            i2s_tx_ptr = i2s_usb_buffer + (i2s_tx_frame_count * HW_FIFO_SIZE);
            for (uint32_t i = 0; i < HW_FIFO_SIZE; i++)
            {
                Cy_AudioTDM_WriteTxData(TDM_STRUCT0_TX, (uint32_t) i2s_tx_ptr[i]);
            }

            i2s_tx_frame_count ++;
            if (i2s_tx_frame_count >= NUM_I2S_BUFFERS_IN_AUDIO_FRAME)
            {
                i2s_tx_frame_count = 0;
                if (valid_audio_frame==1)
                {
                    usb_aec_push(i2s_usb_buffer);
                    valid_audio_frame=0;
                }
            }
        }
    }
    else if(CY_TDM_INTR_TX_FIFO_UNDERFLOW & intr_status)
    {
        /*app_log_print("Error: I2S transmit underflowed");*/
    }

    /* Clear all Tx I2S Interrupt */
    Cy_AudioTDM_ClearTxInterrupt(TDM_STRUCT0_TX, CY_TDM_INTR_TX_MASK);
}

/* [] END OF FILE */
