#include "chickendodge/pch/precomp.h"

#include "chickendodge/components.h"

#include <fmt/core.h>

static const SimpleGE::LocaleFiles Locales = {
    {"en", "locales/en.json"},
    {"fr", "locales/fr.json"},
};

constexpr auto TeamFile = "equipe.txt";
constexpr auto TeamDefaultString = "Coéquipiers";
constexpr auto LaunchScene = "scenes/play.json";
constexpr auto SettingsFile = "settings.json";

namespace ChickenDodge
{
  class Game : public SimpleGE::Game
  {
  public:
    Game(gsl::span<char*> args) : SimpleGE::Game(args) {}

  protected:
    [[nodiscard]] std::string_view GetLaunchScene() const override { return LaunchScene; }

    void SetupSystem() override
    {
      AddSystem<SimpleGE::PhysicSystem>();
      AddSystem<SimpleGE::LogicSystem>();
      AddSystem<SimpleGE::DisplaySystem>();
    }

    void RegisterComponents() const override { RegisterGameComponents(); }
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
  std::unordered_map<std::string, std::string> settings;
  auto settingsFile = SimpleGE::Resources::Get<SimpleGE::TextAsset>(SettingsFile);
  if (settingsFile == nullptr)
  {
    fmt::print(stderr, "Échec d'ouverture de {}\n", SettingsFile);
    return false;
  }

  nlohmann::json::parse(settingsFile->Value()).get_to(settings);
  SimpleGE::Localisation::Init(Locales, settings.at("lang"));

  SimpleGE::Localisation::SetContext("PLAYER_1", settings.at("alias1"));
  SimpleGE::Localisation::SetContext("PLAYER_2", settings.at("alias2"));

  return true;
}

static void ShowInstructions()
{
  fmt::print("{} ({}):\n", SimpleGE::Localisation::GetContext("PLAYER_1"), SimpleGE::Localisation::Get("greenPlayer"));
  fmt::print("  {}: {}\n", SimpleGE::Localisation::Get("CHAR_KEYS"), SimpleGE::Localisation::Get("MOVE"));
  fmt::print("  {}: {}\n", SimpleGE::Localisation::Get("SPACE_KEY"), SimpleGE::Localisation::Get("ATTACK"));

  fmt::print("{} ({}):\n", SimpleGE::Localisation::GetContext("PLAYER_2"), SimpleGE::Localisation::Get("redPlayer"));
  fmt::print("  {}: {}\n", SimpleGE::Localisation::Get("ARROW_KEYS"), SimpleGE::Localisation::Get("MOVE"));
  fmt::print("  {}: {}\n", SimpleGE::Localisation::Get("ENTER_KEY"), SimpleGE::Localisation::Get("ATTACK"));
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