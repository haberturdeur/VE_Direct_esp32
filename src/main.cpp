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



extern "C" void app_main()
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
    ESP_LOGI(TAG,"starting");
    VEDirect com;
    com.init(UART_NUM_2, 2, 12);
    ESP_LOGI(TAG, "Started");
    while(1){
        if(com.read()>0)
            printf("Voltage: %i\n", com["V"]);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}