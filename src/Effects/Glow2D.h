#pragma once
#include "FastLED.h"

#include "Core/Vector.h"

class Glow2D
{
  public:
    Glow2D( uint32_t length );

    void Update();

    vector_t<CRGB> &GetColors()
    {
        return _colors;
    };

  private:
    uint32_t          _length = 0;
    vector_t<uint8_t> _heat;
    vector_t<CRGB>    _colors;
};