/******************************************************************************
* File Name : audio_receive_task.c
*
* Description :
* Code to receive audio data from PC via USB audio class.
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
#include "USB_Audio.h"
#include "audio_app.h"
#include "audio.h"
#include "cycfg.h"
#include "rtos.h"
#include "audio_conv_utils.h"
#include "audio_usb_send_utils.h"
#include "audio_receive_task.h"
#include "cyabs_rtos.h"
#include "app_logger.h"

#include "cy_afe_configurator_settings.h"

#ifdef PROFILER_ENABLE
#include "cy_afe_profiler.h"
#include "cy_profiler.h"
#endif /* PROFILER_ENABLE */
/*******************************************************************************
* Macros
*******************************************************************************/
#define USB_10MS_AUDIO_SAMP            (160)
#define USB_AUDIO_RX_TASK_PRIORITY     (6)

/*******************************************************************************
* Global Variables
*******************************************************************************/

/* Audio IN flags */
volatile bool audio_out_is_streaming    = false;
volatile bool audio_start_streaming     = false;


int usb_packet_count                    = 0;
int ping_pong_buff                      = 0;
uint32_t initial_buffer_count           = 0;
int16_t audio_mic_buffer_usb_ping[USB_10MS_AUDIO_SAMP*2];
int16_t audio_mic_buffer_usb_pong[USB_10MS_AUDIO_SAMP*2];
int16_t audio_mic_buffer_zero_buff[USB_10MS_AUDIO_SAMP*2] = {0};


int8_t *audio_usb_ptr                   = NULL;
int16_t *usb_aec_ref                    = NULL;

int8_t *audio_usb_ref                   = NULL;
int8_t aec_ref_flag                     = 0;

TaskHandle_t rtos_audio_out_task;
TaskHandle_t rtos_audio_buf_task;

extern char *bdm_aec_ref_buffer;

/*******************************************************************************
* Functions Prototypes
*******************************************************************************/
extern void usb_mic_data_feed(int16_t *audio_data,uint16_t length);
extern void ae_audio_data_feed_usb(int16_t *audio_data);
/*******************************************************************************
* Function Name: audio_out_init
********************************************************************************
* Summary:
*   Initialize the audio OUT endpoint and USB mic task.
*
*******************************************************************************/
void audio_out_init(void)
{
    BaseType_t rtos_task_status;

    rtos_task_status = xTaskCreate(audio_out_process, "usb_audio_to_psoc",
                        RTOS_STACK_DEPTH, NULL, USB_AUDIO_RX_TASK_PRIORITY,
                        &rtos_audio_out_task);

    if (pdPASS != rtos_task_status)
    {
        CY_ASSERT(0);
    }
    rtos_task_status = xTaskCreate(audio_buff_task, "usb_buffer_task",
                         RTOS_STACK_DEPTH*4, NULL, USB_AUDIO_RX_TASK_PRIORITY,
                         &rtos_audio_buf_task);

     if (pdPASS != rtos_task_status)
     {
         CY_ASSERT(0);
     }
}


/*******************************************************************************
* Function Name: audio_out_enable
********************************************************************************
* Summary:
*   Start a playing session.
*
*******************************************************************************/
void audio_out_enable(void)
{
    USBD_AUDIO_Start_Listen(usb_audioContext, NULL);
    audio_start_streaming = true;

}

/*******************************************************************************
* Function Name: audio_out_disable
********************************************************************************
* Summary:
*   Stop a playing session.
*
*******************************************************************************/
void audio_out_disable(void)
{
    USBD_AUDIO_Stop_Listen(usb_audioContext);
    audio_out_is_streaming = false;

}

/*******************************************************************************
* Function Name: audio_out_process
********************************************************************************
* Summary:
*   Main task for the audio out endpoint. Check if it should start a playing
*   session.
*
*******************************************************************************/
void audio_out_process(void *arg)
{
    (void) arg;

    USBD_AUDIO_Start_Listen(usb_audioContext, NULL);

    USBD_AUDIO_Read_Task();

    while (1)
    {

    }
}

/*******************************************************************************
* Function Name: audio_buff_task
********************************************************************************
* Summary:
*   Buffers audio data received over USB for further processing.
*
*******************************************************************************/
void audio_buff_task(void *arg)
{
    uint32_t notify_val=0;
#if AFE_INPUT_SOURCE== AFE_INPUT_SOURCE_USB   
    int16_t usb_buffer[USB_10MS_AUDIO_SAMP*2] = {0};
#endif /* AFE_INPUT_SOURCE */  
    while(1)
    {
        xTaskNotifyWait(0,0,&notify_val,portMAX_DELAY);
        aec_ref_flag=0;

        if(NULL == bdm_aec_ref_buffer)
        {
#if AFE_INPUT_SOURCE==AFE_INPUT_SOURCE_MIC
#if AFE_USE_TARGET_SPEAKER /* Data coming from USB is played on device speaker*/
            usb_mic_push(usb_aec_ref);
#else
            usb_mic_push(usb_aec_ref);
#endif /*AFE_USE_TARGET_SPEAKER*/            
#else
            memcpy(usb_buffer,usb_aec_ref,USB_10MS_AUDIO_SAMP*2*2);
            ae_audio_data_feed_usb(usb_buffer);
#endif /*AFE_INPUT_SOURCE */
        }
    }
}

/*******************************************************************************
* Function Name: audio_out_endpoint_callback
********************************************************************************
* Summary:
*   Audio OUT endpoint callback implementation. 
*     Populates buffers with audio data received over USB from PC.
*   Populates buffers with audio data to loopback over USB to PC.
*
*******************************************************************************/
void audio_out_endpoint_callback(void * pUserContext,
                                 int NumBytesReceived,
                                 uint8_t ** ppNextBuffer,
                                 unsigned long * pNextBufferSize)
{
    CY_UNUSED_PARAMETER(pUserContext);
    if (audio_start_streaming)
    {
        audio_start_streaming = false;
        audio_out_is_streaming = true;


        audio_usb_ptr = (int8_t*)audio_mic_buffer_usb_ping;
        ping_pong_buff= 0;
        audio_usb_ref = audio_usb_ptr;

/* Start a transfer to the Audio OUT endpoint */
        *ppNextBuffer = (uint8_t *) audio_usb_ptr;
        initial_buffer_count=0;
/* Flush queues as PC will not send USB stop always. Depends on the media player used in PC */
        usb_mic_flush();
        usb_aec_flush();
    }
    else if(audio_out_is_streaming)
    {
/* USB receives 1ms of data for every interrupt */
        if(NumBytesReceived != 0)
        {
            if (usb_packet_count<USB_10MS_AUDIO_SAMP*4) {
                usb_packet_count+=NumBytesReceived;
                audio_usb_ptr+=NumBytesReceived;
            }
/* 10 ms data collected, so queue them */
            if (usb_packet_count>=USB_10MS_AUDIO_SAMP*4)
            {
/* Pre-buffer counter for I2S playback */
                initial_buffer_count++;
                usb_packet_count=0;
                usb_aec_ref=(int16_t *)audio_usb_ref;
                aec_ref_flag=1;

                xTaskNotify(rtos_audio_buf_task, 0,eNoAction);
/* Double buffers so that 10 ms AEC reference can be queued */
                if (ping_pong_buff==0)
                {
                    audio_usb_ptr=(int8_t*)audio_mic_buffer_usb_pong;
                    audio_usb_ref = audio_usb_ptr;
                    ping_pong_buff=1;
                }
                else
                {
                    audio_usb_ptr=(int8_t*)audio_mic_buffer_usb_ping;
                    audio_usb_ref = audio_usb_ptr;
                    ping_pong_buff=0;
                }
            }
/* Start a transfer to OUT endpoint */
            *ppNextBuffer = (uint8_t *) audio_usb_ptr;
        }
    }
}
/* [] END OF FILE */
