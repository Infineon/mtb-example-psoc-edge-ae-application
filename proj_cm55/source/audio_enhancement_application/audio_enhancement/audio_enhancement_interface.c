/******************************************************************************
* File Name : audio_enhancement_interface.c
*
* Description :
* Wrapper for Audio Enhancement
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

#include "audio_enhancement_interface.h"
#include "audio_usb_send_utils.h"

/*******************************************************************************
* Macros
*******************************************************************************/

/*******************************************************************************
* Global Variables
*******************************************************************************/
extern volatile bool ae_toggle_flag;
/*******************************************************************************
* Function Name: license_limitation_exit
********************************************************************************
* Summary:
*  This function is blocking call after Audio Front End license timeout
*
*******************************************************************************/
void license_limitation_exit()
{
    while(1){}
}

/*******************************************************************************
 * Function Name: audio_enhancement_process_output
 *******************************************************************************
 * Summary:
 * Use case API
 * Sends back AE data/tuning data back to PC via USB Audio Class
 *
 * Parameters:
 *  output_buffer: pointer to the output audio data buffer.
 *
 * Return:
 *  void
 *
 *******************************************************************************/
 
void audio_enhancement_process_output(ae_buffer_info_t *ae_output_buffer)
{
    
#ifdef AE_FUNCTIONAL_MODE
    int16_t *output_buffer = (int16_t *)ae_output_buffer->output_buf;
#endif /*AE_FUNCTIONAL_MODE*/

#ifdef AE_TUNING_MODE
    char zero_buffer[AE_FRAME_BUFFER_MEMORY] = {0};
    int16_t *output_dgb1 = (int16_t *)ae_output_buffer->dbg_output1;
    int16_t *output_dgb2 = (int16_t *)ae_output_buffer->dbg_output2;
    int16_t *output_dgb3 = (int16_t *)ae_output_buffer->dbg_output3;
    int16_t *output_dgb4 = (int16_t *)ae_output_buffer->dbg_output4;
#endif /* AE_TUNING_MODE */

#if AE_APP_PROFILE
    cy_afe_profile(AFE_PROFILE_CMD_PRINT_STATS_1SEC, NULL);
    cy_afe_profile(AFE_PROFILE_CMD_RESET, NULL);
#endif /* AE_APP_PROFILE */
    
    if (ae_toggle_flag)
    {
#ifdef AE_FUNCTIONAL_MODE
        usb_send_out_dbg_put(USB_CHANNEL_1,(int16_t *)output_buffer);
#endif /* AE_FUNCTIONAL_MODE*/

#ifdef AE_TUNING_MODE
        usb_send_out_dbg_put(USB_CHANNEL_1,(int16_t *)output_dgb1);
        usb_send_out_dbg_put(USB_CHANNEL_2,(int16_t *)output_dgb2);
        usb_send_out_dbg_put(USB_CHANNEL_3,(int16_t *)output_dgb3);
        usb_send_out_dbg_put(USB_CHANNEL_4,(int16_t *)output_dgb4);
#endif /* AE_TUNING_MODE */
    }
    else
    {
#ifdef AE_TUNING_MODE
        usb_send_out_dbg_put(USB_CHANNEL_2,(int16_t *)zero_buffer);
        usb_send_out_dbg_put(USB_CHANNEL_3,(int16_t *)zero_buffer);
        usb_send_out_dbg_put(USB_CHANNEL_4,(int16_t *)zero_buffer);
#endif /* AE_TUNING_MODE */
    } 
    return;
}


/*******************************************************************************
* Function Name: ae_interface_init
********************************************************************************
* Summary:
* Initialize Audio Enhancement
*
* Parameters:
*  channels - number of input channels
* 
* Return:
*  Result of AE initialization.
*
*******************************************************************************/

int ae_interface_init(int channels)
{

    ae_rslt_t result = AE_RSLT_SUCCESS;

    result = audio_enhancement_init(channels);
    
    if (result != AE_RSLT_SUCCESS) 
    {
        app_log_print("DEEPCRAFT Audio Enhancement initialization failed \r\n");
    }
    else
    {
        app_log_print("DEEPCRAFT Audio Enhancement initialized \r\n");
    }
#if AE_APP_PROFILE
    cy_profiler_init();
    cy_afe_profile(AFE_PROFILE_CMD_ENABLE,NULL);
#endif /* AE_APP_PROFILE */
    
    return result;
}

/*******************************************************************************
* Function Name: ae_interface_feed
********************************************************************************
* Summary:
*  Feed mic audio data and AEC reference to Audio Enhancement.
*
*******************************************************************************/

int ae_interface_feed(void* audio_input, void* aec_buffer)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    result = audio_enhancement_feed_input((int16_t*)audio_input, (int16_t*)aec_buffer);
    
    if (AE_RSLT_LICENSE_ERROR == result)
    {
        app_log_print("CPU Halt: Audio Enhancement Restricted License Timeout - Reset the board \r\n");
        license_limitation_exit();
        return result;
    }

    if(AE_RSLT_SUCCESS != result)
    {
        app_log_print("Failed to feed data to AE \r\n");
        return result;
    }
    return result;
}
/* [] END OF FILE */
