#include <stdio.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PWM_GPIO GPIO_NUM_4

// At 50Hz frequency
// ~1ms pulse width.
#define MIN_DUTY_CYCLE 215
// ~2ms pulse width.
#define MAX_DUTY_CYCLE 415

#define DUTY_CYCLE 300

void calibrate_esc(const uint32_t min_duty_cycle, const uint32_t max_duty_cycle)
{
    uint32_t duty = max_duty_cycle;
    while (duty > MIN_DUTY_CYCLE)
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(15));
        --duty;
    }
}

void app_main(void)
{
    // Initialize ledc_timer_config_t.
    //      set speed mode : ledc_mode_t
    //      timer number   : ledc_timer_t
    //      pwm frequency in Hz
    //      resolution of pwm duty
    //      source clock   : ledc_clk_cfg_t
    ledc_timer_config_t timer = 
    {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK,
    };


    // Initialize ledc_channel_config_t.
    //      do so with the frequency specified in timer settings
    //      and the given duty cycle.
    ledc_channel_config_t channel =
    {
        .gpio_num = PWM_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = MIN_DUTY_CYCLE, // range of duty is [0, (2**duty_resoluion).
        .hpoint = 0, // range of hpoint is [0, (2**duty_resolution) - 1 ]
    };

    // Call led timer config function.
    ledc_timer_config(&timer);
    // Call led channel config function.
    ledc_channel_config(&channel);
    
    vTaskDelay(pdMS_TO_TICKS(10000));

    uint32_t duty = ((MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) / 2) + MIN_DUTY_CYCLE;
    while (1)
    {
        while (duty < MAX_DUTY_CYCLE )
        {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(15));
            ++duty;
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
        while (duty > ((MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) / 2) + MIN_DUTY_CYCLE)
        {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(15));
            --duty;
        }

    }

}
