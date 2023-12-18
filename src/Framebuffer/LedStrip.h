#pragma once

#include "LedStripBase.h"

template <int LED_COUNT, bool IS_REVERSED, uint8_t DATA_PIN, EOrder RGB_ORDER>
class LedStrip : public LedStripBase<LED_COUNT, DATA_PIN, RGB_ORDER>
{
    using LedStripBase<LED_COUNT, DATA_PIN, RGB_ORDER>::_ledArray;
    using LedStripBase<LED_COUNT, DATA_PIN, RGB_ORDER>::_ledCount;

  public:
    LedStrip()
        : LedStripBase<LED_COUNT, DATA_PIN, RGB_ORDER>()
        , _isReversed{ IS_REVERSED }
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

  protected:
    uint32_t _isReversed = 0;

  private:
    inline int GetIndex( int i )
    {
        if constexpr( IS_REVERSED )
        {
            return LED_COUNT - i - 1;
        }
        else
        {
            return i;
        }
    }
};