#pragma once

#include <FastLED.h>
#include <vector>

template <int LED_COUNT, uint8_t DATA_PIN, EOrder RGB_ORDER>
class LedStripBase
{
  public:
    LedStripBase( )
        : _ledCount{ LED_COUNT }
        , _ledArray{ std::vector<CRGB>( LED_COUNT ) }

    {
        _driver.addLeds<WS2812, DATA_PIN, RGB_ORDER>( _ledArray.data(), _ledCount );
        _driver.setBrightness( 2 );
    }

    void Clear()
    {
        _driver.clear();
    }

    void Present()
    {
        _driver.show();
    }

  public:
    uint32_t          _ledCount = 0;
    std::vector<CRGB> _ledArray{};
    CFastLED          _driver{};
};