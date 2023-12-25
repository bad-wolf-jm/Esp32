#pragma once

#include <FastLED.h>
#include <vector>

template <uint8_t DATA_PIN, EOrder RGB_ORDER>
class LedStripBase
{
  public:
    LedStripBase( int ledCount, float brightness = 5.0f / 255.0f )
        : _ledCount{ ledCount }
        , _ledArray{ std::vector<CRGB>( ledCount ) }

    {
        _driver.addLeds<WS2812, DATA_PIN, RGB_ORDER>( _ledArray.data(), _ledCount );
        std::fill(_ledArray.begin(), _ledArray.end(), CRGB::Black);
        SetBrightness( brightness );
    }

    void Clear()
    {
        _driver.clear();
    }

    void Present()
    {
        _driver.show();
    }

    void SetBrightness( float brightness )
    {
        float clamped = std::max( 0.0f, std::min( brightness, 1.0f ) );
        _driver.setBrightness( (uint8_t)( clamped * 255.0 ) );
    }

  protected:
    uint32_t          _ledCount = 0;
    std::vector<CRGB> _ledArray{};
    CFastLED          _driver{};
};