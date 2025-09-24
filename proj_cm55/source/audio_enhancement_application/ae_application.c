/******************************************************************************
* File Name : ae_application.c
*
* Description :
* Audio application for CM55 core - DEEPCRAFT(TM) Audio Enhancement (AE).
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

#include "app_logger.h"
#include "cycfg_pins.h"
#include "cybsp_types.h"
#include "ae_application.h"
#include "pdm_mic_interface.h"
#include "usb_audio_interface.h"
#include "audio_usb_send_utils.h"
#include "user_interaction.h"
#include "i2s_playback.h"
#include "audio_enhancement_interface.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/
volatile bool ae_toggle_flag = true;


/*******************************************************************************
* Function Name: ae_user_btn_callback
********************************************************************************
* Summary:
* Callback for user button - To toggle between AE processed/unprocessed stream.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/

void ae_user_btn_callback(void)
{
    ae_toggle_flag = !ae_toggle_flag;
    if (ae_toggle_flag)
    {
        Cy_GPIO_Write(CYBSP_LED_BLUE_PORT, CYBSP_LED_BLUE_PIN, CYBSP_LED_STATE_ON);
    }
    else
    {
        Cy_GPIO_Write(CYBSP_LED_BLUE_PORT, CYBSP_LED_BLUE_PIN, CYBSP_LED_STATE_OFF);
    }
}

/*******************************************************************************
* Function Name: ae_user_btn_init
********************************************************************************
* Summary:
* Initialize user button and pass the callback.
*
* Parameters:
*  user_action_cb - Callback function for user button.
*
* Return:
*  None
*
*******************************************************************************/

void ae_user_btn_init(cb_user_action user_action_cb)
{
    user_interaction_init(USER_INTERACTION_BUTTON,user_action_cb);
}

/*******************************************************************************
* Function Name: ae_application
********************************************************************************
* Summary:
* Initialize AE application with AFE MW, PDM and USB.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ae_application()
{
#if AFE_INPUT_SOURCE==AFE_INPUT_SOURCE_MIC
    cy_rslt_t result;
#endif /* AFE_INPUT_SOURCE_MIC */
    app_core2_boot_log();

/* Initialize I2S for audio playback */
    i2s_init();

/* Initializing DEEPCRAFT(TM) Audio Enhancement */
    ae_interface_init(AFE_INPUT_NUMBER_CHANNELS);

/* Initializing USB for TX/RX of audio data */
    app_log_print("Initializing USB interface \r\n");
    usb_audio_interface_init();
    usb_send_out_dbg_init_channels();

/* Initialize LEDs */
    led_init_hp();

/* Initialize the user button */
    ae_user_btn_init(ae_user_btn_callback);

    app_log_print("\x1b[2J\x1b[;H");

    app_log_print("****************** \
    DEEPCRAFT(TM) Audio Enhancement application on High Performance Core CM55+U55\
    ****************** \r\n\n");

    app_log_print("1. Connect PSOC Edge to the PC via USB device cable \r\n");
    app_log_print("2. Send Audio Data via PC to the enumerated USB Speaker for AEC reference \r\n");
    app_log_print("3. Capture Audio Data via PC from the enumerated USB Mic for AE processed data\r\n");
    app_log_print("Note: \r\n Refer to the README.md/ae_design_guide.md of this CE for details of different configurations and tuning via AFE configurator\r\n");

    /* PDM mic initialization. Initialize if PDM mic is choosen as the input-mode.
     * PDM mic data arrives via ISR.
    */
#if AFE_INPUT_SOURCE==AFE_INPUT_SOURCE_MIC
    result = pdm_mic_interface_init();
    if(CY_RSLT_SUCCESS != result)
    {
        app_log_print("PDM initialization failed - Reset the board \r\n");
        CY_ASSERT(0);
    }
#endif /* AFE_INPUT_SOURCE_MIC */

}

/*******************************************************************************
* Function Name: led_init_hp
********************************************************************************
* Summary:
*  Initializes LEDs for AE application.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/

void led_init_hp()
{
    if (ae_toggle_flag)
    {
        Cy_GPIO_Write(CYBSP_LED_BLUE_PORT, CYBSP_LED_BLUE_PIN, CYBSP_LED_STATE_ON);
    }
}


/* [] END OF FILE */
