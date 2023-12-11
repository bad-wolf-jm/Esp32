#include "framebuffer2D.h"

framebuffer2D_t::framebuffer2D_t(uint32_t width, uint32_t height, DataFlowOrigin origin, DataFlowDirection direction)
    : _width{width}, _height{height}, _ledArray{std::vector<CRGB>(width * height)}, _origin{origin}, _direction{direction}, _framebuffer{std::vector<CRGB>(width * height)}
{
    _driver.addLeds<WS2812B, 25>(_ledArray.data(), width * height);
    _driver.setBrightness(2);
}

void framebuffer2D_t::Clear()
{
    for(auto& v : _framebuffer) v = CRGB::Black;
}

void framebuffer2D_t::Present()
{
    uint32_t numPixels = _width * _height;
    for (uint32_t k = 0; k < numPixels; k++)
    {
        uint32_t i = k / _width;
        uint32_t j = k % _width;

        uint32_t index = 0;
        switch (_origin)
        {
        case DataFlowOrigin::TOP_RIGHT:
            if (_direction == DataFlowDirection::HORIZONTAL)
            {
                if (i % 2 == 0)
                    index = i * _width + (_width - j - 1);
                else
                    index = i * _width + j;
            }
            else
            {
                // TODO
            }
            break;
        case DataFlowOrigin::BOTTOM_LEFT:
            i = (_height - 1 - i);
            if (_direction == DataFlowDirection::HORIZONTAL)
            {
                if (i % 2 == 0)
                    index = i * _width + j;
                else
                    index = i * _width + (_width - j - 1);
            }
            else
            {
                // TODO
            }
            break;
        case DataFlowOrigin::BOTTOM_RIGHT:
            if (_direction == DataFlowDirection::HORIZONTAL)
            {
                // TODO
            }
            else
            {
                j = (_width - 1 - j);

                if (j % 2 == 0) // going up (inverted)
                    index = j * _height + (_height - 1 - i);
                else // going down (correct)
                    index = j * _height + i;
            }
            break;
        case DataFlowOrigin::TOP_LEFT:
        default:
            if (_direction == DataFlowDirection::HORIZONTAL)
            {
                // TODO
            }
            else
            {
                if (j % 2 == 1)
                    index = (j + 1) * _height - (i + 1);
                else
                    index = j * _height + i;
            }
            break;
        }

        _ledArray[index] = _framebuffer[k];
    }
    _driver.show();
}

void framebuffer2D_t::SetPixel(uint32_t i, uint32_t j, CRGB value)
{
    uint32_t index = i * _width + j;
    _framebuffer[index] = value;
}

void framebuffer2D_t::SetPixels(std::vector<CRGB> pixelValues)
{
    std::copy(pixelValues.begin(), pixelValues.end(), _framebuffer.begin());
}