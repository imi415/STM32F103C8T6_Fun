#include <stdint.h>

#include "tim.h"
#include "gpio.h"

#include "user_app.h"

uint16_t half_tick_ms = 200;
uint8_t is_ready = 0;
                      //    0    1    2    3    4    5    6    7    8
                      //    !    C    D    E    F    G    A    B    C
uint16_t note_freq[] = { 65534, 262, 294, 330, 349, 392, 440, 494, 523};
uint8_t note_list[] =   { 5, 5, 6, 5, 3, 1, 2, 3, 2, 5, 5, 4, 3, 2,
                          5, 5, 3, 1, 6, 7, 8, 6, 5, 6, 5, 4, 3, 2,
                          1, 0, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8,
                          6, 5, 0, 8, 6, 4, 3, 3, 6, 5, 4, 3, 2, 1,
                          1, 0
                        };
uint8_t note_length[] = { 3, 1, 2, 2, 2, 1, 1, 4, 3, 1, 2, 1, 1, 8,
                          3, 1, 2, 2, 3, 1, 2, 2, 3, 1, 1, 1, 1, 1,
                          3, 1, 3, 1, 2, 2, 1, 1, 1, 1, 4, 3, 1, 2,
                          2, 3, 1, 4, 2, 2, 3, 1, 2, 2, 2, 2, 3, 1,
                          4, 4
                        };
uint8_t note_index = 58;

uint16_t led_map[] = { GPIO_PIN_10, GPIO_PIN_9, GPIO_PIN_8, GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_0 };

void _enable_output(void);
void _disable_output(void);
void _reconfigure_pwm(uint16_t freq);
void _reconfigure_channel(uint16_t ar_value);
void _play_note(uint8_t index);

void setup(void) {
  _disable_output();
}

void loop(void) {
  if (is_ready) {
    for(uint8_t i = 0; i < note_index; i++) {
      _play_note(i);
    }
    is_ready = 0;
  }
}

void _play_note(uint8_t index) {
  _reconfigure_pwm(note_freq[note_list[index]]);
  _enable_output();
  HAL_GPIO_WritePin(GPIOB, led_map[note_list[index]], GPIO_PIN_RESET);
  HAL_Delay(note_length[index] * half_tick_ms);
  HAL_GPIO_WritePin(GPIOB, led_map[note_list[index]], GPIO_PIN_SET);
  _disable_output();
  HAL_Delay(20);
}

void _enable_output(void) {
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void _disable_output(void) {
  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}

void _reconfigure_pwm(uint16_t freq) {
  _disable_output();
  uint16_t ar = 1000000 / freq;
  __HAL_TIM_SET_AUTORELOAD(&htim2, ar);
  _reconfigure_channel(ar);
  _enable_output();
}

void _reconfigure_channel(uint16_t ar_value) {
  TIM_OC_InitTypeDef cfg;
  cfg.OCMode = TIM_OCMODE_PWM1;
  cfg.Pulse = ar_value / 2;
  cfg.OCPolarity = TIM_OCPOLARITY_HIGH;
  cfg.OCFastMode = TIM_OCFAST_ENABLE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &cfg, TIM_CHANNEL_1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if(GPIO_Pin == GPIO_PIN_1) {
    is_ready = 1;
  }
}
