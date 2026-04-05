/**
 * @file uart.h
 * @author eng. Jakub Jasiejko
 * @date 2025-03-15
 * @brief UART-USB communication header with XOR checksum support.
 *
 * @details
 * This header declares helper functions used to initialize the UART port,
 * start communication with a host, and send diagnostic or error messages.
 * Communication is based on a simple XOR checksum protocol for frame validation.
 */

 #ifndef UART_H
 #define UART_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "driver/uart.h"
 #include "esp_system.h"
 #include "esp_timer.h"
 #include "sdkconfig.h"

 #define UART_BAUDRATE        921600
 #define UART_PORT            UART_NUM_0
 #define UART_TX_PIN          GPIO_NUM_1
 #define UART_RX_PIN          GPIO_NUM_3
 #define UART_BUF_SIZE        16384
 #define UART_TIMEOUT_MS      10000

 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Computes the XOR checksum of two bytes.
  *
  * @param byte1 First byte
  * @param byte2 Second byte
  * @return uint8_t XOR checksum value
  */
 uint8_t checksum(uint8_t byte1, uint8_t byte2);
 
 /**
  * @brief Initializes the UART port using settings defined in this header.
  *
  * Transmission parameters:
  * - baud rate
  * - 8 data bits
  * - 1 stop bit
  * - no parity and no flow control
  */
 void initUART();
 
 /**
  * @brief Starts UART communication with a host computer or master device.
  *
  * Waits for the startup frame [0xAA][0x01][checksum], validates it,
  * and sends either an acknowledgment or a timeout error frame.
  */
 void beginSerialCommunication();
 
 /**
  * @brief Sends an error frame in the format [0xAA][errorMask][checksum].
  *
  * @param errorMask Error code sent over UART, for example 0xCC or 0xDD
  */
 void errorOnUART(uint8_t errorMask);
 
 /**
  * @brief Sends a 16-bit value over UART as an ASCII binary string.
  *
  * Useful for debugging configuration values such as FDC1004 registers.
  *
  * @param value 16-bit value to print in binary form
  */
 void binaryDebug(uint16_t value);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif // UART_H
 
