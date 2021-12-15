#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class NetworkPlayerManagerComponent;

  struct CountdownImpl;

  class CountdownComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Countdown";

    struct Description
    {
      std::vector<std::string> sprites;
      std::string waitSprite;
      std::string playerSpritePrefix;
      nlohmann::json spriteTemplate;
      std::chrono::milliseconds delay;
      SimpleGE::ComponentReference<NetworkPlayerManagerComponent> playerWait;
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