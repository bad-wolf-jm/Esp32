#pragma once

// #include "LedStripBase.h"
#include <FastLED.h>
#include <vector>

class LedStripRenderer
{
  public:
    LedStripRenderer( float length, int dpi )
        : _length{ length }
        , _dpi{ dpi }
        , _ledCount{ (uint32_t)( _length * _dpi ) }
        , _ledArray{ std::vector<CRGB>( _ledCount ) }
    {
    }

    inline void Clear()
    {
        Clear( CRGB::Black );
    }

    inline void Clear( CRGB color )
    {
        std::fill( _ledArray.begin(), _ledArray.end(), color );
    }

    inline std::vector<CRGB> const &GetPixels() const
    {
        return _ledArray;
    }

    inline void DrawPixel( int x, CRGB value )
    {
        if( x < 0 )
            return;

        if( x >= _ledCount )
            return;

        _ledArray[x] += value;
    }

    inline void DrawPixel( int x, CRGB value, float fraction )
    {
        DrawPixel(x, ColorFraction(value, fraction));
    }

    inline void SetPixel( float x, CRGB color )
    {
        if( x < 0 )
            return;

        int i = ( x * _dpi );

        if( i >= _ledCount )
            return;

        float intensity0 = 1.0f - ( x - (int)x );
        float intensity1 = x - (int)x;

        _ledArray[i] += ColorFraction( color, intensity0 );
        _ledArray[i + 1] += ColorFraction( color, intensity1 );
    }

    inline CRGB ColorFraction( CRGB in, float fraction )
    {
        fraction = std::min( 1.0f, fraction );
        return CRGB( in ).fadeToBlackBy( 255 * ( 1.0f - fraction ) );
    }

    inline int GetPixelPosition( float pos )
    {
        return static_cast<int>( pos * _dpi );
    }

    inline int GetLengthInPixels( float pos )
    {
        return static_cast<int>( pos * _dpi );
    }

    inline float GetPixelFraction( float pos )
    {
        return ( pos - static_cast<int>( pos ) );
    }

    inline void DrawLine( float startPosition, float length, CRGB color )
    {
        int positionInStrip = ( startPosition * _dpi );

        float remaining = std::min( length * _dpi, _ledCount - startPosition * _dpi );

        // First led
        if( remaining > 0.0 )
        {
            float intensity = 1.0f - ( startPosition - (int)startPosition );
            _ledArray[positionInStrip++] += ColorFraction( color, intensity );
            remaining -= intensity;
        }

        while( remaining > 1.0 )
        {
            _ledArray[positionInStrip++] += color;
            remaining -= 1.0;
        }

        if( remaining > 0.0 )
            _ledArray[positionInStrip] += ColorFraction( color, remaining );
    }

  private:
    float             _length;
    uint32_t          _dpi;
    uint32_t          _ledCount = 0;
    std::vector<CRGB> _ledArray;
};