#pragma once
#include "FastLED.h"
#include "LedStripRenderer.h"
#include <random>

#include "Core/Vector.h"
// #include <vector>
// #include "globals.h"
// #include "musiceffect.h"
// #include "soundanalyzer.h"
// #include "systemcontainer.h"

class FireEffect // : public LEDStripEffect
{
    // void construct()
    // {
    //     heat.reset( psram_allocator<uint8_t>().allocate(CellCount()) );
    // }

  protected:
    int  LEDCount; // Number of LEDs total
    int  CellsPerLED;
    int  Cooling;     // Rate at which the pixels cool off
    int  Sparks;      // How many sparks will be attempted each frame
    int  SparkHeight; // If created, max height for a spark
    int  Sparking;    // Probability of a spark each attempt
    bool bReversed;   // If reversed we draw from 0 outwards
    bool bMirrored;   // If mirrored we split and duplicate the drawing

    vector_t<uint8_t> heat;

    // When diffusing the fire upwards, these control how much to blend in from the cells below (ie: downward neighbors)
    // You can tune these coefficients to control how quickly and smoothly the fire spreads

    static const uint8_t BlendSelf      = 2; // 2
    static const uint8_t BlendNeighbor1 = 3; // 3
    static const uint8_t BlendNeighbor2 = 2; // 2
    static const uint8_t BlendNeighbor3 = 1; // 1

    static const uint8_t BlendTotal = ( BlendSelf + BlendNeighbor1 + BlendNeighbor2 + BlendNeighbor3 );

    // static constexpr int _jsonSize = LEDStripEffect::_jsonSize + 128;

    int CellCount() const
    {
        return LEDCount * CellsPerLED;
    }

  public:
    FireEffect( const String &strName, int ledCount = 1, int cellsPerLED = 1, int cooling = 30, int sparking = 100, int sparks = 3,
                int sparkHeight = 4, bool breversed = false, bool bmirrored = false )
        // : LEDStripEffect( EFFECT_STRIP_FIRE, strName )
        : LEDCount( ledCount )
        , CellsPerLED( cellsPerLED )
        , Cooling( cooling )
        , Sparks( sparks )
        , SparkHeight( sparkHeight )
        , Sparking( sparking )
        , bReversed( breversed )
        , bMirrored( bmirrored )
    {
        if( bMirrored )
            LEDCount = LEDCount / 2;

        heat = vector_t<uint8_t>( CellCount() );
        // construct();
    }

    // FireEffect( const JsonObjectConst &jsonObject )
    //     // : LEDStripEffect( jsonObject )
    //     , LEDCount( jsonObject[PTY_LEDCOUNT] )
    //     , CellsPerLED( jsonObject[PTY_CELLSPERLED] )
    //     , Cooling( jsonObject[PTY_COOLING] )
    //     , Sparks( jsonObject[PTY_SPARKS] )
    //     , SparkHeight( jsonObject[PTY_SPARKHEIGHT] )
    //     , Sparking( jsonObject[PTY_SPARKING] )
    //     , bReversed( jsonObject[PTY_REVERSED] )
    //     , bMirrored( jsonObject[PTY_MIRORRED] )
    // {
    //     construct();
    // }

    // bool SerializeToJSON( JsonObject &jsonObject ) override
    // {
    //     StaticJsonDocument<_jsonSize> jsonDoc;

    //     JsonObject root = jsonDoc.to<JsonObject>();
    //     LEDStripEffect::SerializeToJSON( root );

    //     jsonDoc[PTY_LEDCOUNT]    = LEDCount;
    //     jsonDoc[PTY_CELLSPERLED] = CellsPerLED;
    //     jsonDoc[PTY_COOLING]     = Cooling;
    //     jsonDoc[PTY_SPARKS]      = Sparks;
    //     jsonDoc[PTY_SPARKHEIGHT] = SparkHeight;
    //     jsonDoc[PTY_SPARKING]    = Sparking;
    //     jsonDoc[PTY_REVERSED]    = bReversed;
    //     jsonDoc[PTY_MIRORRED]    = bMirrored;

    //     assert( !jsonDoc.overflowed() );

    //     return jsonObject.set( jsonDoc.as<JsonObjectConst>() );
    // }

    virtual ~FireEffect()
    {
    }

    // size_t DesiredFramesPerSecond() const override
    // {
    //     return 45;
    // }

    // void Draw() override
    // {
    //     FastLED.clear( false );
    //     DrawFire();
    // }

    virtual void GenerateSparks( float multiplier = 1.0 )
    {
        for( int i = 0; i < Sparks * multiplier; i++ )
        {
            if( random( 255 ) < Sparking )
            {
                int y = random( SparkHeight * CellsPerLED );
                heat[y] += random( 200, 255 ); // Can roll over which actually looks good!
            }
        }
    }

    virtual void DrawFire()
    {
        // First cool each cell by a little bit

        // EVERY_N_MILLISECONDS( 50 )
        // {
        for( int i = 0; i < CellCount(); i++ )
        {
            int coolingAmount = random( 0, Cooling );
            heat[i]           = ::max( 0, heat[i] - coolingAmount );
        }
        // }

        // EVERY_N_MILLISECONDS( 20 )
        // {
        // Next drift heat up and diffuse it a little bit
        for( int i = 3; i < CellCount(); i++ )
            heat[i] = std::min( 255, ( heat[i] * BlendSelf + heat[( i - 1 )] * BlendNeighbor1 + heat[( i - 2 )] * BlendNeighbor2 +
                                       heat[( i - 3 )] * BlendNeighbor3 ) /
                                         BlendTotal );
        // }

        // Randomly ignite new sparks down in the flame kernel

        // EVERY_N_MILLISECONDS( 20 )
        // {
        GenerateSparks( 5.0 );
        // }
    }

    virtual void Render( LedStripRenderer &renderer )
    {
        // Finally, convert heat to a color
        for( int i = 0; i < LEDCount; i++ )
        {
            auto sum = 0;
            for( int j = 0; j < CellsPerLED; j++ )
                sum += heat[i * CellsPerLED + j];
            auto avg = sum / CellsPerLED;

            // #if LANTERN
            // CRGB color = CRGB( avg, avg * .45, avg * .08 );
            // #else
            CRGB color = GetBlackBodyHeatColor( avg / (float)std::numeric_limits<uint8_t>::max() );
            // #endif

            // If we're reversed, we work from the end back.  We don't reverse the bonus pixels

            // int j = ( !bReversed ) ? i : LEDCount - 1 - i;
            renderer.DrawPixel( i, color );
            // setPixelsOnAllChannels( j, 1, color, false );
            // if( bMirrored )
            //     setPixelsOnAllChannels( !bReversed ? ( 2 * LEDCount - 1 - i ) : LEDCount + i, 1, color, false );
        }
    }

    virtual CRGB GetBlackBodyHeatColor( float temp ) const
    {
        return ColorFromPalette( HeatColors_p, 255 * temp );
    }

    // The variant allows you to specify a base flame color other than red, and the result
    // is interpolated from black to your color and on through yellow and white

    // virtual CRGB GetBlackBodyHeatColor( float temp, CRGB baseColor ) const
    // {
    //     if( baseColor == CRGB::Red )
    //         return GetBlackBodyHeatColor( temp );

    //     temp = std::clamp( temp, 0.0f, 1.0f );

    //     if( temp < 0.33f )
    //         return LedStripRenderer::ColorFraction( baseColor, temp * 3.0f ); // Interpolate from black to baseColor

    //     if( temp < 0.66f )
    //         return baseColor +
    //                ColorFraction( CRGB::Yellow - baseColor, ( temp - 0.33f ) * 3.0f ); // Interoplate from baseColor to Yellow

    //     return CRGB::Yellow + ColorFraction( CRGB::Blue, ( temp - 0.66f ) * 3.0f ); // Interpolate from Yellow to White
    // }
};

// class PaletteFlameEffect : public FireEffect
// {
//     CRGBPalette16 _palette;
//     bool          _ignoreGlobalColor;

//     void construct()
//     {
//         _effectNumber = EFFECT_STRIP_PALETTE_FLAME;
//     }

//   public:
//     PaletteFlameEffect( const String &strName, const CRGBPalette16 &palette, bool ignoreGlobalColor = false, int ledCount =
//     NUM_LEDS,
//                         int cellsPerLED = 1,
//                         int cooling     = 20, // Was 1.8 for NightDriverStrip
//                         int sparking = 100, int sparks = 3, int sparkHeight = 3, bool reversed = false, bool mirrored = false )
//         : FireEffect( strName, ledCount, cellsPerLED, cooling, sparking, sparks, sparkHeight, reversed, mirrored )
//         , _palette( palette )
//         , _ignoreGlobalColor( ignoreGlobalColor )
//     {
//         construct();
//     }

//     PaletteFlameEffect( const JsonObjectConst &jsonObject )
//         : FireEffect( jsonObject )
//         , _palette( jsonObject[PTY_PALETTE].as<CRGBPalette16>() )
//         , _ignoreGlobalColor( jsonObject[PTY_IGNOREGLOBALCOLOR] )
//     {
//         construct();
//     }

//     bool SerializeToJSON( JsonObject &jsonObject ) override
//     {
//         AllocatedJsonDocument jsonDoc( FireEffect::_jsonSize + 512 );

//         JsonObject root = jsonDoc.to<JsonObject>();
//         FireEffect::SerializeToJSON( root );

//         jsonDoc[PTY_PALETTE]           = _palette;
//         jsonDoc[PTY_IGNOREGLOBALCOLOR] = _ignoreGlobalColor;

//         assert( !jsonDoc.overflowed() );

//         return jsonObject.set( jsonDoc.as<JsonObjectConst>() );
//     }

//     virtual CRGB GetBlackBodyHeatColor( float temp ) const override
//     {
//         temp               = min( 1.0f, temp );
//         int   index        = fmap( temp, 0.0f, 1.0f, 0.0f, 240.0f );
//         auto &deviceConfig = g_ptrSystem->DeviceConfig();
//         if( deviceConfig.ApplyGlobalColors() && !_ignoreGlobalColor )
//         {
//             auto tempPalette = CRGBPalette16( CRGB::Black, deviceConfig.GlobalColor(), CRGB::Yellow, CRGB::White );
//             return ColorFromPalette( tempPalette, index, 255 );
//         }
//         else
//             return ColorFromPalette( _palette, index, 255 );

//         //        uint8_t heatramp = (uint8_t)(t192 & 0x3F);
//         //        heatramp <<=2;
//     }
// };

// class ClassicFireEffect : public LEDStripEffect
// {
//     bool _Mirrored;
//     bool _Reversed;
//     int  _Cooling;

//   public:
//     ClassicFireEffect( bool mirrored = false, bool reversed = false, int cooling = 5 )
//         : LEDStripEffect( EFFECT_STRIP_CLASSIC_FIRE, "Classic Fire" )
//         , _Mirrored( mirrored )
//         , _Reversed( reversed )
//         , _Cooling( cooling )
//     {
//     }

//     ClassicFireEffect( const JsonObjectConst &jsonObject )
//         : LEDStripEffect( jsonObject )
//         , _Mirrored( jsonObject[PTY_MIRORRED] )
//         , _Reversed( jsonObject[PTY_REVERSED] )
//         , _Cooling( jsonObject[PTY_COOLING] )
//     {
//     }

//     bool SerializeToJSON( JsonObject &jsonObject ) override
//     {
//         StaticJsonDocument<LEDStripEffect::_jsonSize + 64> jsonDoc;

//         JsonObject root = jsonDoc.to<JsonObject>();
//         LEDStripEffect::SerializeToJSON( root );

//         jsonDoc[PTY_MIRORRED] = _Mirrored;
//         jsonDoc[PTY_REVERSED] = _Reversed;
//         jsonDoc[PTY_COOLING]  = _Cooling;

//         assert( !jsonDoc.overflowed() );

//         return jsonObject.set( jsonDoc.as<JsonObjectConst>() );
//     }

//     void Draw() override
//     {
//         Fire( _Cooling, 180, 5 );
//         delay( 20 );
//     }

//     void Fire( int Cooling, int Sparking, int Sparks )
//     {
//         static std::unique_ptr<uint8_t[]> heat = make_unique_psram_array<uint8_t>( NUM_LEDS );
//         setAllOnAllChannels( 0, 0, 0 );

//         // Step 1.  Cool down every cell a little
//         for( int i = 0; i < _cLEDs; i++ )
//         {
//             int cooldown = random_range( 0, Cooling );
//             if( cooldown > heat[i] )
//             {
//                 heat[i] = 0;
//             }
//             else
//             {
//                 heat[i] = heat[i] - cooldown;
//             }
//         }

//         // Step 2.  Heat from each cell drifts 'up' and diffuses a little
//         for( int k = _cLEDs - 1; k >= 3; k-- )
//         {
//             heat[k] = ( heat[k - 1] + heat[k - 2] + heat[k - 3] ) / 3;
//         }

//         // Step 3.  Randomly ignite new 'sparks' near the bottom
//         for( int frame = 0; frame < Sparks; frame++ )
//         {
//             if( random( 255 ) < Sparking )
//             {
//                 int y = random( 5 );
//                 heat[y] =
//                     heat[y] + random( 160, 255 ); // This randomly rolls over sometimes of course, and that's essential to the
//                     effect
//             }
//         }

//         // Step 4.  Convert heat to LED colors
//         for( int j = 0; j < _cLEDs; j++ )
//         {
//             setPixelHeatColor( j, heat[j] );
//         }
//     }

//     void setPixelWithMirror( int Pixel, CRGB temperature )
//     {
//         // Serial.printf("Setting pixel %d to %d, %d, %d\n", Pixel, temperature.r, temperature.g, temperature.b);

//         if( _Mirrored )
//         {
//             int middle = _cLEDs / 2;
//             setPixelOnAllChannels( middle - Pixel, temperature );
//             setPixelOnAllChannels( middle + Pixel, temperature );
//         }
//         else
//         {
//             if( _Reversed )
//                 setPixelOnAllChannels( _cLEDs - 1 - Pixel, temperature );
//             else
//                 setPixelOnAllChannels( Pixel, temperature );
//         }
//     }

//     void setPixelHeatColor( int Pixel, uint8_t temperature )
//     {
//         // Scale 'heat' down from 0-255 to 0-191
//         uint8_t t192 = round( ( temperature / 255.0 ) * 191 );

//         // calculate ramp up from
//         uint8_t heatramp = t192 & 0x3F; // 0..63
//         heatramp <<= 2;                 // scale up to 0..252

//         // figure out which third of the spectrum we're in:
//         if( t192 > 0x80 )
//         { // hottest
//             setPixelWithMirror( Pixel, CRGB( 255, 255, heatramp ) );
//         }
//         else if( t192 > 0x40 )
//         { // middle
//             setPixelWithMirror( Pixel, CRGB( 255, heatramp, 0 ) );
//         }
//         else
//         { // coolest
//             setPixelWithMirror( Pixel, CRGB( heatramp, 0, 0 ) );
//         }
//     }
// };

class SmoothFireEffect
{
  private:
    bool  _Reversed;
    float _Cooling;
    int   _Sparks;
    float _Drift;
    int   _DriftPasses;
    int   _SparkHeight;
    bool  _Turbo;
    bool  _Mirrored;
    int   _cLEDs;

    vector_t<float> _Temperatures;

  public:
    // Parameter:   Cooling   Sparks    driftPasses  drift sparkHeight   Turbo
    // Calm Fire:     0.75f        2         1         64       8          F
    // Full Red:      0.75f        8         1        128      16          F
    // Good Video:    1.20f       64         1        128      12          F

    SmoothFireEffect( int ledCount = 1, bool reversed = true, float cooling = 0.75f, int sparks = 16, int driftPasses = 1,
                      float drift = 64, int sparkHeight = 12, bool turbo = false, bool mirrored = false )

        // : LEDStripEffect( EFFECT_STRIP_SMOOTH_FIRE, "Fire Sound Effect v2" )
        : _Reversed( reversed )
        , _Cooling( cooling )
        , _Sparks( sparks )
        , _Drift( drift )
        , _DriftPasses( driftPasses )
        , _SparkHeight( sparkHeight )
        , _Turbo( turbo )
        , _Mirrored( mirrored )
        , _cLEDs{ ledCount }
    {
        _Temperatures = vector_t<float>( _cLEDs );
    }

    // SmoothFireEffect( const JsonObjectConst &jsonObject )
    //     : LEDStripEffect( jsonObject )
    //     , _Reversed( jsonObject[PTY_REVERSED] )
    //     , _Cooling( jsonObject[PTY_COOLING] )
    //     , _Sparks( jsonObject[PTY_SPARKS] )
    //     , _Drift( jsonObject["dft"] )
    //     , _DriftPasses( jsonObject["dtp"] )
    //     , _SparkHeight( jsonObject[PTY_SPARKHEIGHT] )
    //     , _Turbo( jsonObject["trb"] )
    //     , _Mirrored( jsonObject[PTY_MIRORRED] )
    // {
    // }

    // bool SerializeToJSON( JsonObject &jsonObject ) override
    // {
    //     StaticJsonDocument<LEDStripEffect::_jsonSize> jsonDoc;

    //     JsonObject root = jsonDoc.to<JsonObject>();
    //     LEDStripEffect::SerializeToJSON( root );

    //     jsonDoc[PTY_MIRORRED]    = _Reversed;
    //     jsonDoc[PTY_COOLING]     = _Cooling;
    //     jsonDoc[PTY_SPARKS]      = _Sparks;
    //     jsonDoc["dft"]           = _Drift;
    //     jsonDoc["dtp"]           = _DriftPasses;
    //     jsonDoc[PTY_SPARKHEIGHT] = _SparkHeight;
    //     jsonDoc["trb"]           = _Turbo;
    //     jsonDoc[PTY_MIRORRED]    = _Mirrored;

    //     assert( !jsonDoc.overflowed() );

    //     return jsonObject.set( jsonDoc.as<JsonObjectConst>() );
    // }

    // bool Init( vector_t<std::shared_ptr<GFXBase>> &gfx ) override
    // {
    //     // LEDStripEffect::Init( gfx );
    //     _Temperatures = (float *)PreferPSRAMAlloc( sizeof( float ) * _cLEDs );
    //     if( !_Temperatures )
    //     {
    //         Serial.println( "ERROR: Could not allocate memory for FireEffect" );
    //         return false;
    //     }
    //     return true;
    // }

    ~SmoothFireEffect()
    {
        // free( _Temperatures );
    }

    template <typename T>
    inline static T random_range( T lower, T upper )
    {
        static_assert( std::is_arithmetic<T>::value, "Template argument must be numeric type" );

        static std::random_device rd;
        static std::mt19937       gen( rd() );

        if constexpr( std::is_integral<T>::value )
        {
            std::uniform_int_distribution<T> distrib( lower, upper );
            return distrib( gen );
        }
        else if constexpr( std::is_floating_point<T>::value )
        {
            std::uniform_real_distribution<T> distrib( lower, upper );
            return distrib( gen );
        }
    }

    void Update( float deltaTime )
    {
        // float deltaTime = (float)g_Values.AppTime.LastFrameTime();
        // setAllOnAllChannels( 0, 0, 0 );

        float cooldown = random_range( 0.0f, _Cooling ) * deltaTime;

        for( int i = 0; i < _cLEDs; i++ )
        {
            if( cooldown > _Temperatures[i] )
                _Temperatures[i] = 0;
            else
                _Temperatures[i] = _Temperatures[i] - cooldown;
        }

        // Heat from each cell drifts 'up' and diffuses a little
        for( int pass = 0; pass < _DriftPasses; pass++ )
        {
            for( int k = _cLEDs - 1; k >= 3; k-- )
            {
                float amount = std::min(0.85f, _Drift * deltaTime);//0.2f; //+ g_Analyzer._VURatio; // MIN(0.85f, _Drift * deltaTime);
                float c0     = 1.0f - amount;
                float c1     = amount * 0.33f;
                float c2     = c1;
                float c3     = c1;

                _Temperatures[k] =
                    _Temperatures[k] * c0 + _Temperatures[k - 1] * c1 + _Temperatures[k - 2] * c2 + _Temperatures[k - 3] * c3;
            }
        }

        // Randomly ignite new 'sparks' near the bottom
        for( int frame = 0; frame < _Sparks; frame++ )
        {
            if( random_range( 0.0f, 1.0f ) < 0.70f )
            {
                // NB: This randomly rolls over sometimes of course, and that's essential to the effect
                int y            = random_range( 0, _SparkHeight );
                _Temperatures[y] = ( _Temperatures[y] + random_range( 0.6f, 1.0f ) );

                // if( !_Turbo )
                while( _Temperatures[y] > 1.0f )
                    _Temperatures[y] -= 1.0f;
                // else
                // _Temperatures[y] = min( _Temperatures[y], 1.0f );
            }
        }
    }

    virtual CRGB GetBlackBodyHeatColor( float temp ) const
    {
        return ColorFromPalette( HeatColors_p, 255 * temp );
    }

    virtual void Render( LedStripRenderer &renderer )
    {

        for( uint j = 0; j < _cLEDs; j++ )
        {
            CRGB c = GetBlackBodyHeatColor( _Temperatures[j] );
            renderer.DrawPixel( j, c );
        }
        // // Finally, convert heat to a color
        // for( int i = 0; i < LEDCount; i++ )
        // {
        //     auto sum = 0;
        //     for( int j = 0; j < CellsPerLED; j++ )
        //         sum += heat[i * CellsPerLED + j];
        //     auto avg = sum / CellsPerLED;

        //     // #if LANTERN
        //     // CRGB color = CRGB( avg, avg * .45, avg * .08 );
        //     // #else
        //     CRGB color = GetBlackBodyHeatColor( avg / (float)std::numeric_limits<uint8_t>::max() );
        //     // #endif

        //     // If we're reversed, we work from the end back.  We don't reverse the bonus pixels

        //     // int j = ( !bReversed ) ? i : LEDCount - 1 - i;
        //     renderer.DrawPixel( i, color );
        //     // setPixelsOnAllChannels( j, 1, color, false );
        //     // if( bMirrored )
        //     //     setPixelsOnAllChannels( !bReversed ? ( 2 * LEDCount - 1 - i ) : LEDCount + i, 1, color, false );
        // }
    }

    // void setPixelWithMirror( int Pixel, CRGB temperature )
    // {
    //     if( _Reversed || _Mirrored )
    //         setPixelOnAllChannels( Pixel, temperature );

    //     if( !_Reversed || _Mirrored )
    //         setPixelOnAllChannels( _cLEDs - 1 - Pixel, temperature );
    // }
};

// class BaseFireEffect : public LEDStripEffect
// {
//     void construct()
//     {
//         heat = std::make_unique<uint8_t[]>( CellCount );
//     }

//   protected:
//     int  Cooling;     // Rate at which the pixels cool off
//     int  Sparks;      // How many sparks will be attempted each frame
//     int  SparkHeight; // If created, max height for a spark
//     int  Sparking;    // Probability of a spark each attempt
//     bool bReversed;   // If reversed we draw from 0 outwards
//     bool bMirrored;   // If mirrored we split and duplicate the drawing

//     int LEDCount;  // Number of LEDs total
//     int CellCount; // How many heat cells to represent entire flame

//     std::unique_ptr<uint8_t[]> heat;

//     // When diffusing the fire upwards, these control how much to blend in from the cells below (ie: downward neighbors)
//     // You can tune these coefficients to control how quickly and smoothly the fire spreads

//     static const uint8_t BlendSelf      = 0; // 2
//     static const uint8_t BlendNeighbor1 = 1; // 3
//     static const uint8_t BlendNeighbor2 = 2; // 2
//     static const uint8_t BlendNeighbor3 = 0; // 1

//     static const uint8_t BlendTotal = ( BlendSelf + BlendNeighbor1 + BlendNeighbor2 + BlendNeighbor3 );

//   public:
//     BaseFireEffect( int ledCount, int cellsPerLED = 1, int cooling = 20, int sparking = 100, int sparks = 3, int sparkHeight = 4,
//                     bool breversed = false, bool bmirrored = false )
//         : LEDStripEffect( EFFECT_STRIP_BASE_FIRE, "BaseFireEffect" )
//         , Cooling( cooling )
//         , Sparks( sparks )
//         , SparkHeight( sparkHeight )
//         , Sparking( sparking )
//         , bReversed( breversed )
//         , bMirrored( bmirrored )
//     {
//         LEDCount  = bMirrored ? ledCount / 2 : ledCount;
//         CellCount = LEDCount * cellsPerLED;

//         construct();
//     }

//     BaseFireEffect( const JsonObjectConst &jsonObject )
//         : LEDStripEffect( jsonObject )
//         , Cooling( jsonObject[PTY_COOLING] )
//         , Sparks( jsonObject[PTY_SPARKS] )
//         , SparkHeight( jsonObject[PTY_SPARKHEIGHT] )
//         , Sparking( jsonObject[PTY_SPARKING] )
//         , bReversed( jsonObject[PTY_REVERSED] )
//         , bMirrored( jsonObject[PTY_MIRORRED] )
//         , LEDCount( jsonObject[PTY_LEDCOUNT] )
//         , CellCount( jsonObject["clc"] )
//     {
//         construct();
//     }

//     virtual ~BaseFireEffect()
//     {
//     }

//     bool SerializeToJSON( JsonObject &jsonObject ) override
//     {
//         StaticJsonDocument<LEDStripEffect::_jsonSize + 128> jsonDoc;

//         JsonObject root = jsonDoc.to<JsonObject>();
//         LEDStripEffect::SerializeToJSON( root );

//         jsonDoc[PTY_COOLING]     = Cooling;
//         jsonDoc[PTY_SPARKS]      = Sparks;
//         jsonDoc[PTY_SPARKHEIGHT] = SparkHeight;
//         jsonDoc[PTY_SPARKING]    = Sparking;
//         jsonDoc[PTY_REVERSED]    = bReversed;
//         jsonDoc[PTY_MIRORRED]    = bMirrored;
//         jsonDoc[PTY_LEDCOUNT]    = LEDCount;
//         jsonDoc["clc"]           = CellCount;

//         assert( !jsonDoc.overflowed() );

//         return jsonObject.set( jsonDoc.as<JsonObjectConst>() );
//     }

//     virtual CRGB MapHeatToColor( uint8_t temperature )
//     {
//         uint8_t t192 = round( ( temperature / 255.0 ) * 191 );

//         // calculate ramp up from
//         uint8_t heatramp = t192 & 0x3F; // 0..63
//         heatramp <<= 2;                 // scale up to 0..252

//         // figure out which third of the spectrum we're in:
//         if( t192 > 0x80 )
//         { // hottest
//             return CRGB( 255, 255, heatramp );
//         }
//         else if( t192 > 0x40 )
//         { // middle
//             return CRGB( 255, heatramp, 0 );
//         }
//         else
//         { // coolest
//             return CRGB( heatramp, 0, 0 );
//         }
//     }

//     void Draw() override
//     {
//         FastLED.showColor( CRGB::Red );
//         return;
//         FastLED.clear( false );
//         DrawFire();
//         delay( 120 );
//     }

//     virtual void DrawFire()
//     {
//         // First cool each cell by a little bit
//         for( int i = 0; i < CellCount; i++ )
//             heat[i] = max( 0L, heat[i] - random( 0, ( ( Cooling * 10 ) / CellCount ) + 2 ) );

//         // Next drift heat up and diffuse it a little bit
//         for( int i = 0; i < CellCount; i++ )
//             heat[i] = min( 255, ( heat[i] * BlendSelf + heat[( i + 1 ) % CellCount] * BlendNeighbor1 +
//                                   heat[( i + 2 ) % CellCount] * BlendNeighbor2 + heat[( i + 3 ) % CellCount] * BlendNeighbor3 ) /
//                                     BlendTotal );

//         // Randomly ignite new sparks down in the flame kernel

//         for( int i = 0; i < Sparks; i++ )
//         {
//             if( random( 255 ) < Sparking )
//             {
//                 int y   = CellCount - 1 - random( SparkHeight * CellCount / LEDCount );
//                 heat[y] = random( 200, 255 ); // heat[y] + random(50, 255);       // Can roll over which actually looks good!
//             }
//         }

//         // Finally, convert heat to a color

//         int cellsPerLED = CellCount / LEDCount;
//         for( int i = 0; i < LEDCount; i++ )
//         {
//             int sum = 0;
//             for( int iCell = 0; iCell < cellsPerLED; iCell++ )
//                 sum += heat[i * cellsPerLED + iCell];
//             int  avg   = sum / cellsPerLED;
//             CRGB color = MapHeatToColor( heat[avg] );
//             int  j     = bReversed ? ( LEDCount - 1 - i ) : i;
//             setPixelsOnAllChannels( j, 1, color, true );
//             if( bMirrored )
//                 setPixelsOnAllChannels( !bReversed ? ( 2 * LEDCount - 1 - i ) : LEDCount + i, 1, color, true );
//         }
//     }
// };