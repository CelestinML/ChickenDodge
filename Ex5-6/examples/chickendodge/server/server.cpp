#include "chickendodge/pch/precomp.h"

#include "chickendodge/components.h"
#include "chickendodge/messages.h"
#include "networkplayerserver.h"

constexpr auto LaunchScene = "scenes/server.json";
constexpr std::uint16_t DefaultPort = 4321;

namespace ChickenDodge
{
  class Server : public SimpleGE::Game
  {
  public:
    Server(gsl::span<char*> args) : SimpleGE::Game(args)
    {
      std::uint16_t port = (args.size() > 1) ? std::atoi(gsl::at(args, 1)) : DefaultPort;

      server = SimpleGE::Network::Listen(
          port,
          [](const std::shared_ptr<SimpleGE::Network::Connection>& connection)
          { NetworkPlayerServerComponent::OnConnect(connection); },
          [](gsl::not_null<SimpleGE::Network::Connection*> connection)
          { NetworkPlayerServerComponent::OnDisconnect(connection); },
          [this](gsl::not_null<SimpleGE::Network::Connection*> connection, gsl::span<std::byte> data)
          { OnData(connection, data); });
    }

  protected:
    [[nodiscard]] std::string_view GetLaunchScene() const override { return LaunchScene; }

    void SetupSystem() override
    {
      AddSystem<SimpleGE::NetworkSystem>();
      AddSystem<SimpleGE::LogicSystem>();
    }

    void RegisterComponents() const override
    {
      RegisterGameComponents();
      RegisterServerComponents();
    }

    void RegisterMessages() const override { RegisterGameMessages(); }

  private:
    static void RegisterServerComponents()
    {
      using Component = SimpleGE::Component;

      Component::Register<NetworkPlayerServerComponent>();
    }

    void OnData(gsl::not_null<SimpleGE::Network::Connection*> connection, gsl::span<std::byte> data)
    {
      SimpleGE::NetworkSystem::Recv(*connection, data);
    }

    std::shared_ptr<SimpleGE::Network::Server> server;
  };
} // namespace ChickenDodge

int main(int argc, char* argv[])
{
  ChickenDodge::Server server({argv, (size_t) argc});
  auto runResult = server.Run();
  return runResult == SimpleGE::Game::RunResult::Success ? 0 : 1;
}