#include "main.h"
#include "tim.h"
#include "gpio.h"

#include "stm32f1xx_hal.h"

#define DEBOUNCE_TIME_MS 50 // 50ms

TIM_HandleTypeDef htim;

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_TIM_Init(void);

volatile uint8_t inputFlags[3] = {0}; // Flags for debounced inputs

//--------------------------------------------------------------------------
int main(void)
{
  
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM1_Init();
  HAL_TIM_Base_Start_IT(&htim);

  while (1)
  {
		 // Check the flags and control the LEDs accordingly
     HAL_GPIO_WritePin(LED_PORT, LED_PIN_0, inputFlags[0] ? GPIO_PIN_RESET : GPIO_PIN_SET);
     HAL_GPIO_WritePin(LED_PORT, LED_PIN_1, inputFlags[1] ? GPIO_PIN_RESET : GPIO_PIN_SET);
     HAL_GPIO_WritePin(LED_PORT, LED_PIN_2, inputFlags[2] ? GPIO_PIN_RESET : GPIO_PIN_SET);
  }

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_RCC_EnableCSS();
}
//--------------------------------------------------------------------------

// Interrupt Service Routine (ISR) - Timer 1 (every 1ms)
//--------------------------------------------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1) // Assuming Timer1 is used
    {
        // Read inputs and apply debouncing
        static uint32_t debounceCounter[3] = {0}; // Assuming 3 inputs
        static GPIO_PinState previousState[3] = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET}; // Assuming active low inputs initially
        GPIO_PinState currentState[3];
				
				currentState[0] = HAL_GPIO_ReadPin(INPUT_PORT, INPUT_PIN_0); // Input 1 --> PB0
        currentState[1] = HAL_GPIO_ReadPin(INPUT_PORT, INPUT_PIN_1); // Input 2 --> PB1
        currentState[2] = HAL_GPIO_ReadPin(INPUT_PORT, INPUT_PIN_2); // Input 3 --> PB2

        for (int i = 0; i < 3; i++)
        {
            if (currentState[i] != previousState[i])
            {
                debounceCounter[i]++;
                if (debounceCounter[i] >= DEBOUNCE_TIME_MS)
                {
                    debounceCounter[i] = 0;
                    previousState[i] = currentState[i];
                    
                    // If the input is active for longer than debounce time, set the flag
                    if (currentState[i] == GPIO_PIN_RESET) // Assuming active low inputs
                    {
                        inputFlags[i] = 1;
                    }
                    else
                    {
                        inputFlags[i] = 0;
                    }
                }
            }
            else
            {
                debounceCounter[i] = 0;
            }
        }
    }
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
		//---
  }
}
//--------------------------------------------------------------------------

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
