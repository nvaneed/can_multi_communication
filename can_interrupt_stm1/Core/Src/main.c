/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body (Configured for STM_1)
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <ctype.h>
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
CAN_HandleTypeDef hcan1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* UART Buffer Variables */
char uart_rx_buffer[100];
uint8_t rx_byte;
uint8_t rx_index = 0;
uint8_t message_ready = 0;

/* CAN RX Buffer Variables */
char can_rx_buffer[100];
uint8_t can_rx_index = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void CAN_Filter_Config(void);
void SendDynamicMessageViaCAN(char* msg);
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
  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */

  /* 1. Configure Hardware Filter for STM_1 (Accept only ID 0x101) */
  CAN_Filter_Config();

  /* 2. Start CAN Bus */
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
      Error_Handler();
  }

  /* 3. Activate CAN RX Interrupt */
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
      Error_Handler();
  }

  /* 4. Start UART RX Interrupt (Listen for first keystroke) */
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

  /* Boot Message */
  char startMsg[] = "\r\n=== STM_1 Node Initialized ===\r\nType a message for stm_2 or stm_3...\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)startMsg, strlen(startMsg), 100);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if (message_ready == 1)
      {
          // Send the assembled UART string over CAN
          SendDynamicMessageViaCAN(uart_rx_buffer);

          // Clear flag to accept next message
          message_ready = 0;
      }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
}

/**
  * @brief CAN1 Initialization Function
  */
static void MX_CAN1_Init(void)
{
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 6;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

void CAN_Filter_Config(void)
{
    CAN_FilterTypeDef canfilterconfig;

    canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig.FilterBank = 0;
    canfilterconfig.FilterFIFOAssignment = CAN_RX_FIFO0;

    /* Hardware Filter specifically for STM_1 (Accepts ONLY 0x101) */
    canfilterconfig.FilterIdHigh = 0x101 << 5;
    canfilterconfig.FilterIdLow = 0x0000;
    canfilterconfig.FilterMaskIdHigh = 0x7FF << 5; // Check all 11 bits of the ID
    canfilterconfig.FilterMaskIdLow = 0x0000;

    canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;

    if (HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void SendDynamicMessageViaCAN(char* msg)
{
    uint32_t target_id = 0;
    char temp_msg[100];

    // Copy the message and convert the copy to lowercase for checking
    strncpy(temp_msg, msg, sizeof(temp_msg));
    for(int i = 0; temp_msg[i]; i++) {
        temp_msg[i] = tolower((unsigned char)temp_msg[i]);
    }

    // Find destination
    if (strstr(temp_msg, "stm_1") != NULL) target_id = 0x101;
    else if (strstr(temp_msg, "stm_2") != NULL) target_id = 0x102;
    else if (strstr(temp_msg, "stm_3") != NULL) target_id = 0x103;
    else
    {
        char err[] = "Error: Target (stm_1, stm_2, stm_3) not found in message.\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)err, strlen(err), 100);
        return;
    }

    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    uint8_t TxData[8];

    TxHeader.StdId = target_id;
    TxHeader.ExtId = 0x00;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.TransmitGlobalTime = DISABLE;

    uint16_t len = strlen(msg);
    uint16_t offset = 0;

    // Echo what we are sending to the terminal
    char echo[150];
    sprintf(echo, "Sending to 0x%lX: %s\r\n", target_id, msg);
    HAL_UART_Transmit(&huart2, (uint8_t*)echo, strlen(echo), 100);

    // Break into 8-byte chunks
    while (offset < len + 1)
    {
        uint8_t chunk_size = (len + 1 - offset > 8) ? 8 : (len + 1 - offset);
        TxHeader.DLC = chunk_size;

        memset(TxData, 0, 8);
        memcpy(TxData, msg + offset, chunk_size);

        while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

        if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
        {
            Error_Handler();
        }

        offset += chunk_size;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        // Check for Enter key (carriage return or newline)
        if (rx_byte == '\n' || rx_byte == '\r')
        {
            uart_rx_buffer[rx_index] = '\0';
            if (rx_index > 0)
            {
                message_ready = 1;
            }
            rx_index = 0;

            // Print a newline so the terminal looks clean
            uint8_t newline[] = "\r\n";
            HAL_UART_Transmit(&huart2, newline, 2, 10);
        }
        else
        {
            // Echo the character back so you can see what you are typing
            HAL_UART_Transmit(&huart2, &rx_byte, 1, 10);

            if (rx_index < 99)
            {
                uart_rx_buffer[rx_index++] = rx_byte;
            }
        }

        // Re-arm interrupt
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
    {
        for(int i = 0; i < RxHeader.DLC; i++)
        {
            can_rx_buffer[can_rx_index++] = RxData[i];

            // If we hit null terminator OR buffer is full
            if(RxData[i] == '\0' || can_rx_index >= 99)
            {
                char printBuf[150];
                sprintf(printBuf, "\r\n[CAN RX from 0x%lX]: %s\r\n", RxHeader.StdId, can_rx_buffer);
                HAL_UART_Transmit(&huart2, (uint8_t*)printBuf, strlen(printBuf), 100);

                can_rx_index = 0;
                break;
            }
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
