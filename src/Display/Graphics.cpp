#include "Graphics.h"

Graphics::Graphics( GraphicsConfiguration const &config )
    : _config{ config }
{
    _display = std::make_unique<TFT_eSPI>( config.Width, config.Height );
    _display->init();
    _display->setRotation( _config.Rotation );
    _display->fillScreen( TFT_NAVY );

    _backBuffer = std::make_unique<TFT_eSprite>( _display.get() );
    _backBuffer->setColorDepth( 16 );

    switch( _config.Rotation )
    {
    case 1:
    case 3:
        _backBuffer->createSprite( _config.Height, _config.Width );
        break;
    case 0:
    case 2:
    default:
        _backBuffer->createSprite( _config.Width, _config.Height );
        break;
    }
}

Graphics::~Graphics()
{
    _backBuffer->deleteSprite();
}

void Graphics::BeginFrame()
{
    _backBuffer->fillSprite( TFT_NAVY );
    _backBuffer->setTextColor( TFT_WHITE );
    _backBuffer->setTextSize( 1 );
    _backBuffer->setCursor( 0, 0 );
}

void Graphics::EndFrame()
{
    _backBuffer->pushSprite( 0, 0 );
}

void Graphics::DrawRectangle( int x0, int y0, int x1, int y1, uint32_t color )
{
    _backBuffer->drawLine( x0, y0, x1, y0, color );
    _backBuffer->drawLine( x1, y0, x1, y1, color );
    _backBuffer->drawLine( x1, y1, x0, y1, color );
    _backBuffer->drawLine( x0, y1, x0, y0, color );
}

void Graphics::DrawLine( int x0, int y0, int x1, int y1, uint32_t color )
{
    _backBuffer->drawLine( x0, y0, x1, y1, color );
}

void Graphics::DrawText( std::string text, int x, int y )
{
    _backBuffer->setTextColor( TFT_WHITE );
    _backBuffer->setTextFont( 2 );
    _backBuffer->setTextSize( 2 );
    _backBuffer->setTextDatum( TL_DATUM );
    _backBuffer->drawString( text.c_str(), x, y );
}

void Graphics::DrawText( String text, int x, int y )
{
    _backBuffer->setTextColor( TFT_WHITE );
    _backBuffer->setTextFont( 2 );
    _backBuffer->setTextSize( 1 );
    _backBuffer->setTextDatum( TL_DATUM );
    _backBuffer->drawString( text.c_str(), x, y );
}

void Graphics::Write( std::string const &line )
{
    _backBuffer->print( line.c_str() );
}

void Graphics::WriteLine( std::string const &line )
{
    _backBuffer->println( line.c_str() );
}
