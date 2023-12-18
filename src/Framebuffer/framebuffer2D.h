#pragma once

#include "LedFramebufferBase.h"

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

template <uint8_t DATA_PIN, EOrder RGB_ORDER>
class Framebuffer2D : public LedFramebufferBase<DATA_PIN, RGB_ORDER>
{
    using LedFramebufferBase<DATA_PIN, RGB_ORDER>::_ledArray;
    using LedFramebufferBase<DATA_PIN, RGB_ORDER>::_ledCount;

  public:
    Framebuffer2D( uint32_t width, uint32_t height, DataFlowOrigin origin = DataFlowOrigin::TOP_LEFT,
                   DataFlowDirection direction = DataFlowDirection::VERTICAL )
        : LedFramebufferBase<DATA_PIN, RGB_ORDER>( width * height )
        , _width{ width }
        , _height{ height }
        , _origin{ origin }
        , _direction{ direction }
    {
    }

    inline void SetPixel( uint32_t i, uint32_t j, CRGB value ) override
    {
        switch( _origin )
        {
        case DataFlowOrigin::TOP_RIGHT:
            if( _direction == DataFlowDirection::HORIZONTAL )
            {
                if( ( i & 0x01 ) == 0 )
                    index = i * _width + ( _width - j - 1 );
                else
                    index = i * _width + j;
            }
            else
            {
                // TODO
            }
            break;
        case DataFlowOrigin::TOP_LEFT:
        default:
            if( _direction == DataFlowDirection::HORIZONTAL )
            {
                // TODO, my led matrix has vertical strips
            }
            else
            {
                if( ( j & 0x01 ) == 1 )
                    index = ( j + 1 ) * _height - ( i + 1 );
                else
                    index = j * _height + i;
            }
            break;
        }
        _ledArray[index] = value;
    }

    void SetPixels( std::vector<CRGB> const &pixelValues ) override
    {
        std::copy( pixelValues.begin(), pixelValues.end(), _ledArray.begin() );
    }

  private:
    uint32_t          _width     = 0;
    uint32_t          _height    = 0;
    DataFlowOrigin    _origin    = DataFlowOrigin::TOP_LEFT;
    DataFlowDirection _direction = DataFlowDirection::VERTICAL;
};