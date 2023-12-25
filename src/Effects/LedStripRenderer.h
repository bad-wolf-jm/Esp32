#pragma once

// #include "LedStripBase.h"
#include <FastLED.h>
#include <vector>

class LedStripRenderer
{
  public:
    LedStripRenderer( float length, int dpi )
        : _ledCount{ (uint32_t)( length * dpi ) }
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

    inline void SetPixel( float x, CRGB value )
    {
        _ledArray[(int)x] = value;
    }

    inline CRGB ColorFraction( CRGB in, float fraction )
    {
        fraction = std::min( 1.0f, fraction );
        return CRGB( in ).fadeToBlackBy( 255 * ( 1.0f - fraction ) );
    }

    inline void DrawLine( float startPosition, float length, CRGB color )
    {
        int positionInStrip = startPosition;

        float remaining = std::min( length, _ledCount - startPosition );

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
        {
            _ledArray[positionInStrip] += ColorFraction( color, remaining );
        }
    }

  protected:
    uint32_t _isReversed = 0;

  private:
    uint32_t          _ledCount = 0;
    std::vector<CRGB> _ledArray;
};