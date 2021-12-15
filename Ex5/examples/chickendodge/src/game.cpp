#include "chickendodge/pch/precomp.h"

#include "chickendodge/components.h"
#include "chickendodge/messages.h"

#include <thread>

static const SimpleGE::LocaleFiles Locales = {
    {"en", "locales/en.json"},
    {"fr", "locales/fr.json"},
};

constexpr auto TeamFile = "equipe.txt";
constexpr auto TeamDefaultString = "Coéquipiers";
constexpr auto LaunchScene = "scenes/play.json";
constexpr auto DefaultLang = "fr";
constexpr auto DefaultName = "Anonyme";
constexpr auto DefaultServer = "127.0.0.1";
constexpr std::uint16_t DefaultPort = 4321;

namespace ChickenDodge
{
  class Game : public SimpleGE::Game
  {
  public:
    Game(gsl::span<char*> args) : SimpleGE::Game(args)
    {
      const char* alias = (args.size() > 1) ? gsl::at(args, 1) : DefaultName;
      const char* server = (args.size() > 2) ? gsl::at(args, 2) : DefaultServer;
      std::uint16_t port = (args.size() > 3) ? std::atoi(gsl::at(args, 3)) : DefaultPort;

      // Debug delay, for multi launch
      if (args.size() > 3)
      {
        using namespace std::chrono_literals;
        static constexpr auto DebugDelay = 2s;
        std::this_thread::sleep_for(DebugDelay);
      }

      serverConnection = SimpleGE::Network::Connect(
          server, port, [](gsl::not_null<SimpleGE::Network::Connection*> connection) { OnDisconnect(connection); },
          [](gsl::not_null<SimpleGE::Network::Connection*> connection, gsl::span<std::byte> data)
          { OnData(connection, data); });

      NetworkPlayerManagerComponent::SetLocalPlayerName(alias);
    }

  protected:
    [[nodiscard]] std::string_view GetLaunchScene() const override { return LaunchScene; }

    void SetupSystem() override
    {
      AddSystem<SimpleGE::NetworkSystem>();
      AddSystem<SimpleGE::PhysicSystem>();
      AddSystem<SimpleGE::LogicSystem>();
      AddSystem<SimpleGE::DisplaySystem>();
    }

    void RegisterComponents() const override { RegisterGameComponents(); }
    void RegisterMessages() const override { RegisterGameMessages(); }

  private:
    static void OnDisconnect(gsl::not_null<SimpleGE::Network::Connection*> connection)
    {
      fmt::print(stderr, "Connexion au serveur perdue, fermeture.\n");
      Close();
    }

    static void OnData(gsl::not_null<SimpleGE::Network::Connection*> connection, gsl::span<std::byte> data)
    {
      SimpleGE::NetworkSystem::Recv(*connection, data);
    }

    std::shared_ptr<SimpleGE::Network::Connection> serverConnection;
  };
} // namespace ChickenDodge

[[nodiscard]] static bool ValidateTeam()
{
  auto teamFile = SimpleGE::Resources::Get<SimpleGE::TextAsset>(TeamFile);
  if (teamFile == nullptr)
  {
    fmt::print(stderr, "Échec d'ouverture de {}\n", TeamFile);
    return false;
  }

  if ((std::string_view) *teamFile == TeamDefaultString)
  {
    fmt::print(stderr, "N'oubliez pas d'inscrire les membres de votre équipe dans le fichier data/equipe.txt!\n");
    return false;
  }

  return true;
}

[[nodiscard]] static bool SetupLocales()
{
  const char* lang = std::getenv("LANG");
  if (lang == nullptr)
  {
    lang = DefaultLang;
  }
  SimpleGE::Localisation::Init(Locales, std::string_view(lang, 2));

  return true;
}

static void ShowInstructions()
{
  fmt::print("{}: {}\n", SimpleGE::Localisation::Get("CHAR_KEYS"), SimpleGE::Localisation::Get("MOVE"));
  fmt::print("{}: {}\n", SimpleGE::Localisation::Get("SPACE_KEY"), SimpleGE::Localisation::Get("ATTACK"));
}

int main(int argc, char* argv[])
{
  if (!ValidateTeam())
  {
    return -1;
  }

  if (!SetupLocales())
  {
    return -1;
  }

  ShowInstructions();

  ChickenDodge::Game game({argv, (size_t) argc});
  auto runResult = game.Run();
  return runResult == SimpleGE::Game::RunResult::Success ? 0 : 1;
}