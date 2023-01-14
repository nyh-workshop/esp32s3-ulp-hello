/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/rtc_io.h"

#include "soc/soc.h"
#include "soc/gpio_periph.h"
#include "hal/gpio_hal.h"

#include "ulp.h"
#include "ulp_main.h"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

static void init_ulp_program(void);

void app_main(void)
{
    init_ulp_program();

    printf("Hello World!\n");

    // Note:
    // For Cytron Maker Feather AIOT S3 users, please enable GPIO11 to use the diagnostic LEDs!
   
    esp_rom_gpio_pad_select_gpio(GPIO_NUM_14);
    esp_rom_gpio_pad_select_gpio(GPIO_NUM_39);
    esp_rom_gpio_pad_select_gpio(GPIO_NUM_11);

    gpio_set_direction(GPIO_NUM_39, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_11, GPIO_MODE_OUTPUT);
    
    gpio_set_level(GPIO_NUM_11, 1);    
    gpio_set_level(GPIO_NUM_39, 1);
    
    while (1)
    {
        gpio_set_level(GPIO_NUM_39, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_39, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void init_ulp_program(void)
{
    esp_err_t err = ulp_load_binary(0, ulp_main_bin_start,
            (ulp_main_bin_end - ulp_main_bin_start) / sizeof(uint32_t));
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_14));
    rtc_gpio_set_direction(GPIO_NUM_21, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_pullup_dis(GPIO_NUM_14);
    rtc_gpio_pulldown_dis(GPIO_NUM_14);

    err = ulp_run(&ulp_entry - RTC_SLOW_MEM);
    ESP_ERROR_CHECK(err);
}