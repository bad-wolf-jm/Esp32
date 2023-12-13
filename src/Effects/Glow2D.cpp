#include "Glow2D.h"

Glow2D::Glow2D(uint32_t length)
    : _length{length}
    , _heat{std::vector<uint8_t>(_length)}
    , _colors{std::vector<CRGB>(_length)}

{

}

void Glow2D::Update()
{
    for(auto& v : _heat)
        v = (uint8_t) (v * 0.95);

    int count = random(_length / 12);
    for(int i=0; i < count; i++)
    {
        int index = random(_length);
        _heat[index] = 255;
    }

    for(int i=0; i < _length; i++)
        _colors[i] = HeatColor(_heat[i]);
}