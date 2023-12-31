#pragma once
// #include "Framebuffer/LedStrip.h"
#include "LedStripRenderer.h"

class LedStripEffectBase
{
  public:
    virtual void Update( float ts )                   = 0;
    virtual void Render( LedStripRenderer &renderer ) = 0;
};