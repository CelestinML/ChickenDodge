#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/referee.h"

#include "chickendodge/components/player.h"
#include "chickendodge/components/score.h"

#include <fmt/core.h>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, RefereeComponent::Description& desc) { j.at("players").get_to(desc.players); }

  struct RefereeImpl
  {
    EventTrigger<RefereeComponent::WinEventType> winEvent;
    std::array<ComponentReference<PlayerComponent>, 2> players;

    Component::SetupResult Setup(RefereeComponent& comp, const RefereeComponent::Description& descr)
    {
      winEvent.Register([](auto& winner, auto& loser)
                        { fmt::print(stderr, "Victoire de {} contre {}!\n", winner.GetName(), loser.GetName()); });

      return {&comp,
              {{[descr]() {
                  return std::all_of(descr.players.begin(), descr.players.end(),
                                     [](const auto& player) { return player.Ready(); });
                },
                [this, descr]()
                {
                  std::transform(descr.players.begin(), descr.players.end(), players.begin(),
                                 [this](const auto& player)
                                 {
                                   player->RegisterDeadEvent([this]() { OnDead(); });
                                   return player;
                                 });
                }}}};
    }

    void OnDead()
    {
      int bestScore{-1};
      PlayerComponent* bestPlayer{};
      int worstScore{std::numeric_limits<int>::max()};
      PlayerComponent* worstPlayer{};

      bool gameOver{true};

      for (const auto& p : players)
      {
        if (!gameOver)
        {
          continue;
        }
        if (!p->IsDead())
        {
          gameOver = false;
          continue;
        }

        int score = p->GetScore()->GetValue();
        if (score > bestScore)
        {
          bestScore = score;
          bestPlayer = p.Resolve();
        }
        if (score < worstScore)
        {
          worstScore = score;
          worstPlayer = p.Resolve();
        }
      }

      if (gameOver)
      {
        Ensures(bestPlayer != nullptr);
        Ensures(worstPlayer != nullptr);
        winEvent.Trigger(*bestPlayer, *worstPlayer);
      }
    }
  };

  RefereeComponent::RefereeComponent(Entity& owner) : Component(owner), impl(std::make_unique<RefereeImpl>()) {}

  RefereeComponent::~RefereeComponent() = default;

  Component::SetupResult RefereeComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult RefereeComponent::Setup(const Description& descr) { return impl->Setup(*this, descr); }

  void RefereeComponent::RegisterWinEvent(const WinEventType& onWin) { impl->winEvent.Register(onWin); }

} // namespace ChickenDodge
