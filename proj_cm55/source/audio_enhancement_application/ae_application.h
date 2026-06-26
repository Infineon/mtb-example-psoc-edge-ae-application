/******************************************************************************
* File Name : ae_application.h
*
* Description :
* Header for DEEPCRAFT(TM) Audio Enhancement application
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

#ifndef __AE_APPLICATION_H__
#define __AE_APPLICATION_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cycfg_pins.h"
#include "cybsp_types.h"
#include "FreeRTOS.h"
#include "timers.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#ifdef PSE84_AI_KIT
#define BLUE_LED_PORT          CYBSP_LED_RGB_BLUE_PORT
#define BLUE_LED_PIN           CYBSP_LED_RGB_BLUE_PIN

/* LED PWM duty cycle: ON ticks out of LED_PWM_PERIOD_MS total.
 * Adjust LED_PWM_ON_MS to control brightness (lower = dimmer).
 * Example: 2 ON out of 10 period = 20% brightness. */
#define LED_PWM_PERIOD_MS      (10U)
#define LED_PWM_ON_MS          (2U)

#else
#define BLUE_LED_PORT          CYBSP_LED_BLUE_PORT
#define BLUE_LED_PIN           CYBSP_LED_BLUE_PIN
#endif /* PSE84_AI_KIT */



/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/

void ae_application();
void led_init_hp();

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* __AE_APPLICATION_H__ */
/* [] END OF FILE */
