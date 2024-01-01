#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#define FASTLED_INTERNAL
#include <FastLED.h>

#include <string>
#include <vector>
#include <memory>

#define PIN_0  0
#define PIN_2  2
#define PIN_4  4
#define PIN_5  5
#define PIN_12 12
#define PIN_13 13
#define PIN_15 15
#define PIN_16 16
#define PIN_17 17
#define PIN_18 18
#define PIN_19 19
#define PIN_21 21
#define PIN_22 22
#define PIN_23 23
#define PIN_25 25
#define PIN_26 26
#define PIN_27 27
#define PIN_32 32
#define PIN_33 33
#define PIN_35 35
#define PIN_36 36
#define PIN_37 37
#define PIN_38 38
#define PIN_39 39

#define SCREEN_PRIORITY ( tskIDLE_PRIORITY + 3 )
#define LED_PRIORITY    ( tskIDLE_PRIORITY + 3 )

#define ANY_CORE    tskNO_AFFINITY;
#define LED_CORE    0
#define SCREEN_CORE 1
