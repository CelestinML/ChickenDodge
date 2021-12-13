#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/networkplayermanager.h"

#include "chickendodge/components/player.h"

#include "chickendodge/messages/networklogin.h"
#include "chickendodge/messages/networkstart.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, NetworkPlayerManagerComponent::Description::PlayerEntry& entry)
  {
    j.at("player").get_to(entry.player);
    j.at("input").get_to(entry.input);
  }

  static void from_json(const json& j, NetworkPlayerManagerComponent::Description& desc)
  {
    j.at("players").get_to(desc.players);
  }

  struct NetworkPlayerManagerImpl
  {
    using PlayerEntry = NetworkPlayerManagerComponent::Description::PlayerEntry;

    static std::string localPlayerName;

    EventTrigger<NetworkPlayerManagerComponent::PlayerReadyEventType> readyEvent;
    std::vector<PlayerEntry> players;
  };
  std::string NetworkPlayerManagerImpl::localPlayerName{};

  void NetworkPlayerManagerComponent::SetLocalPlayerName(std::string_view name)
  {
    NetworkPlayerManagerImpl::localPlayerName = name;
  }

  NetworkPlayerManagerComponent::NetworkPlayerManagerComponent(Entity& owner)
      : Component(owner), impl(std::make_unique<NetworkPlayerManagerImpl>())
  {
  }

  NetworkPlayerManagerComponent::~NetworkPlayerManagerComponent() = default;

  Component::SetupResult NetworkPlayerManagerComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult NetworkPlayerManagerComponent::Setup(const Description& descr)
  {
    impl->players = descr.players;
    NetworkLogin msg(impl->localPlayerName);
    NetworkSystem::Send(msg);

    return {this, {}};
  }

  void NetworkPlayerManagerComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    if (!msg.Is<NetworkStart>())
    {
      return;
    }

    const auto& startMsg = msg.Get<NetworkStart>();

    for (int i = 0; i < startMsg.names.size(); i++)
    {
      bool isLocal = (i == startMsg.playerIndex);
      auto& entry = gsl::at(impl->players, i);
      entry.player->SetName(startMsg.names[i]);
      entry.player->SetLocal(isLocal);
      entry.input->SetLocal(isLocal);
    }

    impl->readyEvent.Trigger(startMsg.playerIndex);
  }

  void NetworkPlayerManagerComponent::RegisterPlayerReadyEvent(const PlayerReadyEventType& onPlayerReady)
  {
    impl->readyEvent.Register(onPlayerReady);
  }
} // namespace ChickenDodge
