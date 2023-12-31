#include "Glow2D.h"

Glow2D::Glow2D( uint32_t length )
    : _length{ length }
    , _heat{ vector_t<uint8_t>( _length ) }
    , _colors{ vector_t<CRGB>( _length ) }

{
}

void Glow2D::Update()
{
    for( auto &v : _heat )
    {
        int Cooling       = 20;
        int coolingAmount = random( 0, Cooling );

        v = ::max( 100, v - coolingAmount );
    }

    int count = random( _length / 6 );
    for( int i = 0; i < count; i++ )
    {
        int index = random( _length );
        _heat[index] += random( 128 );
    }

    for( int i = 0; i < _length; i++ )
        _colors[i] = HeatColor( _heat[i] );
}