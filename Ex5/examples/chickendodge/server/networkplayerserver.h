#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct NetworkPlayerServerImpl;

  class NetworkPlayerServerComponent : public SimpleGE::NetworkComponent
  {
  public:
    static constexpr auto Type = "NetworkPlayerServer";

    static void OnConnect(const std::shared_ptr<SimpleGE::Network::Connection>& connection)
    {
      Instance().OnConnectImpl(connection);
    }

    static void OnDisconnect(gsl::not_null<SimpleGE::Network::Connection*> connection)
    {
      Instance().OnDisconnectImpl(connection);
    }

    NetworkPlayerServerComponent(SimpleGE::Entity& owner);
    ~NetworkPlayerServerComponent() override;

    void OnMessage(SimpleGE::Network::Connection& connection, const SimpleGE::BaseMessage& msg) override;

  private:
    [[nodiscard]] static NetworkPlayerServerComponent& Instance()
    {
      NetworkPlayerServerComponent* instance = InstancePtr();
      Expects(instance != nullptr);
      return *instance;
    }

    [[nodiscard]] static NetworkPlayerServerComponent*& InstancePtr()
    {
      static NetworkPlayerServerComponent* instance{};
      return instance;
    }

    void OnConnectImpl(const std::shared_ptr<SimpleGE::Network::Connection>& connection);
    void OnDisconnectImpl(gsl::not_null<SimpleGE::Network::Connection*> connection);

    std::unique_ptr<NetworkPlayerServerImpl> impl;
  };
} // namespace ChickenDodge