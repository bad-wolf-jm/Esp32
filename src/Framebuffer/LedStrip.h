#pragma once

#include "LedStripBase.h"

template <uint8_t DATA_PIN, EOrder RGB_ORDER>
class LedStrip : public LedStripBase<DATA_PIN, RGB_ORDER>
{
    using LedStripBase<DATA_PIN, RGB_ORDER>::_ledArray;
    using LedStripBase<DATA_PIN, RGB_ORDER>::_ledCount;

  public:
    LedStrip( uint32_t length, bool isReversed )
        : LedStripBase<DATA_PIN, RGB_ORDER>( length )
        , _isReversed{ isReversed }
    {
    }

    inline void SetPixel( uint32_t i, CRGB value )
    {
        if( _isReversed )
            _ledArray[_ledCount - i - 1] = value;
        else
            _ledArray[i] = value;
    }

    inline void SetPixels( std::vector<CRGB> const &pixelValues )
    {
        if( _isReversed )
        {
            for( int i = 0; i < _ledCount; i++ )
                _ledArray[_ledCount - i - 1] = pixelValues[i];
        }
        else
        {
            for( int i = 0; i < _ledCount; i++ )
                _ledArray[i] = pixelValues[i];
        }
    }

  protected:
    uint32_t _isReversed = 0;
};