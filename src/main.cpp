#include "Modules/Definitions.h"

// #include <Arduino.h>
// #include <TFT_eSPI.h>
// #include <WiFi.h>
// #include <string>
// #include <vector>

#define FASTLED_INTERNAL
#include <FastLED.h>

#include "Display/Graphics.h"
#include "Effects/BouncingBall.h"
#include "Effects/Fire.h"
#include "Effects/Glow2D.h"
#include "Effects/LaserLine.h"
#include "Effects/LedStripRenderer.h"
#include "Effects/SpectrumAnalyzer.h"
#include "Framebuffer/GFXBase.h"
#include "SoundAnalysis.h"
#include "TaskManager.h"

// #include "entt/entt.hpp"

std::unique_ptr<Graphics> gDisplay;
TaskManager               TM;

const uint32_t BUTTON_PRESS_THRESHOLD = 30;

GFXBase          gfx( 16, 16, 3, 1 );
LedStripRenderer led_renderer( 1.0, 144 );

TaskHandle_t _screenTaskHandle;
TaskHandle_t _ledTaskHandle;
TaskHandle_t _sampleSoundTaskHandle;

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

LaserLineEffect    laser;
BouncingBallEffect bouncing_balls;
SpectrumAnalyzer   analyzer( 16 );
Glow2D             glow_effect( 256u * 3 );
FireEffect         fire( "Fire", 144, 3 );
SmoothFireEffect   smooth_fire( 48 );
SmoothFireEffect   smooth_fire_m[16];
LedStripRenderer   led_renderer_m[16];

void IRAM_ATTR UpdateLeds( void *param )
{
    float lineStart  = 0.0;
    float lineLength = .15;

    long time     = millis();
    long lastShot = millis();

    int position_x = 0;
    int position_y = 0;

    float            k = 1.0;
    vector_t<double> peaks( 16 );

    bouncing_balls.Fire();
    bouncing_balls.Fire();
    bouncing_balls.Fire();

    for( ;; )
    {
        long frameStart = millis();

        led_renderer.Clear( CRGB::Black );
        long ts = millis() - time;
        laser.Update( ts / 1000.0f );
        bouncing_balls.Update( ts / 1000.0f );
        time = millis();

        long timeSinceLastShot = millis() - lastShot;
        if( timeSinceLastShot > 500 )
        {
            laser.Fire();
            // bouncing_balls.Fire();
            lastShot = millis();
        }

        fire.DrawFire();
        smooth_fire.Update( ts / 1000.0f );

        for( int i = 0; i < 16; i++ )
            smooth_fire_m[i].Update( ts / 1000.0f );

        gfx.Clear();
        for( int i = 0; i < peaks.size(); i++ )
            peaks[i] = sin( 3 * ( i + k ) * ( 2 * PI / 48.0 ) ) * 2048 + 2048;
        k += 1;

        FastLED.clear();
        led_renderer.Clear();
        bouncing_balls.Render( led_renderer );

        for( int i = 0; i < 16; i++ )
        {
            led_renderer_m[i].Clear();
            smooth_fire_m[i].Render( led_renderer_m[i] );
            gfx.DrawHStrip( led_renderer_m[i], i );
        }

        FastLED.show();

        long frameDuration = millis() - frameStart;

        if( frameDuration < 30 )
            delay( 30 - frameDuration );
    }
}

void IRAM_ATTR SampleSound( void *param )
{
    SoundAnalyzer sound_analyzer( I2S_NUM_0, 44100, I2S_CHANNEL_FMT_ONLY_LEFT );
    for( ;; )
    {

        sound_analyzer.FillBuffer();
        auto     &b       = sound_analyzer.GetBuffer();
        uint16_t *samples = (uint16_t *)b.data();
        // Serial.printf( "Samples: %d, %d, %d, %d, %d, %d, %d, %d\n", samples[0], samples[1], samples[2], samples[3], samples[4],
        // samples[5], samples[6], samples[7] ); delay( 30 );
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

    for( int i = 0; i < 16; i++ )
    {
        smooth_fire_m[i]  = SmoothFireEffect( 48 );
        led_renderer_m[i] = LedStripRenderer( 1.0, 144 );
    }

    pinMode( LED_CHANNEL_4, OUTPUT );
    FastLED.addLeds<WS2812, LED_CHANNEL_4, GRB>( led_renderer.GetPixels().data(), led_renderer.GetPixels().size() );

    pinMode( LED_CHANNEL_5, OUTPUT );
    FastLED.addLeds<WS2812, LED_CHANNEL_5, GRB>( gfx.GetPixels().data(), gfx.GetPixels().size() );

    FastLED.setBrightness( 5 );

    pinMode( BUTTON_1_PIN, INPUT_PULLUP );
    pinMode( BUTTON_2_PIN, INPUT_PULLUP );
    TM.Start();
    TM.StartThread( UpdateScreen, "UpdateScreen", DEFAULT_STACK_SIZE, SCREEN_PRIORITY, &_screenTaskHandle, SCREEN_CORE );
    // TM.StartThread( UpdateLeds, "UpdateLeds", DEFAULT_STACK_SIZE, LED_PRIORITY, &_ledTaskHandle, LED_CORE );
    TM.StartThread( SampleSound, "SampleSound", DEFAULT_STACK_SIZE, LED_PRIORITY, &_sampleSoundTaskHandle, LED_CORE );
}

void loop()
{
    delay( 10000 );
}
