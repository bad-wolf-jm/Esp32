#pragma once

#include "Framebuffer/GFXBase.h"
#include <vector>

class SpectrumAnalyzer
{
  public:
    SpectrumAnalyzer( int numBands )
        : _numBands( numBands )
        , _amplitudes{ std::vector<double>( _numBands ) }
        , _peaks{ std::vector<double>( _numBands ) }
    {
    }

    void SetPeaks( std::vector<double> const &peaks, float ts )
    {
        std::copy( peaks.begin(), peaks.end(), _amplitudes.begin() );
        for( int i = 0; i < _amplitudes.size(); i++ )
        {
            if( _peaks[i] > _amplitudes[i] )
                _peaks[i] -= 1.5 * 4096 * ts;

            _peaks[i] = std::max( _peaks[i], _amplitudes[i] );
        }
    }

    void Render( GFXBase &renderer )
    {
        int totalHeight = renderer.Height - 2;
        int bandWidth   = renderer.Width / _amplitudes.size();
        int bandX       = 0;

        static CRGB colors[] = { CRGB::IndianRed, CRGB::GreenYellow, CRGB::BlueViolet };

        for( size_t i = 0; i < _amplitudes.size(); i++ )
        {
            int bandHeight = ( _amplitudes[i] / 4096.0 ) * totalHeight;
            int bandX      = i * bandWidth;
            int bandY      = renderer.Height - bandHeight;
            renderer.DrawFilledRect( bandX, bandY, bandWidth, bandHeight, colors[i % 3] );

            int peakHeight = ( _peaks[i] / 4096.0 ) * totalHeight;
            int peakY      = renderer.Height - peakHeight - 1;
            renderer.DrawHLine( bandX, peakY, bandWidth, CRGB::AntiqueWhite );
        }
    }

  private:
    int                 _numBands;
    std::vector<double> _amplitudes;
    std::vector<double> _peaks;
};