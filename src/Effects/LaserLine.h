#pragma once
#include "EffectBase.h"

struct LaserShot
{
    float Position;
    float Length;
    float Speed;
};

class LaserLineEffect : public LedStripEffectBase
{
  public:
    virtual void Update( float ts )
    {
        auto it = _shots.begin();
        while( it != _shots.end() )
        {
            ( *it ).Position += ( *it ).Speed * ts;
            if( ( *it ).Position > ( *it ).Length + 1.0 )
            {
                _shots.erase( it );
            }
            else
                it++;
        }
    }

    virtual void Render( LedStripRenderer &renderer )
    {
        for( auto &shot : _shots )
        {
            CRGB color = CRGB::Magenta;
            for( float position = shot.Position; position > shot.Position - shot.Length; position -= 0.01 )
            {
                renderer.SetPixel( position, color );
                // color.fadeToBlackBy( 15 );
            }
        }
    }

    void Fire()
    {
        _shots.emplace_back( LaserShot{ 0.0f, 0.2f, 2.0f } );
    }

  private:
    std::vector<LaserShot> _shots;
};