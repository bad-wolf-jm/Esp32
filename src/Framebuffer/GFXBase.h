#pragma once

#include <Adafruit_GFX.h>
#include <FastLED.h>

class GFXBase : public Adafruit_GFX
{
  public:
    int Width;
    int Height;

  private:
    int _numPixelsPerGridRow;
    int _panelWidth;
    int _panelHeight;

  public:
    GFXBase( uint8_t panelWidth, uint8_t panelHeight, uint8_t gridWidth, uint8_t gridHeight )
        : Adafruit_GFX( panelWidth * gridWidth, panelHeight * gridHeight )
        , Width{ panelWidth * gridWidth }
        , Height{ panelHeight * gridHeight }
        , _panelWidth{ panelWidth }
        , _panelHeight{ panelHeight }
        , _numPixelsPerGridRow{ Width * panelHeight }
        , _pixels{ std::vector<CRGB>( Width * Height ) }
    {
    }

    static const uint8_t gamma5[];
    static const uint8_t gamma6[];
    static CRGB          from16Bit( uint16_t color ) // Convert 16bit 5:6:5 to 24bit color using lookup table for gamma
    {
        uint8_t r = gamma5[color >> 11];
        uint8_t g = gamma6[( color >> 5 ) & 0x3F];
        uint8_t b = gamma5[color & 0x1F];

        return CRGB( r, g, b );
    }

    static uint16_t to16bit( uint8_t r, uint8_t g, uint8_t b ) // Convert RGB -> 16bit 5:6:5
    {
        return ( ( r / 8 ) << 11 ) | ( ( g / 4 ) << 5 ) | ( b / 8 );
    }

    static uint16_t to16bit(const CRGB rgb) // Convert CRGB -> 16 bit 5:6:5
    {
        return ((rgb.r / 8) << 11) | ((rgb.g / 4) << 5) | (rgb.b / 8);
    }

    virtual void drawPixel( int16_t x, int16_t y, uint16_t color )
    {
        SetPixel( y, x, from16Bit( color ) );
    }

    inline void SetPixel( uint32_t i, uint32_t j, CRGB value )
    {
        _pixels[GetIndex( i, j )] = value;
    }

    void SetPixels( std::vector<CRGB> const &pixelValues )
    {
        std::copy( pixelValues.begin(), pixelValues.end(), _pixels.begin() );
    }

    std::vector<CRGB> &GetPixels()
    {
        return _pixels;
    }

    void DrawHLine(int x, int y, int w, CRGB color)
    {
        drawFastHLine(x, y, w, to16bit(color));
    }

    void DrawVLine(int x, int y, int h, CRGB color)
    {
        drawFastVLine(x, y, h, to16bit(color));
    }


    void DrawRect(int x, int y, int w, int h, CRGB color)
    {
        drawRect(x, y, w, h, to16bit(color));
    }

    void DrawFilledRect(int x, int y, int w, int h, CRGB color)
    {
        fillRect(x, y, w, h, to16bit(color));
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

    std::vector<CRGB> _pixels;
};