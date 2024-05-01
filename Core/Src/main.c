/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  *
  * ToDo
  * - dodanie peryferiów i uruchomienie (plus wyczyszczenie kodu)
  * 	- zrobiony! OLED
  * 	- zrobiony! KeyPad
  * 	- zrobiony! RS232 huart2
  * 	- PS2
  * 		- branch PS2; na bazie https://github.com/RobertoBenjami/stm32_ps2
  * 		- usunięcie obsługi myszy
  * 	- USB keyboard
  * 		- custom HID
  * 			- report...kbd_report
  * 			- port i pin LED
  * - sprawdzić rolę Timer2 - czy jest potrzebny
  * - sprawdzić, czy jest IP DMA (ustawienia USB OTG w MX)potrzebne
  * - nastąpiła zmiana opisów kolumn KeyPada
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// OLED
#include "ssd1306.h"
// KeyPad
#include "KeyPad.h"
// keyboard PS2
#include "keyboardPS2.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// OLED begin
#define SSD1306_USE_I2C
#define __DEBUG 1
#define BUFFERSIZE 100
#define I2CBUF	12
// OLED end

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// OLED begin:
const uint8_t SSD1306_ADDRESS = 0x3C << 1;
const uint8_t RANDOM_REG = 0x0F;
char uartBuffer[BUFFERSIZE] = "";
uint8_t I2CBuffer[I2CBUF] = {0};
HAL_StatusTypeDef returnValue = 0;
// OLED end

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define KEY_MOD_LCTRL  	0x01
#define KEY_MOD_LSHIFT 	0x02
#define KEY_MOD_LALT   	0x04
#define KEY_MOD_LMETA  	0x08
#define KEY_MOD_RCTRL  	0x10
#define KEY_MOD_RSHIFT 	0x20
#define KEY_MOD_RALT   	0x40
#define KEY_MOD_RMETA  	0x80
#define KEY_CAPSLOCK 	0x39
#define KEY_LET(v)		(v - 'A' + 4) // A key - code 4

// ToDo poprawić
#define BTN_DOWN 1

struct kbd_report {
    uint8_t modifier;
    uint8_t reserved;
    uint8_t key[6];
};
void HAL_SYSTICK_Callback(void)
{
    static const struct kbd_report kreps[5] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, KEY_LET('A'), 0, 0, 0, 0, 0},
        {KEY_MOD_LSHIFT, 0, KEY_LET('B'), 0, 0, 0, 0, 0},
        {KEY_MOD_RALT, 0, KEY_LET('C'), 0, 0, 0, 0, 0},
        {0, 0, KEY_CAPSLOCK, 0, 0, 0, 0, 0}
    };
    static uint8_t phase = 5;
    static uint8_t khist;
    static uint8_t tdiv;
    ++ tdiv;
    //if (tdiv % 10 == 0 && phase == 5 && (khist = (khist << 1 | BTN_DOWN) & 3) == 1))
    if (true)
        {
            phase = 0;
        }
        if (tdiv == 100)
        {
            tdiv = 0;
            USBD_CUSTOM_HID_SendReport_FS((uint8_t*)&kreps[phase], sizeof(struct kbd_report));
            if (phase < 5) ++phase;
        }
    }
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	// KeyPad begin
	KeyPad_Init();
	// KeyPad end

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
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  MX_SPI3_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  // OLED begin
	ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0,26);
	ssd1306_WriteString("Manager started.", Font_7x10, White);
	ssd1306_UpdateScreen();
	// OLED end

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	// keybord PS2 begin
	uint8_t ch;
	//ToDo nie działa printf wzięty z biblioteki PS2
	printf("\r\nControl start\r\n");
	// keyboard PS2 end
	// RS send begin
	const char message[] = "Keyboard started!\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
	// RS send end
  while (1)
  {
	// keybord PS2 begin
	/* here we simulate the time of other activities in the program loop */
	HAL_Delay(30);

	/* get keyboard */
	while(ps2_kbd_getkey(&ch) == 1)
	{ /* recevied keyboard data */
		/*
	  if(ch >= 32)
		printf("Kbd: '%c', 0x%X\r\n", ch, ch);
	  else
		printf("Kbd:       0x%X\r\n", ch);
		*/
		ssd1306_Init();
		ssd1306_Fill(Black);
		ssd1306_SetCursor(0,26);
		ssd1306_WriteChar(ch, Font_11x18, White);
		ssd1306_UpdateScreen();

	}
	// keyboard PS2 end

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // KeyPad begin
		 uint32_t timeout = 20;
		 char znak = KeyPad_WaitForKeyGetChar(timeout);
		 if (znak != 'X')
		 {
			  	ssd1306_Init();
			    ssd1306_Fill(Black);
			    ssd1306_SetCursor(0,26);
			    //char c = znak;
			    char str1[2] = {znak , '\0'};
				ssd1306_WriteString(str1, Font_11x18, White);
				ssd1306_UpdateScreen();

		 }
		 // KeyPad end
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 20;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
