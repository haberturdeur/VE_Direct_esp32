#include "VEDirect.hpp"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

VEDirect com;

extern "C" void app_main()
{
    com.init(UART_NUM_1, 14, 27);
    printf("Voltage: %i", com["V"]);
}