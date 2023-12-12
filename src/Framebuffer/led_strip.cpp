#include "led_strip.h"

LedStrip::LedStrip(uint32_t length, bool isReversed)
    : _length{length}, _isReversed{isReversed}, _ledArray{std::vector<CRGB>(length)}
{
    _driver.addLeds<WS2812B, 25>(_ledArray.data(), Length);
    _driver.setBrightness(2);
}

void LedStrip::Clear()
{
    _driver.clear();
}

void LedStrip::Present()
{
    _driver.show();
}

void LedStrip::SetPixel(uint32_t i, CRGB value)
{
    if (_isReversed)
        _ledArray[_length - i] = value;
    else
        _ledArray[i] = value;
}

void LedStrip::SetPixels(std::vector<CRGB> pixelValues)
{
    std::copy(pixelValues.begin(), pixelValues.end(), _ledArray.begin());
}