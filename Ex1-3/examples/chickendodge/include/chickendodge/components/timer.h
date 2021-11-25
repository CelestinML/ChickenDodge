#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct TimerImpl;

  class TimerComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Timer";

    struct Description
    {
    };

    TimerComponent(SimpleGE::Entity& owner);
    ~TimerComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

  protected:
    void OnEnabled() override;
    void OnDisabled() override;

  private:
    std::unique_ptr<TimerImpl> impl;
  };
} // namespace ChickenDodge