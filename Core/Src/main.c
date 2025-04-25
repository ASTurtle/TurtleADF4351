/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "stm32g0xx_hal_gpio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "./ec_button.h" // Adjusted path to locate the header file
#include "./lcd.h"
#include "./menu.h"
#include "./adf4351.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char str[40]={0};
struct button_obj_t ecbt;
static uint8_t btn1_event_val=0,ecop_val[2]={0,0};

uint16_t ec_cnt = 0;
short ec_dcnt = 0;  //encoder cnt difference


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM17_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  
  button_init(&ecbt, read_button_pin, 0, BUTTON_1, NULL, 0);
  button_start(&ecbt);
  HAL_TIM_Base_Start_IT(&htim17); // Start timer interrupt for button handling

  delay_init(64); // Initialize delay function with a 64MHz clock
  LCD_Init(); // Initialize the LCD
  LCD_Fill(0, 0, LCD_W, LCD_H, BLACK); // Fill the LCD with white color
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL); // Start the encoder timer
  //delay_us(10);
  ADF4351_Init(&myADF4351,&myADF4351_settings); // Initialize the ADF4351 PLL
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); // Turn on the LED
  Menu_Init();
  Show_table(0); // Show the main menu on the LCD
  // LCD_ShowString(24, 0, (uint8_t*)"ADF4351_PLL_Menu", GREEN, BLACK, 24, 0);
  // LCD_ShowString(0, 24, (uint8_t*)"1.", CYAN, BLACK, 24, 0);
  // LCD_ShowString(0, 48, (uint8_t*)"2.", CYAN, BLACK, 24, 0);
  // LCD_ShowString(0, 72, (uint8_t*)"3.", CYAN, BLACK, 24, 0);
  // LCD_ShowString(0, 96, (uint8_t*)"4.", CYAN, BLACK, 24, 0);
  // LCD_ShowString(0, 120, (uint8_t*)"Powered by Turtle@AutoC4M", CYAN, BLACK, 12, 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /****************Get Encoder and button operation************************/
    if(btn1_event_val!=get_button_key_value(&ecbt)){
      //btn1_event_val = get_button_key_value(&ecbt);
      btn1_event_val = ecbt.key_value; // Get the key value from the button object
      switch(btn1_event_val){
        case NONE_PRESS_KV:
          ecop_val[0] = OP_NONE;
          //sprintf(str,"NONE_PRESS_KV\r\n");
          //HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
          break;
        case SINGLE_CLICK_KV:
          ecop_val[0] = OP_ENTER;
          //sprintf(str,"SINGLE_CLICK_KV\r\n");
          //HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);

          break;
        case DOUBLE_CLICK_KV:
          ecop_val[0] = OP_EXIT;
          //sprintf(str,"DOUBLE_CLICK_KV\r\n");
          //HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);

          break;
        default:
          break;
      }
    }
    if((ecop_val[0]==OP_NONE)&&(ec_cnt!=__HAL_TIM_GET_COUNTER(&htim3))){
      ec_dcnt = __HAL_TIM_GET_COUNTER(&htim3) - ec_cnt; // Get the encoder count difference
      ec_cnt = __HAL_TIM_GET_COUNTER(&htim3); // Get the current encoder count value
      if(abs(ec_dcnt)>=1){
        if(ec_dcnt>0){
          ecop_val[0] = OP_UP; // Encoder turned up
        }else{
          ecop_val[0] = OP_DOWN; // Encoder turned down
        }
      }else{
        ecop_val[0] = OP_NONE; // No encoder movement
      }
    }

    /*******************Encoder and button op got********************/
    if(ecop_val[1]!=ecop_val[0]){ // Check if the operation value has changed
      Menu_Update(ecop_val[0]); // Update the menu based on the operation
      //LCD_ShowIntNum(10, 30, ecop_val[0], 6, BLACK, WHITE, 16); 
    }
    
    
  ecop_val[1] = ecop_val[0]; // Store the operation value for the next iteration
  ecop_val[0] = OP_NONE; // Reset the operation value

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*********** Implement timer interrupt callback **************/
/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
  {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(htim);
  
    if(htim==&htim17){
       button_ticks();//embbed button  button state update
       //ec_dcnt = __HAL_TIM_GetCounter(&htim3) - ec_cnt;  //get encoder val plus
       //ec_cnt = __HAL_TIM_GET_COUNTER(&htim3); // get encoder val
    }
  }
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

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
