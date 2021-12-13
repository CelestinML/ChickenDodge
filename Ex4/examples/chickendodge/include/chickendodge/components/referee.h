#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct RefereeImpl;

  class PlayerComponent;

  class RefereeComponent : public SimpleGE::Component
  {
  public:
    static constexpr auto Type = "Referee";

    using WinEventType = std::function<void(PlayerComponent&, PlayerComponent&)>;

    struct Description
    {
      std::array<SimpleGE::ComponentReference<PlayerComponent>, 2> players;
    };

    RefereeComponent(SimpleGE::Entity& owner);
    ~RefereeComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void RegisterWinEvent(const WinEventType& onWin);

  private:
    std::unique_ptr<RefereeImpl> impl;
  };
} // namespace ChickenDodge