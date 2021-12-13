#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class PlayerComponent;
  struct NetworkPlayerManagerImpl;

  class NetworkPlayerManagerComponent : public SimpleGE::NetworkComponent
  {
  public:
    static constexpr auto Type = "NetworkPlayerManager";

    using PlayerReadyEventType = std::function<void(int)>;

    struct Description
    {
      struct PlayerEntry
      {
        SimpleGE::ComponentReference<PlayerComponent> player;
        SimpleGE::ComponentReference<SimpleGE::NetworkInputComponent> input;
      };

      std::vector<PlayerEntry> players;
    };

    static void SetLocalPlayerName(std::string_view name);

    NetworkPlayerManagerComponent(SimpleGE::Entity& owner);
    ~NetworkPlayerManagerComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void OnMessage(SimpleGE::Network::Connection& connection, const SimpleGE::BaseMessage& msg) override;

    void RegisterPlayerReadyEvent(const PlayerReadyEventType& onPlayerReady);

  private:
    std::unique_ptr<NetworkPlayerManagerImpl> impl;
  };
} // namespace ChickenDodge