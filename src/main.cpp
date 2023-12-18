#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <string>
#include <vector>

#define FASTLED_INTERNAL
#include <FastLED.h>

#include "Effects/Glow2D.h"
#include "Framebuffer/LedMatrix.h"
#include "Framebuffer/LedStrip.h"

TFT_eSPI gDisplay;

const uint32_t BUTTON_1_PIN = 35;
const uint32_t BUTTON_2_PIN = 0;

const uint32_t BUTTON_PRESS_THRESHOLD = 30;

// matrix_t led_matrix(16, 16, DataFlowOrigin::TOP_LEFT, DataFlowDirection::VERTICAL);
// LedMatrix led_matrix(16, 16, DataFlowOrigin::TOP_RIGHT, DataFlowDirection::HORIZONTAL);

// LedStrip<17, GRB> led_strip(256u * 3, false);
LedMatrix<16, 16, 3, 1, 17, GRB> led_matrix;
Glow2D             glow_effect( 256u * 3 );

void setup()
{
    Serial.begin( 9600 );
    Serial.println( "" );
    Serial.println( "" );
    Serial.println( "==========================================" );
    Serial.println( "|                TEST                    |" );
    Serial.println( "==========================================" );

    gDisplay.init();
    gDisplay.setRotation( 1 );
    gDisplay.fillScreen( TFT_BLUE );

    pinMode( BUTTON_1_PIN, INPUT_PULLUP );
    pinMode( BUTTON_2_PIN, INPUT_PULLUP );
}

void loop()
{
    static int position_x = 0;
    static int position_y = 0;

    EVERY_N_MILLISECONDS( 5 )
    {
        if( ( position_x == 0 ) && ( position_y == 0 ) )
            led_matrix.Clear();
        led_matrix.SetPixel( position_y, position_x, CRGB::DarkOrchid );
        position_x = ( position_x + 1 ) % led_matrix.Width;
        if( position_x == 0 )
            position_y = ( position_y + 1 )  % led_matrix.Height;
        led_matrix.Present();
    }

    // led_strip.Clear();
    // glow_effect.Update();
    // led_strip.SetPixels(glow_effect.GetColors());
    // led_strip.Present();

    // led_matrix.Clear();
    // glow_effect.Update();
    // led_matrix.SetPixels(glow_effect.GetColors());
    // led_matrix.Present();
    // delay(10);

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
