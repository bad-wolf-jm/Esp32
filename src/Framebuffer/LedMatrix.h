#pragma once

#include "LedStripBase.h"

template <uint8_t PANEL_WIDTH, uint8_t PANEL_HEIGHT, uint8_t GRID_WIDTH, uint8_t GRID_HEIGHT, uint8_t DATA_PIN, EOrder RGB_ORDER>
class LedMatrix : public LedStripBase<PANEL_WIDTH * GRID_WIDTH * PANEL_HEIGHT * GRID_HEIGHT, DATA_PIN, RGB_ORDER>
{

  public:
    static constexpr int Width  = PANEL_WIDTH * GRID_WIDTH;
    static constexpr int Height = PANEL_HEIGHT * GRID_HEIGHT;

  private:
    static constexpr int _numPixelsPerGridRow = Width * PANEL_HEIGHT;

    using LedStripBase<Width * Height, DATA_PIN, RGB_ORDER>::_ledArray;
    using LedStripBase<Width * Height, DATA_PIN, RGB_ORDER>::_ledCount;

  public:
    LedMatrix()
        : LedStripBase<Width * Height, DATA_PIN, RGB_ORDER>()
    {
    }

    inline void SetPixel( uint32_t i, uint32_t j, CRGB value )
    {
        _ledArray[GetIndex(i, j)] = value;
    }

    void SetPixels( std::vector<CRGB> const &pixelValues )
    {
        std::copy( pixelValues.begin(), pixelValues.end(), _ledArray.begin() );
    }

  private:
    inline int GetIndex( uint32_t i, uint32_t j )
    {
        int index   = 0;
        int gridRow = i / PANEL_HEIGHT;

        i = i % PANEL_HEIGHT;

        if( ( j & 0x01 ) == 1 )
            return _numPixelsPerGridRow * gridRow + ( ( j + 1 ) * PANEL_HEIGHT - ( i + 1 ) );
        else
            return _numPixelsPerGridRow * gridRow + ( j * PANEL_HEIGHT + i );
    }
};