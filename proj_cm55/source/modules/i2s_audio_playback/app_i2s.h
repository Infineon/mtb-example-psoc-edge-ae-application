/******************************************************************************
* File Name : app_pdm_pcm.h
*
* Description : Header file for i2s containing  function ptototypes.
********************************************************************************
* (c) 2025-2026, Infineon Technologies AG, or an affiliate of Infineon
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

/* Speaker volume: value written to the DAC digital volume register
 * (two's-complement, 0x00 = 0 dB). 110 | 0x80 = 0xEE ~= -9 dB. */
#define I2S_TLV_CODEC_VOLUME              (110)

/* Headphone volume: value written to the analog HP routing volume register
 * where 0 = 0 dB and larger values = more attenuation (127 ~= mute). This
 * register uses the opposite encoding from the speaker DAC volume register,
 * so it must NOT reuse I2S_TLV_CODEC_VOLUME (which would nearly mute the HP).
 * Step 18 matches the codec driver's default analog HP level. */
#define I2S_TLV_HP_CODEC_VOLUME           (5)

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
