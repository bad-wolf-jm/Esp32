#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <string>
#include <vector>

#define FASTLED_INTERNAL
#include <FastLED.h>

#include "Display/Graphics.h"
#include "Effects/Glow2D.h"
#include "Effects/LaserLine.h"
#include "Effects/LedStripRenderer.h"
#include "Framebuffer/GFXBase.h"
#include "Framebuffer/LedMatrix.h"
#include "Framebuffer/LedStrip.h"
#include "TaskManager.h"
#include "Effects/SpectrumAnalyzer.h"
#include "Effects/Fire.h"

// #include "entt/entt.hpp"

std::unique_ptr<Graphics> gDisplay;
TaskManager               TM;

// TFT_eSPI tft;
// TFT_eSprite img = TFT_eSprite( &tft );

const uint32_t BUTTON_1_PIN = 35;
const uint32_t BUTTON_2_PIN = 0;

const uint32_t BUTTON_PRESS_THRESHOLD = 30;

// matrix_t led_matrix(16, 16, DataFlowOrigin::TOP_LEFT, DataFlowDirection::VERTICAL);
// LedMatrix led_matrix(16, 16, DataFlowOrigin::TOP_RIGHT, DataFlowDirection::HORIZONTAL);

LedStrip<17, GRB>  led_strip( 144, false );
LedStripRenderer   led_renderer( 1.0, 144 );
LedMatrix<21, GRB> led_matrix( 16, 16, 3, 1 );
GFXBase            gfx( 16, 16, 3, 1 );

Glow2D glow_effect( 256u * 3 );
FireEffect fire("Fire", 144, 3 );
SmoothFireEffect smooth_fire(144);

#define SCREEN_PRIORITY ( tskIDLE_PRIORITY + 3 )
#define SCREEN_CORE     1
#define LED_PRIORITY    ( tskIDLE_PRIORITY + 3 )
#define LED_CORE        1

TaskHandle_t _screenTaskHandle;
TaskHandle_t _ledTaskHandle;

void IRAM_ATTR UpdateScreen( void *param )
{
    long frameStart = 0, frameDuration = 0;
    for( ;; )
    {
        frameStart = millis();

        gDisplay->BeginFrame();
        gDisplay->DrawText( str_sprintf( "CPU:   %4.1f%% (%4.1f%% - %4.1f%%)", TM.GetCPUUsagePercent( -1 ), TM.GetCPUUsagePercent( 0 ),
                                         TM.GetCPUUsagePercent( 1 ) ),
                            5, 5 );
        gDisplay->DrawText( str_sprintf( "HEAP:  F: %.1fKB - T: %.1fKB", ESP.getFreeHeap() / 1024.0, ESP.getHeapSize() / 1024.0 ), 5,
                            20 );
        if( ESP.getPsramSize() > 0 )
            gDisplay->DrawText( str_sprintf( "PSRAM: F: %.1fKB - T: %.1fKB", ESP.getFreePsram() / 1024.0, ESP.getPsramSize() ), 5,
                                35 );
        else
            gDisplay->DrawText( str_sprintf( "PSRAM: N/A", ESP.getFreePsram() / 1024.0, ESP.getPsramSize() ), 5, 35 );

        gDisplay->DrawText( str_sprintf( "SCREEN UPDATE: %dms", frameDuration ), 5, 50 );
        gDisplay->EndFrame();

        frameDuration = millis() - frameStart;
        if( frameDuration < 1000 )
            delay( 1000 - frameDuration );
    }
}

LaserLineEffect laser;
SpectrumAnalyzer analyzer(16);

void IRAM_ATTR UpdateLeds( void *param )
{
    float lineStart  = 0.0;
    float lineLength = .15;

    long time     = millis();
    long lastShot = millis();

    int position_x = 0;
    int position_y = 0;

    float k = 1.0;
    std::vector<double> peaks(16);

    for( ;; )
    {
        long frameStart = millis();

        led_renderer.Clear( CRGB::Black );
        long ts = millis() - time;
        laser.Update( ts / 1000.0f );
        time = millis();

        long timeSinceLastShot = millis() - lastShot;
        if( timeSinceLastShot > 500 )
        {
            laser.Fire();
            lastShot = millis();
        }

        fire.DrawFire();
        smooth_fire.Update(ts / 1000.0f);
        
        // led_renderer.DrawLine( lineStart, lineLength, CRGB::Red );
        // led_renderer.SetPixel( lineStart, CRGB::Red );
        // lineLength += 0.01;
        // lineStart += 0.01;
        // if( lineStart + lineLength >= 1.0 )
        // {
        //     lineStart += 0.01;
        //     lineLength -= 0.01;
        // }

        // if( lineStart >= 1.0 )
        // {
        //     lineStart  = 0.0;
        //     lineLength = .15;
        // }

        // if( ( position_x == 0 ) && ( position_y == 0 ) )
        //     led_matrix.Clear();
        // led_matrix.SetPixel( position_y, position_x, CRGB::Red );
        // position_x = ( position_x + 1 ) % led_matrix.Width;
        // if( position_x == 0 )
        //     position_y = ( position_y + 1 ) % led_matrix.Height;
        // led_matrix.Present();

        // gfx.DrawRect( 0, 0, 48, 16, CRGB::Gray );
        // uint16_t start    = 2;
        // CRGB     colors[] = { CRGB::IndianRed, CRGB::GreenYellow, CRGB::BlueViolet };
        // for( int i = 0; i < 23; i++ )
        // {
        //     gfx.DrawHLine(2 * i + 1, 4, 2, CRGB::Yellow);
        //     gfx.DrawFilledRect( 2 * i + 1, 5, 2, 5, colors[i % 3] );
        // }


        gfx.Clear();
        for (int i=0; i < peaks.size(); i++)
            peaks[i] = sin(3 * (i + k) * (2 * PI / 48.0)) * 2048 + 2048;
        k += 1;
        // // std::fill(peaks.begin(), peaks.end(), 4096);
        analyzer.SetPeaks(peaks, ts / 1000.0f);
        analyzer.Render(gfx);
        // led_matrix.Clear();
        led_matrix.SetPixels( gfx.GetPixels() );
        led_matrix.Present();

        led_renderer.Clear();
        // laser.Render( led_renderer );
        smooth_fire.Render(led_renderer);
        led_strip.Clear();
        // led_strip.SetPixel(0, CRGB::Red);
        led_strip.Blit( led_renderer );
        led_strip.Present();

        long frameDuration = millis() - frameStart;

        if( frameDuration < 30 )
            delay( 30 - frameDuration );

        
    }
}

void setup()
{
    Serial.begin( 9600 );
    Serial.println( "==========================================" );
    Serial.println( "|                TEST                    |" );
    Serial.println( "==========================================" );

    GraphicsConfiguration config{};
    config.Width    = (uint32_t)TFT_WIDTH;
    config.Height   = (uint32_t)TFT_HEIGHT;
    config.Rotation = (uint8_t)1;
    gDisplay        = std::make_unique<Graphics>( config );

    pinMode( BUTTON_1_PIN, INPUT_PULLUP );
    pinMode( BUTTON_2_PIN, INPUT_PULLUP );
    TM.Start();
    TM.StartThread( UpdateScreen, "UpdateScreen", DEFAULT_STACK_SIZE, SCREEN_PRIORITY, &_screenTaskHandle, SCREEN_CORE );
    TM.StartThread( UpdateLeds, "UpdateLeds", DEFAULT_STACK_SIZE, LED_PRIORITY, &_ledTaskHandle, LED_CORE );
}

void drawStar( int x, int y, int star_color );
void numberBox( int x, int y, float num );

// void loop()
// {
//     static int position_x = 0;
//     static int position_y = 0;

// }

void loop()
{
    delay( 10000 );
    // static int position_x = 0;
    // static int position_y = 0;
    // constexpr int step = 5;

    // tft.fillScreen( TFT_NAVY );

    // // Draw 10 sprites containing a "transparent" colour
    // for( int i = 0; i < 10; i++ )
    // {
    //     int x = random( 240 - 70 );
    //     int y = random( 320 - 80 );
    //     int c = random( 0x10000 ); // Random colour
    //     drawStar( x, y, c );
    // }

    // delay( 2000 );

    // uint32_t dt = millis();

    // // Now go bananas and draw 500 more
    // for( int i = 0; i < 500; i++ )
    // {
    //     int x = random( 240 - 70 );
    //     int y = random( 320 - 80 );
    //     int c = random( 0x10000 ); // Random colour
    //     drawStar( x, y, c );
    //     yield(); // Stop watchdog reset
    // }

    // Show time in milliseconds to draw and then push 1 sprite to TFT screen
    // numberBox( 10, 10, ( millis() - dt ) / 500.0 );

    // delay( 2000 );
}

// // #########################################################################
// // Create sprite, plot graphics in it, plot to screen, then delete sprite
// // #########################################################################
// void drawStar( int x, int y, int star_color )
// {
//     // Create an 8-bit sprite 70x 80 pixels (uses 5600 bytes of RAM)
//     img.setColorDepth( 8 );
//     img.createSprite( 70, 80 );

//     // Fill Sprite with a "transparent" colour
//     // TFT_TRANSPARENT is already defined for convenience
//     // We could also fill with any colour as "transparent" and later specify that
//     // same colour when we push the Sprite onto the screen.
//     img.fillSprite( TFT_TRANSPARENT );

//     // Draw 2 triangles to create a filled in star
//     img.fillTriangle( 35, 0, 0, 59, 69, 59, star_color );
//     img.fillTriangle( 35, 79, 0, 20, 69, 20, star_color );

//     // Punch a star shaped hole in the middle with a smaller transparent star
//     img.fillTriangle( 35, 7, 6, 56, 63, 56, TFT_TRANSPARENT );
//     img.fillTriangle( 35, 73, 6, 24, 63, 24, TFT_TRANSPARENT );

//     // Push sprite to TFT screen CGRAM at coordinate x,y (top left corner)
//     // Specify what colour is to be treated as transparent.
//     img.pushSprite( x, y, TFT_TRANSPARENT );

//     // Delete it to free memory
//     img.deleteSprite();
// }

// // #########################################################################
// // Draw a number in a rounded rectangle with some transparent pixels
// // #########################################################################
// void numberBox( int x, int y, float num )
// {

// // Size of sprite
// #define IWIDTH  80
// #define IHEIGHT 35

//     // Create a 8-bit sprite 80 pixels wide, 35 high (2800 bytes of RAM needed)
//     img.setColorDepth( 8 );
//     img.createSprite( IWIDTH, IHEIGHT );

//     // Fill it with black (this will be the transparent colour this time)
//     img.fillSprite( TFT_BLACK );

//     // Draw a background for the numbers
//     img.fillRoundRect( 0, 0, 80, 35, 15, TFT_RED );
//     img.drawRoundRect( 0, 0, 80, 35, 15, TFT_WHITE );

//     // Set the font parameters
//     img.setTextSize( 1 );          // Font size scaling is x1
//     img.setTextColor( TFT_WHITE ); // White text, no background colour

//     // Set text coordinate datum to middle right
//     img.setTextDatum( MR_DATUM );

//     // Draw the number to 3 decimal places at 70,20 in font 4
//     img.drawFloat( num, 3, 70, 20, 4 );

//     // Push sprite to TFT screen CGRAM at coordinate x,y (top left corner)
//     // All black pixels will not be drawn hence will show as "transparent"
//     img.pushSprite( x, y, TFT_BLACK );

//     // Delete sprite to free up the RAM
//     img.deleteSprite();
// }