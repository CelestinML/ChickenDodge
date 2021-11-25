#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/logic.h>

namespace SimpleGE
{
  struct DebugDrawCallsImpl;

  class DebugDrawCallsComponent : public LogicComponent
  {
  public:
    static constexpr auto Type = "DebugDrawCalls";

    DebugDrawCallsComponent(Entity& owner);
    ~DebugDrawCallsComponent() override;

    void UpdateLogic(const Timing& timing) override;

  private:
    std::unique_ptr<DebugDrawCallsImpl> impl;
  };
} // namespace SimpleGE
