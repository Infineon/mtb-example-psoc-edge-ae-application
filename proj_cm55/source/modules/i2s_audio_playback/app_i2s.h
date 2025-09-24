/******************************************************************************
* File Name : app_pdm_pcm.h
*
* Description : Header file for i2s containing  function ptototypes.
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


#ifndef __APP_I2S_H__
#define __APP_I2S_H__


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cy_pdl.h"
#include "mtb_hal.h"
#include "cybsp.h"

#include "mtb_tlv320dac3100.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* 16kHz playback */
#define MCLK_HZ                           (2048000)

/* Sampling rate in KHz */
#define SAMPLE_RATE_HZ                    (16000u)
/* I2S word length parameter */
#define I2S_WORD_LENGTH                   (16u)

#define I2C_ADDRESS                       (0x18)
/* I2C frequency in Hz */
#define I2C_FREQUENCY_HZ                  (400000u)

#define I2S_HW_FIFO_SIZE                  (128u)

#define I2S_TLV_CODEC_VOLUME              (110)

#define I2S_ISR_PRIORITY                  (2)



/*******************************************************************************
* Global Variables
*******************************************************************************/
extern volatile int16_t *audio_data_ptr;
extern int32_t recorded_data_size;
extern volatile bool i2s_flag;
/*******************************************************************************
* Functions Prototypes
*******************************************************************************/
void app_i2s_init(void);
void app_tlv_codec_init(void);
void tlv_codec_i2c_init(void);
void i2s_tx_interrupt_handler(void);
void app_i2s_enable(void);
void app_i2s_activate(void);
void app_i2s_deactivate(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __APP_I2S_H__ */
/* [] END OF FILE */
