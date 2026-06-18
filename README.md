#  STM32 CAN Chat Network: Node Setup Guide

This guide explains how to configure **STM_2** and **STM_3** for the 3-way CAN communication network. Since the core logic is identical across all nodes, you only need to duplicate the base `STM_1` code and modify two specific parameters for each additional board: the **Hardware Filter ID** and the **Boot Message**.

---

##  Configuration for STM_2

Copy the base `STM_1` code into your STM_2 `main.c` file and make the following two changes:

### 1. Update the Hardware Filter
Scroll down to the `CAN_Filter_Config` function (around line 191) and update the `FilterIdHigh` to **`0x102`**. This hardware-level filter tells STM_2 to only let messages through that are specifically addressed to it.

```c
    /* Hardware Filter specifically for STM_2 (Accepts ONLY 0x102) */
    canfilterconfig.FilterIdHigh = 0x102 << 5; 
```

### 2. Update the Boot Message
Locate your `main()` function (around line 99) and modify the welcome text so you can easily identify which serial terminal you are looking at.

```c
  /* Boot Message */
  char startMsg[] = "\r\n=== STM_2 Node Initialized ===\r\nType a message for stm_1 or stm_3...\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)startMsg, strlen(startMsg), 100);
```

---

##  Configuration for STM_3

Similarly, copy the base code into your STM_3 project and apply these two changes:

### 1. Update the Hardware Filter
Update the filter ID to **`0x103`** so the board listens for its unique address.

```c
    /* Hardware Filter specifically for STM_3 (Accepts ONLY 0x103) */
    canfilterconfig.FilterIdHigh = 0x103 << 5; 
```

### 2. Update the Boot Message
Change the welcome text to reflect STM_3.

```c
  /* Boot Message */
  char startMsg[] = "\r\n=== STM_3 Node Initialized ===\r\nType a message for stm_1 or stm_2...\r\n";
  HAL_
