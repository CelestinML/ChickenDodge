#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/timing.h>

namespace SimpleGE
{
  class VisualComponent : public virtual Component
  {
  public:
    inline VisualComponent();
    inline ~VisualComponent() override;

    virtual void Display(const Timing& timing) = 0;
  };
} // namespace SimpleGE
