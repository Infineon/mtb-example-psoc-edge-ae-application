/******************************************************************************
* File Name : user_button.c
*
* Description :
* Source file for user button handling
********************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
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
#include "user_button.h"
#include "app_logger.h"

/*****************************************************************************
* Macros
*****************************************************************************/

#define USER_BUTTON_TASK_NAME               ("user_button_task")
#define USER_BUTTON_TASK_STACK_SIZE         (128)
#define USER_BUTTON_TASK_PRIORITY           (CY_RTOS_PRIORITY_NORMAL)


/* Debounce counter for button presses (multiply by 10 ms) */
#define BUTTON_DEBOUNCE_COUNT               (10U)
#define TASK_WAIT_TIME_MS                   (20)

/*******************************************************************************
* Global Variables
*******************************************************************************/

TaskHandle_t rtos_user_btn_task;
uint8_t user_button_1_press = 0;

/* Button callback*/
void (*button_callback)(void);

/*******************************************************************************
* Function Name: user_button_init
********************************************************************************
* Summary:
* Initialize user button
*
* Parameters:
*  None
* 
* Return:
* None
*
*******************************************************************************/
void user_button_init(cb_user_action arg)
{
    BaseType_t rtos_task_status;
    
    rtos_task_status = xTaskCreate(user_button_task, USER_BUTTON_TASK_NAME,
                        USER_BUTTON_TASK_STACK_SIZE, NULL, USER_BUTTON_TASK_PRIORITY,
                        &rtos_user_btn_task);

    if (pdPASS != rtos_task_status)
    {
        app_log_print("User button task creation failed \r\n");
        CY_ASSERT(0);
    }
    button_callback = (cb_user_action)arg;
    
}

/*******************************************************************************
* Function Name: process_user_button
********************************************************************************
* Summary:
* Process user button and set the functionality
*
* Parameters:
*  None
* 
* Return:
* None
*
*******************************************************************************/

void process_user_button(void)
{
    if (0 == Cy_GPIO_Read(CYBSP_USER_BTN1_PORT, CYBSP_USER_BTN1_NUM))
    {
        if (user_button_1_press==0)
        {
            button_callback();
            user_button_1_press=1;
        }
    }
    else
    {
        user_button_1_press=0;
    }

    
}


/*******************************************************************************
* Function Name: user_button_task
********************************************************************************
* Summary:
* User button task
*
* Parameters:
*  None
* 
* Return:
* None
*
*******************************************************************************/
void user_button_task(void* arg)
{
    
    while(1)
    {
        process_user_button();  
        vTaskDelay(TASK_WAIT_TIME_MS);
    }
    
    
}