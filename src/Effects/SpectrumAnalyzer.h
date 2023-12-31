#pragma once

#include "Framebuffer/GFXBase.h"
#include "Core/Vector.h"

class SpectrumAnalyzer
{
  public:
    SpectrumAnalyzer( int numBands )
        : _numBands( numBands )
        , _amplitudes{ vector_t<double>( _numBands ) }
        , _peaks{ vector_t<double>( _numBands ) }
    {
    }

    void SetPeaks( vector_t<double> const &peaks, float ts );
    void Render( GFXBase &renderer );

  private:
    int                 _numBands;
    vector_t<double> _amplitudes;
    vector_t<double> _peaks;
};