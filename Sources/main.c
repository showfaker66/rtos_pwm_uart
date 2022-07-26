#include "Cpu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "interrupt_manager.h"
#include "clock_manager.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "lpuart1.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

volatile int exit_code = 0;

void delay(volatile int cycles);
static void pwm(void *pvParameters);
#define pwmsize 90
#define pwmpro  2
TaskHandle_t pxpwmtask;

#define uartsize 90
#define uartpro  2
TaskHandle_t pxuarttask;

 void delay(volatile int cycles)
  {
  	while(cycles--);
  }
  /* The queue used by both tasks. */
  static void pwm(void *pvParameters)
  {
  	(void)pvParameters;
  	uint32_t duty = 0;
      bool increaseDuty = true;
      uint8_t channel = pwm_pal1Configs.pwmChannels[0].channel;
      while(1)
  	 {
  		 PWM_UpdateDuty(&pwm_pal1Instance, channel, duty);
  		 if (increaseDuty == true)
  			{
  			   duty++;
  			   if (duty > 4999U)
  				   increaseDuty = false;
  			}
  		 else
  			{
  			   duty--;
  			   if (duty < 1U)
  				   increaseDuty = true;
  			}
  	  delay(1500U);
//  		 vTaskDelay(1000);
  	 }
  }

#define welcomeMsg "Hello world\r\n"
static void uart_task(void *pvParameters)
{
	(void)pvParameters;
   LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
   while(1){
	   LPUART_DRV_SendData(INST_LPUART1,(uint8_t *)welcomeMsg, strlen(welcomeMsg));
   }
  	   vTaskDelay(10);
}

int main(void)
{
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/
	  CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
					 g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	  CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);
	  PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
	  PWM_Init(&pwm_pal1Instance, &pwm_pal1Configs);

	xTaskCreate(pwm,"pwm",pwmsize,NULL,pwmpro,&pxpwmtask);
	xTaskCreate(uart_task,"uart_task",uartsize,NULL,uartpro,&pxuarttask);
	vTaskStartScheduler();
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

