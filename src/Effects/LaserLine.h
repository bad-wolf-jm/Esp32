#pragma once
#include "EffectBase.h"

struct LaserShot
{
    float Position;
    float Length;
    float Speed;
    CRGB  Color;

    void Update( float ts )
    {
        Position += Speed * ts;
    }

    void Render( LedStripRenderer &renderer )
    {
        int   iStart = renderer.GetPixelPosition( Position );
        float frac   = renderer.GetPixelFraction( Position );
        if( frac > 0.0f )
            iStart++;

        int pixelLength = renderer.GetLengthInPixels( Length );
        int iEnd        = std::max( 0, iStart - pixelLength );

        CRGB color = Color;
        renderer.DrawPixel( iStart, Color, frac );
        for( int i = iStart; i > iEnd; i-- )
        {
            renderer.DrawPixel( i, color );
            color.fadeToBlackBy( 15 );
        }
    }
};

class LaserLineEffect : public LedStripEffectBase
{
  public:
    virtual void Update( float ts )
    {
        auto it = _shots.begin();
        while( it != _shots.end() )
        {
            ( *it ).Update( ts );

            if( ( *it ).Position > ( *it ).Length + 1.0 )
                _shots.erase( it );
            else
                it++;
        }
    }

    virtual void Render( LedStripRenderer &renderer )
    {
        for( auto &shot : _shots )
            shot.Render( renderer );
    }

    void Fire()
    {
        static CRGB colors[] = { CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Blue };
        float       value    = random16() / static_cast<float>( UINT16_MAX );
        float       speed    = random16() / static_cast<float>( UINT16_MAX );
        speed                = speed + 1.0f;
        _shots.emplace_back( LaserShot{ 0.0f, value * 0.5f, speed, colors[random8( 4 )] } );
    }

  private:
    std::vector<LaserShot> _shots;
};