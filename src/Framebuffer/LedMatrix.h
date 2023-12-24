#pragma once

#include "LedStripBase.h"

template <uint8_t DATA_PIN, EOrder RGB_ORDER>
class LedMatrix : public LedStripBase<DATA_PIN, RGB_ORDER>
{
  public:
    int Width;
    int Height;

  private:
    int _numPixelsPerGridRow;
    int _panelWidth;
    int _panelHeight;

    using LedStripBase<DATA_PIN, RGB_ORDER>::_ledArray;
    using LedStripBase<DATA_PIN, RGB_ORDER>::_ledCount;

  public:
    LedMatrix( uint8_t panelWidth, uint8_t panelHeight, uint8_t gridWidth, uint8_t gridHeight )
        : LedStripBase<DATA_PIN, RGB_ORDER>( panelWidth * gridWidth * panelHeight * gridHeight )
        , Width{ panelWidth * gridWidth }
        , Height{ panelHeight * gridHeight }
        , _panelWidth{ panelWidth }
        , _panelHeight{ panelHeight }
        , _numPixelsPerGridRow{ Width * panelHeight }
    {
    }

    inline void SetPixel( uint32_t i, uint32_t j, CRGB value )
    {
        _ledArray[GetIndex( i, j )] = value;
    }

    void SetPixels( std::vector<CRGB> const &pixelValues )
    {
        std::copy( pixelValues.begin(), pixelValues.end(), _ledArray.begin() );
    }

  private:
    inline int GetIndex( uint32_t i, uint32_t j )
    {
        int index   = 0;
        int gridRow = i / _panelHeight;

        i = i % _panelHeight;

        if( ( j & 0x01 ) == 1 )
            return _numPixelsPerGridRow * gridRow + ( ( j + 1 ) * _panelHeight - ( i + 1 ) );
        else
            return _numPixelsPerGridRow * gridRow + ( j * _panelHeight + i );
    }
};