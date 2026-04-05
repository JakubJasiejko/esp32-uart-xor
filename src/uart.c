/**
 * @file uart.c
 * @author Jakub Jasiejko
 * @date 2025-03-15
 * @brief UART-USB communication helpers with XOR checksum support.
 *
 * @details
 * This module provides a simple UART transport with an XOR checksum,
 * intended for basic command exchange, diagnostic output, and error reporting
 * over a USB-UART connection.
 */

 #include "uart.h"
 #include <stdio.h>
 #include <string.h>
 #include "sdkconfig.h"
 
 /**
  * @brief Computes the XOR checksum for two bytes.
  *
  * This helper is used to protect a simple three-byte frame by XORing
  * the first two bytes.
  *
  * @param byte1 First byte.
  * @param byte2 Second byte.
  * @return uint8_t Bitwise XOR checksum.
  */
 uint8_t checksum(uint8_t byte1, uint8_t byte2) {
     return byte1 ^ byte2;
 }
 
 /**
  * @brief Initializes the UART peripheral using the defaults from `uart.h`.
  *
  * Configuration:
  * - configured baud rate
  * - 8 data bits
  * - 1 stop bit
  * - no parity
  * - no hardware flow control
  */
 void initUART() {
     uart_config_t uart_config = {
         .baud_rate = UART_BAUDRATE,
         .data_bits = UART_DATA_8_BITS,
         .parity = UART_PARITY_DISABLE,
         .stop_bits = UART_STOP_BITS_1,
         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
         .source_clk = UART_SCLK_APB,
     };
 
     uart_driver_install(UART_PORT, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
     uart_param_config(UART_PORT, &uart_config);
     uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                  UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
 }
 
 /**
  * @brief Starts the UART handshake with a host device such as a PC.
  *
  * The function waits for the frame `[0xAA][0x01][CHECKSUM]` and replies with:
  * - `[0xAA][0x02][CHECKSUM]` on success
  * - no reply when the frame is invalid
  * - `[0xAA][0xFF][CHECKSUM]` on timeout
  */
 void beginSerialCommunication() {
     initUART();
 
     while (true) {
         #ifdef UART_DEBUG
             uart_write_bytes(UART_PORT, "UART init\n", 10);
         #endif
 
         uint8_t received_data[3];
         uint8_t startByte = 0xAA;
         uint8_t comStartByte = 0x01;
         uint8_t response_byte = 0x02;
         uint8_t error_byte = 0xFF;
 
         int64_t start_time = esp_timer_get_time();
 
         #ifdef UART_DEBUG
            uart_write_bytes(UART_PORT, "Waiting for 0xAA 0x01...\n", 25);
         #endif
 
         while ((esp_timer_get_time() - start_time) / 1000 < 10000) {
             int len = uart_read_bytes(UART_PORT, received_data, 3, pdMS_TO_TICKS(100));
 
             if (len == 3) {
                 uint8_t calculatedChecksum = checksum(received_data[0], received_data[1]);
 
                 #ifdef UART_DEBUG
                    uart_write_bytes(UART_PORT, "Received: ", 10);
                     uart_write_bytes(UART_PORT, received_data, 3);
                     uart_write_bytes(UART_PORT, "\n", 1);
                 #endif
 
                 if (received_data[0] == startByte &&
                     received_data[1] == comStartByte &&
                     received_data[2] == calculatedChecksum) {
 
                     uint8_t responseChecksum = checksum(startByte, response_byte);
 
                     uart_write_bytes(UART_PORT, &startByte, 1);
                     uart_write_bytes(UART_PORT, &response_byte, 1);
                     uart_write_bytes(UART_PORT, &responseChecksum, 1);
 
                     #ifdef UART_DEBUG
                        uart_write_bytes(UART_PORT, "\nAcknowledgment sent (0x02)\n", 30);
                     #endif
                     return;
                 } else {
                     #ifdef UART_DEBUG
                        uart_write_bytes(UART_PORT, "Checksum error!\n", 16);
                     #endif
                 }
             }
         }
 
        // Timeout: no valid frame received
         uint8_t timeoutSum = checksum(startByte, error_byte);
 
         uart_write_bytes(UART_PORT, &startByte, 1);
         uart_write_bytes(UART_PORT, &error_byte, 1);
         uart_write_bytes(UART_PORT, &timeoutSum, 1);
 
         #ifdef UART_DEBUG
            uart_write_bytes(UART_PORT, "\nTimeout: no response, sent 0xFF\n", 34);
         #endif
     }
 }
 
 /**
  * @brief Sends an error frame over UART in the format `[0xAA][errorMask][checksum]`.
  *
  * @param errorMask Error code to send, for example `0xCC` or `0xDD`.
  */
 void errorOnUART(uint8_t errorMask) {
     uint8_t startByte = 0xAA;
     uint8_t sum = checksum(startByte, errorMask);
 
     uart_write_bytes(UART_PORT, &startByte, 1);
     uart_write_bytes(UART_PORT, &errorMask, 1);
     uart_write_bytes(UART_PORT, &sum, 1);
 }
 
 /**
  * @brief Sends a 16-bit value as an ASCII binary string over UART.
  *
  * Useful for quick register and configuration debugging.
  *
  * @param value 16-bit value to send.
  */
 void binaryDebug(uint16_t value) {
     char binary_str[17];
 
     for (int i = 15; i >= 0; i--) {
         binary_str[15 - i] = (value & (1 << i)) ? '1' : '0';
     }
     binary_str[16] = '\0';
 
     uart_write_bytes(UART_PORT, binary_str, 16);
 }
 
