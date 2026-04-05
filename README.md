# esp32-uart-xor

Lekka biblioteka UART dla ESP32 z prostą obsługą ramek diagnostycznych i sumą kontrolną XOR.

## Co zawiera
- inicjalizację UART z gotowymi domyślnymi parametrami
- pomocniczą funkcję `checksum()`
- proste ramki diagnostyczne i start komunikacji

## Pliki
- `include/uart.h`
- `src/uart.c`

## Szybki start
```c
#include "uart.h"

void app_main(void)
{
    initUART();
    const char msg[] = "Hello from ESP32\r\n";
    uart_write_bytes(UART_PORT, msg, sizeof(msg) - 1);
}
```

## Domyślna konfiguracja
- baudrate: `921600`
- port: `UART_NUM_0`
- bufor RX/TX: `16384`

Pełny przykład znajdziesz w `examples/basic_usage/basic_usage.c`.
