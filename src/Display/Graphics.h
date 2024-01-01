#pragma once

#include "Modules/Definitions.h"

#include <TFT_eSPI.h>
#include <array>
#include <memory>

struct GraphicsConfiguration
{
    uint32_t Width    = 1;
    uint32_t Height   = 1;
    uint8_t  Rotation = 0;
};

class Graphics
{
  private:
    std::unique_ptr<TFT_eSPI>    _display;
    std::unique_ptr<TFT_eSprite> _backBuffer;
    GraphicsConfiguration        _config{};

  public:
    Graphics( GraphicsConfiguration const &config );
    ~Graphics();

    void BeginFrame();
    void EndFrame();

    void DrawRectangle( int x0, int y0, int x1, int y1, uint32_t color );
    void DrawLine( int x0, int y0, int x1, int y1, uint32_t color );
    void DrawText( std::string text, int x, int y );
    void DrawText( String text, int x, int y );
    void Write( std::string const &line );
    void WriteLine( std::string const &line );
};