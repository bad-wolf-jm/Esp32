#pragma once

#include "Common.h"

constexpr uint8_t GPIO_PIN_0  = PIN_0;
constexpr uint8_t GPIO_PIN_2  = PIN_2;
constexpr uint8_t GPIO_PIN_12 = PIN_12;
constexpr uint8_t GPIO_PIN_13 = PIN_13;
constexpr uint8_t GPIO_PIN_15 = PIN_15;
constexpr uint8_t GPIO_PIN_25 = PIN_25;
constexpr uint8_t GPIO_PIN_26 = PIN_26;
constexpr uint8_t GPIO_PIN_27 = PIN_27;
constexpr uint8_t GPIO_PIN_32 = PIN_32;
constexpr uint8_t GPIO_PIN_33 = PIN_33;
constexpr uint8_t GPIO_PIN_35 = PIN_35;
constexpr uint8_t GPIO_PIN_36 = PIN_36;
constexpr uint8_t GPIO_PIN_37 = PIN_37;
constexpr uint8_t GPIO_PIN_38 = PIN_38;
constexpr uint8_t GPIO_PIN_39 = PIN_39;

// constexpr uint8_t ADC_CHANNEL_0  = PIN_36;
// constexpr uint8_t ADC_CHANNEL_3  = PIN_39;
// constexpr uint8_t ADC_CHANNEL_4  = PIN_32;
// constexpr uint8_t ADC_CHANNEL_5  = PIN_33;
// constexpr uint8_t ADC_CHANNEL_12 = PIN_2;
// constexpr uint8_t ADC_CHANNEL_13 = PIN_15;
// constexpr uint8_t ADC_CHANNEL_14 = PIN_13;
// constexpr uint8_t ADC_CHANNEL_15 = PIN_12;
// constexpr uint8_t ADC_CHANNEL_17 = PIN_27;
// constexpr uint8_t ADC_CHANNEL_18 = PIN_25;
// constexpr uint8_t ADC_CHANNEL_19 = PIN_26;

constexpr uint8_t TOUCH_CHANNEL_2 = PIN_2;
constexpr uint8_t TOUCH_CHANNEL_3 = PIN_15;
constexpr uint8_t TOUCH_CHANNEL_4 = PIN_13;
constexpr uint8_t TOUCH_CHANNEL_5 = PIN_15;
constexpr uint8_t TOUCH_CHANNEL_7 = PIN_27;
constexpr uint8_t TOUCH_CHANNEL_8 = PIN_33;
constexpr uint8_t TOUCH_CHANNEL_9 = PIN_32;

constexpr uint32_t BUTTON_1_PIN = PIN_35;
constexpr uint32_t BUTTON_2_PIN = PIN_0;

constexpr uint8_t LED_PINS[] = {
    PIN_2, PIN_12, PIN_13, PIN_15, PIN_17, PIN_21, PIN_22, PIN_25, PIN_26, PIN_27, PIN_32, PIN_33,
};

constexpr uint8_t LED_CHANNEL_0 = PIN_2;
constexpr uint8_t LED_CHANNEL_1 = PIN_12;
constexpr uint8_t LED_CHANNEL_2 = PIN_13;
constexpr uint8_t LED_CHANNEL_3 = PIN_15;
constexpr uint8_t LED_CHANNEL_4 = PIN_17;
constexpr uint8_t LED_CHANNEL_5 = PIN_21;
constexpr uint8_t LED_CHANNEL_6 = PIN_22;
constexpr uint8_t LED_CHANNEL_7 = PIN_25;

constexpr uint16_t DEFAULT_STACK_SIZE = 2048 + 512;
constexpr uint16_t IDLE_STACK_SIZE    = 2048;
constexpr uint16_t DRAWING_STACK_SIZE = 4096;
constexpr uint16_t AUDIO_STACK_SIZE   = 4096;
constexpr uint16_t JSON_STACK_SIZE    = 4096;
constexpr uint16_t SOCKET_STACK_SIZE  = 4096;
constexpr uint16_t NET_STACK_SIZE     = 8192;
constexpr uint16_t DEBUG_STACK_SIZE   = 8192;
constexpr uint16_t REMOTE_STACK_SIZE  = 4096;
