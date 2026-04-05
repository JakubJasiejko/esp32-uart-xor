# esp32-uart-xor

A lightweight UART helper for ESP32 with simple diagnostic frames and XOR checksum support.

## Features
- UART initialization with sensible default settings
- helper `checksum()` function
- simple diagnostic frames and startup handshake

## Files
- `include/uart.h`
- `src/uart.c`

## Quick start
```c
#include "uart.h"

void app_main(void)
{
    initUART();
    const char msg[] = "Hello from ESP32\r\n";
    uart_write_bytes(UART_PORT, msg, sizeof(msg) - 1);
}
```

## Default configuration
- baudrate: `921600`
- port: `UART_NUM_0`
- bufor RX/TX: `16384`

See `examples/basic_usage/basic_usage.c` for a complete example.
