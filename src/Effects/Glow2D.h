#pragma once
#include <vector>
#include "FastLED.h"

class Glow2D
{
public:
    Glow2D(uint32_t length);

    void Update();

    std::vector<CRGB> &GetColors() { return _colors; };

private:
    uint32_t _length = 0;
    std::vector<uint8_t> _heat;
    std::vector<CRGB> _colors;
};