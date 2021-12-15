#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct NetworkLeaderboardImpl;

  class NetworkLeaderboardComponent : public SimpleGE::NetworkComponent
  {
  public:
    static constexpr auto Type = "NetworkLeaderboard";

    NetworkLeaderboardComponent(SimpleGE::Entity& owner);
    ~NetworkLeaderboardComponent() override;

    void OnMessage(SimpleGE::Network::Connection& connection, const SimpleGE::BaseMessage& msg) override;

  private:
    std::unique_ptr<NetworkLeaderboardImpl> impl;
  };
} // namespace ChickenDodge