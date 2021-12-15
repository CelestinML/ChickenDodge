#include "chickendodge/pch/precomp.h"

#include "networkplayerserver.h"

#include "chickendodge/messages/networklogin.h"
#include "chickendodge/messages/networkstart.h"
#include "chickendodge/messages/networkscore.h"
#include "chickendodge/messages/networkleaderchanged.h"

using namespace SimpleGE;

namespace ChickenDodge
{
  struct SocketData
  {
    SocketData(const std::shared_ptr<Network::Connection>& player) : player(player) {}

    std::shared_ptr<Network::Connection> player;
    std::weak_ptr<Network::Connection> otherPlayer;
    std::string name;
  };

  struct NetworkPlayerServerImpl
  {
    std::unordered_map<Network::Connection::ConnectionIDType, SocketData> clients;
    std::unordered_set<Network::Connection::ConnectionIDType> pendingPlayers;
    std::pair<Network::Connection::ConnectionIDType, int> bestPlayerScore;

    void OnMessage(Network::Connection& connection, const BaseMessage& msg)
    {
      fmt::print("Message de {}\n", connection.GetID());
      if (msg.Is<NetworkLogin>())
      {
        OnNetworkLogin(connection, msg.Get<NetworkLogin>());

        //We send the newly connected player the leaderboard
        NetworkLeaderChanged leaderUpdateMsg = NetworkLeaderChanged(bestPlayerScore.first, bestPlayerScore.second);
        auto player = clients.find(connection.GetID())->second.player;
        NetworkSystem::Send(player.get(), leaderUpdateMsg);

      }
      if (msg.Is<NetworkInputChanged>())
      {
        auto otherPlayer = clients.find(connection.GetID())->second.otherPlayer.lock();
        if (otherPlayer != nullptr)
        {
          NetworkSystem::Send(otherPlayer.get(), msg);
        }
      }
      if (msg.Is<NetworkScore>()) {
        //We get the msg value
        NetworkScore scoreMsg = msg.Get<NetworkScore>();
        //We print the received score for debug
        fmt::print("Nouveau score : {}\n", scoreMsg.score);
        if (bestPlayerScore.second < scoreMsg.score) {
          //Register best player
          bestPlayerScore = {connection.GetID(), scoreMsg.score};
          //Send best player to clients
          NetworkLeaderChanged leaderUpdateMsg = NetworkLeaderChanged(bestPlayerScore.first, bestPlayerScore.second);
          auto otherPlayer = clients.find(connection.GetID())->second.otherPlayer.lock();
          NetworkSystem::Send(otherPlayer.get(), leaderUpdateMsg);
          auto player = clients.find(connection.GetID())->second.player;
          NetworkSystem::Send(player.get(), leaderUpdateMsg);
        }
      }
    }

    void OnNetworkLogin(Network::Connection& connection, const NetworkLogin& msg)
    {
      auto id = connection.GetID();
      auto& socketData = clients.find(id)->second;
      socketData.name = msg.name;

      // Si aucun joueur n'est en attente, on place le nouveau
      // joueur en attente.
      if (pendingPlayers.empty())
      {
        pendingPlayers.insert(id);
        return;
      }

      // Si il y a des joueurs en attente, on associe un de
      // ces joueurs à celui-ci.
      auto otherId = *pendingPlayers.begin();
      pendingPlayers.erase(otherId);

      auto& otherSocketData = clients.find(otherId)->second;
      socketData.otherPlayer = otherSocketData.player;
      otherSocketData.otherPlayer = socketData.player;

      // On envoie alors la liste des joueurs de la partie
      // à chacun des participants.
      NetworkStart p1(0, {otherSocketData.name, socketData.name});
      NetworkStart p2(1, {otherSocketData.name, socketData.name});

      NetworkSystem::Send(otherSocketData.player.get(), p1);
      NetworkSystem::Send(socketData.player.get(), p2);
    }

    void OnConnect(const std::shared_ptr<SimpleGE::Network::Connection>& connection)
    {
      fmt::print("Connexion de {}\n", connection->GetID());
      Expects(clients.find(connection->GetID()) == clients.end());
      clients.emplace(connection->GetID(), connection);
    }

    void OnDisconnect(gsl::not_null<SimpleGE::Network::Connection*> connection)
    {
      auto id = connection->GetID();
      fmt::print("Déconnexion de {}\n", id);
      Expects(clients.find(id) != clients.end());

      auto& socketData = clients.find(id)->second;
      auto otherPlayer = socketData.otherPlayer.lock();
      if (otherPlayer != nullptr)
      {
        clients.erase(otherPlayer->GetID());
      }

      pendingPlayers.erase(id);
      clients.erase(id);
    }
  };

  NetworkPlayerServerComponent::NetworkPlayerServerComponent(Entity& owner)
      : Component(owner), impl(std::make_unique<NetworkPlayerServerImpl>())
  {
    Expects(InstancePtr() == nullptr);
    InstancePtr() = this;
  }

  NetworkPlayerServerComponent::~NetworkPlayerServerComponent() { InstancePtr() = nullptr; }

  void NetworkPlayerServerComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    impl->OnMessage(connection, msg);
  }

  void NetworkPlayerServerComponent::OnConnectImpl(const std::shared_ptr<Network::Connection>& connection)
  {
    impl->OnConnect(connection);
  }

  void NetworkPlayerServerComponent::OnDisconnectImpl(gsl::not_null<Network::Connection*> connection)
  {
    impl->OnDisconnect(connection);
  }
} // namespace ChickenDodge