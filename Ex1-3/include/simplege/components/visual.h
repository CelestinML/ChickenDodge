#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/timing.h>

namespace SimpleGE
{
  struct Vertex
  {
    float x, y, z;
    float u, v;
  };

  class VisualComponent : public virtual Component
  {
  public:

    inline VisualComponent();
    inline ~VisualComponent() override;

    virtual void Display(const Timing& timing) = 0;
  };
} // namespace SimpleGE
