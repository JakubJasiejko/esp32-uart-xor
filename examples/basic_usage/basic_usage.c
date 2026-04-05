#include "uart.h"

void app_main(void)
{
    initUART();
    const char text[] = "UART component ready\r\n";
    uart_write_bytes(UART_PORT, text, sizeof(text) - 1);
}
