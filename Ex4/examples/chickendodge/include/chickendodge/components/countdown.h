#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct CountdownImpl;

  class CountdownComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Countdown";

    struct Description
    {
      std::vector<std::string> sprites;
      nlohmann::json spriteTemplate;
      std::chrono::milliseconds delay;
    };

    CountdownComponent(SimpleGE::Entity& owner);
    ~CountdownComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

  private:
    std::unique_ptr<CountdownImpl> impl;
  };
} // namespace ChickenDodge