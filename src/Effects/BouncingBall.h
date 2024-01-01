#pragma once
#include "EffectBase.h"

struct BouncingBall
{
    float Position;
    float Speed;
    CRGB  Color;

    void Update( float dt )
    {
        Position += Speed * dt;

        // v = a*t + v0;
        // dv = a*dt
        if (Position <= 0.0f)
            Speed *= -1.0f;
        else
            Speed += -9.8 * dt;

    }

    void Render( LedStripRenderer &renderer )
    {
        renderer.SetPixel( Position, Color );
    }
};

class BouncingBallEffect : public LedStripEffectBase
{
  public:
    virtual void Update( float ts )
    {
        auto it = _balls.begin();
        while( it != _balls.end() )
        {
            ( *it ).Update( ts );
            it++;
        }
    }

    virtual void Render( LedStripRenderer &renderer )
    {
        for( auto &shot : _balls )
            shot.Render( renderer );
    }

    void Fire()
    {
        static CRGB colors[] = { CRGB::Red, CRGB::Magenta, CRGB::Green, CRGB::Blue };
        float       value    = random16() / static_cast<float>( UINT16_MAX );
        float       speed    = random16() / static_cast<float>( UINT16_MAX );
        _balls.emplace_back( BouncingBall{ 0.0f, 4 * speed, colors[random8( 4 )] } );
    }

  private:
    vector_t<BouncingBall> _balls;
};