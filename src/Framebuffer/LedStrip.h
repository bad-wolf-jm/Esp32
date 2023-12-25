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

    inline void SetPixels( std::vector<CRGB> const &pixelValues )
    {
        for( int i = 0; i < _ledCount; i++ )
            _ledArray[GetIndex( i )] = pixelValues[i];
    }

    inline void Blit( LedStripRenderer const &renderer )
    {
        auto const &renderedPixels = renderer.GetPixels();
        for( int i = 0; i < _ledCount; i++ )
            _ledArray[GetIndex( i )] = renderedPixels[i];
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