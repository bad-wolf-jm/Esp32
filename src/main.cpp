#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <vector>
#include <string>

#define FASTLED_INTERNAL
#include <FastLED.h>

#include "Framebuffer/framebuffer2D.h"
#include "Framebuffer/LedStrip.h"
#include "Effects/Glow2D.h"

TFT_eSPI gDisplay;

const uint32_t BUTTON_1_PIN = 35;
const uint32_t BUTTON_2_PIN = 0;

const uint32_t BUTTON_PRESS_THRESHOLD = 30;

volatile uint32_t gButton1LastPress;
void IRAM_ATTR Button1Pressed()
{
    if (millis() - gButton1LastPress > BUTTON_PRESS_THRESHOLD)
    {
        gButton1LastPress = millis();
        Serial.println("Button 1 Pressed");
    }
}

volatile int gIndexToDisplay = -1;
volatile uint32_t gButton2LastPress;
void IRAM_ATTR Button2Pressed()
{
    if (millis() - gButton2LastPress > BUTTON_PRESS_THRESHOLD)
    {
        gButton2LastPress = millis();
        gIndexToDisplay = random(' ', 16);
    }
}

const char *gPuzzleElements[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"};

#define NUM_LEDS 256
#define DATA_PIN 25
CRGB leds[NUM_LEDS];

// matrix_t led_matrix(16, 16, DataFlowOrigin::TOP_LEFT, DataFlowDirection::VERTICAL);
// framebuffer2D_t led_matrix(16, 16, DataFlowOrigin::TOP_RIGHT, DataFlowDirection::HORIZONTAL);

LedStrip led_strip(144u, true);
Glow2D glow_effect(144u);

// matrix_t led_matrix(16, 16, DataFlowOrigin::BOTTOM_LEFT, DataFlowDirection::HORIZONTAL);
// matrix_t led_matrix(16, 16, DataFlowOrigin::BOTTOM_RIGHT, DataFlowDirection::VERTICAL);
std::string pulsar_data = 
    ".................."
    ".................."
    "....OOO...OOO....."
    ".................."
    "..O....O.O....O..."
    "..O....O.O....O..."
    "..O....O.O....O..."
    "....OOO...OOO....."
    ".................."
    "....OOO...OOO....."
    "..O....O.O....O..."
    "..O....O.O....O..."
    "..O....O.O....O..."
    ".................."
    "....OOO...OOO....."
    ".................."
    ".................."
    "..................";



// std::string pulsar_data = 
//     ".................."
//     ".................."
//     "...O..O...O...O..."
//     "..O.O.O...O..O.O.."
//     "..OOO.O...O..O.O.."
//     "..O.O.OOO.OOO.O..."
//     ".................."
//     ".................."
//     ".O..O.O..OO.O.OOO."
//     ".O..O.O.O...O.O..."
//     ".O..O.O.O...O.OOO."
//     ".O..O.O.O...O.O..."
//     ".OOOOOO..OO.O.OOO."
//     ".................."
//     ".................."
//     ".................."
//     ".................."
//     "..................";

// char data[] = {
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
//     ' ', ' ', '*', ' ', '*', ' ', ' ', '*', ' ', ' ', ' ', '*', ' ', ' ', ' ', ' ',
//     ' ', '*', ' ', '*', '*', ' ', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ',
//     ' ', '*', '*', '*', '*', ' ', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ',
//     ' ', '*', ' ', '*', '*', ' ', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ',
//     ' ', '*', ' ', '*', '*', '*', '*', '*', '*', '*', ' ', '*', ' ', ' ', ' ', ' ',
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
//     ' ', '*', ' ', ' ', '*', ' ', '*', ' ', '*', '*', ' ', '*', ' ', '*', '*', '*',
//     ' ', '*', ' ', ' ', '*', ' ', '*', '*', ' ', ' ', ' ', '*', ' ', '*', ' ', ' ',
//     ' ', '*', ' ', ' ', '*', ' ', '*', '*', ' ', ' ', ' ', '*', ' ', '*', '*', '*',
//     ' ', '*', ' ', ' ', '*', ' ', '*', '*', ' ', ' ', ' ', '*', ' ', '*', ' ', ' ',
//     ' ', '*', '*', '*', '*', '*', '*', ' ', '*', '*', ' ', '*', ' ', '*', '*', '*',
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
//     ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
// };

#define GRID_SIZE 16
uint8_t state_0[(GRID_SIZE + 2) * (GRID_SIZE + 2)] = {0};
uint8_t state_1[(GRID_SIZE + 2) * (GRID_SIZE + 2)] = {0};
uint8_t *front = state_1;
uint8_t *back = state_0;
std::vector<CRGB> pixelValues(16*16);

void Update(uint8_t *front, uint8_t *back)
{
    // bool gridTwo[GRID_SIZE + 1][GRID_SIZE + 1] = {};
    // compareGrid(gridOne, gridTwo);
    memcpy(front, back, (GRID_SIZE+2) * (GRID_SIZE+2) * sizeof(uint8_t));

    for (int a = 1; a < GRID_SIZE+1; a++)
    {
        for (int b = 1; b < GRID_SIZE+1; b++)
        {
            int alive = 0;

            uint32_t n00 = (a - 1) * (GRID_SIZE + 2) + (b - 1);
            alive += (back[n00] > 0) ? 1 : 0;

            uint32_t n01 = (a - 1) * (GRID_SIZE + 2) + (b);
            alive += (back[n01] > 0) ? 1 : 0;

            uint32_t n02 = (a - 1) * (GRID_SIZE + 2) + (b + 1);
            alive += (back[n02] > 0) ? 1 : 0;

            uint32_t n10 = (a) * (GRID_SIZE + 2) + (b - 1);
            alive += (back[n10] > 0) ? 1 : 0;

            // uint32_t n11 = 0;
            uint32_t n12 = (a) * (GRID_SIZE + 2) + (b + 1);
            alive += (back[n12] > 0) ? 1 : 0;

            uint32_t n20 = (a + 1) * (GRID_SIZE + 2) + (b - 1);
            alive += (back[n20] > 0) ? 1 : 0;

            uint32_t n21 = (a + 1) * (GRID_SIZE + 2) + (b);
            alive += (back[n21] > 0) ? 1 : 0;

            uint32_t n22 = (a + 1) * (GRID_SIZE + 2) + (b + 1);
            alive += (back[n22] > 0) ? 1 : 0;


            if ((alive < 2) || (alive > 3))
            {
                front[a * (GRID_SIZE + 2) + b] = 0;
            }
            else if(alive == 3)
            {
                front[a * (GRID_SIZE + 2) + b] = 1;
            }
        }
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println("");
    Serial.println("");
    Serial.println("==========================================");
    Serial.println("|                TEST                    |");
    Serial.println("==========================================");

    gDisplay.init();
    gDisplay.setRotation(1);
    gDisplay.fillScreen(TFT_BLUE);

    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);

    gButton1LastPress = gButton2LastPress = 0;
    attachInterrupt(BUTTON_1_PIN, Button1Pressed, FALLING);
    attachInterrupt(BUTTON_2_PIN, Button2Pressed, FALLING);

    // for (int i=0; i < (GRID_SIZE+2)*(GRID_SIZE+2); i++)
    // {
    //     state_0[i] = pulsar_data[i] == 'O' ? 1 : 0;
    // }

    // for (int i=0; i < (GRID_SIZE+2)*(GRID_SIZE+2); i++)
    // {
    //     state_0[i] = (uint8_t)random(2);
    // }


    // for (uint32_t i = 0; i < 16; i++)
    // {
    //     for (uint32_t j = 0; j < 16; j++)
    //     {
    //         char v = back[(i+1) * (GRID_SIZE+2) + (j+1)];
    //         pixelValues[i * 16 + j] = (v == 0) ? CRGB::Black : CRGB::White;
    //     }
    // }

    // led_matrix.Clear();
    // led_matrix.SetPixels(pixelValues);
    // led_matrix.Present();
    // delay(1000);

    // Update(front, back);

    // for (uint32_t i = 0; i < 16; i++)
    // {
    //     for (uint32_t j = 0; j < 16; j++)
    //     {
    //         char v = front[(i+1) * (GRID_SIZE+2) + (j+1)];
    //         pixelValues[i * 16 + j] = (v == 0) ? CRGB::Black : CRGB::White;
    //     }
    // }

    // led_matrix.Clear();
    // led_matrix.SetPixels(pixelValues);
    // led_matrix.Present();


    // FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
    // FastLED.setBrightness(8);
#if 0
    // gDisplay.setTextSize(3);

    // gDisplay.setCursor(1' ', 3' ', 2);
    // gDisplay.setTextColor(TFT_SKYBLUE);
    // gDisplay.println("Allo");

    // gDisplay.setCursor(2' ', 6' ', 2);
    // gDisplay.setTextColor(TFT_PINK);
    // gDisplay.println("LUCIE");

    WiFi.mode(WIFI_STA); // Set as station
    // WiFi.mode(WIFI_AP);
    // WiFi.mode(WIFI=_AP_STA);
    WiFi.disconnect(); // disconnect from network
    delay(100);

#endif
}

void loop()
{
    static int position = 0;
    // gDisplay.fillScreen(TFT_BLUE);
    // gDisplay.setCursor(2' ', 4' ', 2);
    // gDisplay.setTextColor(TFT_PINK);
    // gDisplay.setTextSize(5);

    // if (gIndexToDisplay != -1)
    // {
    //     gDisplay.println(gPuzzleElements[gIndexToDisplay]);
    // }

    // delay(100);
    // FastLED.clear(false);
    // leds[position] = CRGB::White;
    // position = (position + 1) % NUM_LEDS;
    // FastLED.show();
    // delay(30);

    led_strip.Clear();
    glow_effect.Update();
    led_strip.SetPixels(glow_effect.GetColors());
    // led_strip.SetPixel(position, CRGB::Magenta);
    // position = (position + 1) % 144;
    led_strip.Present();
    delay(50);
#if 0
    Update(front, back);

    for (uint32_t i = 0; i < 16; i++)
    {
        for (uint32_t j = 0; j < 16; j++)
        {
            char v = front[(i+1) * (GRID_SIZE+2) + (j+1)];
            pixelValues[i * 16 + j] = (v == 0) ? CRGB::Black : CRGB::White;
        }
    }

    std::swap(front, back);

    led_matrix.Clear();
    led_matrix.SetPixels(pixelValues);
    led_matrix.Present();
    delay(50);
#endif
    // for (uint32_t i = 0; i < 16; i++)
    // {
    //     for (uint32_t j = 0; j < 16; j++)
    //     {
    //         // char v = data[i * 16 + j];
    //         // led_matrix.SetPixel(i, j, v == ' ' ? CRGB::Black : CRGB::White);
    //         // char v = data[i * 16 + j];
    //         led_matrix.Clear();
    //         led_matrix.SetPixel(i, j, CRGB::White);
    //         // delay(20);
    //         led_matrix.Present();
    //     }
    // }

#if 0
int n = WiFi.scanNetworks();

    if (n > 0)
    {
        char buffer[256];
        gDisplay.fillScreen(TFT_BLUE);
        sprintf(buffer, "There are %d networks", n);
        gDisplay.setCursor(2' ', 1' ', 2);
        gDisplay.setTextColor(TFT_PINK);
        gDisplay.println(buffer);

        for (uint32_t i = 0; i < n; i++)
        {
            char buffer[256];
            sprintf(buffer, "SSID: %s - Strength %d", WiFi.SSID(i), WiFi.RSSI(i));
            gDisplay.setCursor(2' ', 15 * (i + 2), 2);
            gDisplay.setTextColor(TFT_PINK);
            gDisplay.println(buffer);
        }
    }
#endif

    // Serial.println("Hello!!");
}
