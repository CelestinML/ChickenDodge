#include "chickendodge/pch/precomp.h"

#include "chickendodge/components.h"

#include <fmt/core.h>

constexpr auto TeamFile = "equipe.txt";
constexpr auto TeamDefaultString = "Coéquipiers";
constexpr auto LaunchScene = "scenes/play.json";

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

static bool ValidateTeam()
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

int main(int argc, char* argv[])
{
  if (!ValidateTeam())
  {
    return -1;
  }

  ChickenDodge::Game game({argv, (size_t) argc});
  auto runResult = game.Run();
  return runResult == SimpleGE::Game::RunResult::Success ? 0 : 1;
}