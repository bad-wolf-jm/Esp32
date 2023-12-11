#pragma once

#include <FastLED.h>
#include <vector>

enum DataFlowDirection
{
    HORIZONTAL,
    VERTICAL
};

enum DataFlowOrigin
{
    TOP_LEFT,
    TOP_RIGHT
    // BOTTOM_LEFT,
    // BOTTOM_RIGHT
};

class framebuffer2D_t
{
public:
    framebuffer2D_t(uint32_t width, uint32_t height,
             DataFlowOrigin origin = DataFlowOrigin::TOP_LEFT, DataFlowDirection direction = DataFlowDirection::VERTICAL);
    void Clear();
    void Present();
    void SetPixel(uint32_t i, uint32_t j, CRGB value);
    void SetPixels(std::vector<CRGB> pixelValues);

private:
    uint32_t _width = 0;
    uint32_t _height = 0;
    DataFlowOrigin _origin = DataFlowOrigin::TOP_LEFT;
    DataFlowDirection _direction = DataFlowDirection::VERTICAL;

    CFastLED _driver;
    std::vector<CRGB> _framebuffer;
    std::vector<CRGB> _ledArray;
};