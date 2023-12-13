#pragma once

#include <FastLED.h>
#include <vector>

class LedStrip
{
public:
    LedStrip(uint32_t length, bool isReversed);

    void Clear();
    void Present();

    void SetPixel(uint32_t i, CRGB value);
    void SetPixels(std::vector<CRGB> pixelValues);

private:
    uint32_t _length = 0;
    uint32_t _isReversed = 0;
    std::vector<CRGB> _ledArray;
    CFastLED _driver;
};