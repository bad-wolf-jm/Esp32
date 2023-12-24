#pragma once

#include "LedStripBase.h"

template <uint8_t DATA_PIN, EOrder RGB_ORDER>
class LedStrip : public LedStripBase<DATA_PIN, RGB_ORDER>
{
    using LedStripBase<DATA_PIN, RGB_ORDER>::_ledArray;
    using LedStripBase<DATA_PIN, RGB_ORDER>::_ledCount;

  public:
    LedStrip( int ledCount, bool isReversed, float brightness = 5.0f / 255.0f )
        : LedStripBase<DATA_PIN, RGB_ORDER>( ledCount, brightness )
        , _isReversed{ isReversed }
    {
    }

    inline void SetPixel( uint32_t i, CRGB value )
    {
        _ledArray[GetIndex( i )] = value;
    }

    inline void SetPixels( std::vector<CRGB> const &pixelValues )
    {
        for( int i = 0; i < _ledCount; i++ )
            _ledArray[GetIndex( i )] = pixelValues[i];
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
            _ledArray[GetIndex( positionInStrip++ )] += ColorFraction( color, intensity );
            remaining -= intensity;
        }

        while( remaining > 1.0 )
        {
            _ledArray[GetIndex( positionInStrip++ )] += color;
            remaining -= 1.0;
        }

        if( remaining > 0.0 )
        {
            _ledArray[GetIndex( positionInStrip )] += ColorFraction( color, remaining );
        }
    }

  protected:
    uint32_t _isReversed = 0;

  private:
    inline int GetIndex( int i )
    {
        if( _isReversed )
        {
            return _ledCount - i - 1;
        }
        else
        {
            return i;
        }
    }
};