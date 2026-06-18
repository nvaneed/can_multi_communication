STM32 CAN Chat Network: Node Setup Guide
This guide explains how to configure STM_2 and STM_3 for the 3-way CAN communication network. Since the core logic is identical across all nodes, you only need to duplicate the base STM_1 code and modify two specific parameters for each additional board: the Hardware Filter ID and the Boot Message.

Configuration for STM_2
Copy the base STM_1 code into your STM_2 project and make the following two changes:

1. Update the Hardware Filter
Scroll down to the CAN_Filter_Config function (around line 191) and update the FilterIdHigh to 0x102. This hardware-level filter tells STM_2 to only let messages through that are specifically addressed to it.

C
    /* Hardware Filter specifically for STM_2 (Accepts ONLY 0x102) */
    canfilterconfig.FilterIdHigh = 0x102 << 5; 
2. Update the Boot Message
Locate your main() function (around line 99) and modify the welcome text so you can easily identify which serial terminal you are looking at.

C
  /* Boot Message */
  char startMsg[] = "\r\n=== STM_2 Node Initialized ===\r\nType a message for stm_1 or stm_3...\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)startMsg, strlen(startMsg), 100);
Configuration for STM_3
Similarly, copy the base code into your STM_3 project and apply these two changes:

1. Update the Hardware Filter
Update the filter ID to 0x103 so the board listens for its unique address.

C
    /* Hardware Filter specifically for STM_3 (Accepts ONLY 0x103) */
    canfilterconfig.FilterIdHigh = 0x103 << 5; 
2. Update the Boot Message
Change the welcome text to reflect STM_3.

C
  /* Boot Message */
  char startMsg[] = "\r\n=== STM_3 Node Initialized ===\r\nType a message for stm_1 or stm_2...\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)startMsg, strlen(startMsg), 100);
Architecture: Why does the rest of the code stay the same?
You do not need to alter the message-sending logic because the SendDynamicMessageViaCAN function already contains the routing map for all three IDs:

C
    if (strstr(temp_msg, "stm_1") != NULL) target_id = 0x101;
    else if (strstr(temp_msg, "stm_2") != NULL) target_id = 0x102;
    else if (strstr(temp_msg, "stm_3") != NULL) target_id = 0x103;
Because this mapping is hardcoded and identical across all three boards:

STM_2 inherently knows that if you type "stm_3" into its terminal, it needs to set the CAN transmission ID to 0x103.

STM_3's hardware filter is actively listening for 0x103, meaning it will automatically catch and process the message.

Final Step: Once you make these two minor tweaks for each board, compile and flash them. Your 3-way CAN chat network will be fully operational!
