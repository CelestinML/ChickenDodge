#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/timer.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, TimerComponent::Description& desc) {}

  struct TimerImpl
  {
    TextSpriteComponent* textSprite{};
    std::chrono::milliseconds elapsed{};
    bool paused{};
  };

  TimerComponent::TimerComponent(Entity& owner) : Component(owner), impl(std::make_unique<TimerImpl>()) {}

  TimerComponent::~TimerComponent() = default;

  Component::SetupResult TimerComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult TimerComponent::Setup(const Description& descr)
  {
    impl->textSprite = Owner().GetComponent<TextSpriteComponent>();
    Ensures(impl->textSprite != nullptr);

    return {this, {}};
  }

  void TimerComponent::UpdateLogic(const Timing& timing)
  {
    impl->elapsed += timing.delta;

    auto minutes = fmt::format("{}", std::chrono::duration_cast<std::chrono::minutes>(impl->elapsed).count());
    auto seconds = fmt::format("{:02}", std::chrono::duration_cast<std::chrono::seconds>(impl->elapsed).count());

    auto elapsedStr = fmt::format("{}:{}", minutes, seconds);
    std::vector<std::string_view> characters;
    characters.reserve(elapsedStr.size());
    for (const auto& c : elapsedStr)
    {
      if (c == ':')
      {
        characters.emplace_back("colon");
      }
      else
      {
        characters.emplace_back(&c, 1);
      }
    }

    impl->textSprite->SetCharacters(characters);
  }

  void TimerComponent::OnEnabled() { impl->paused = false; }

  void TimerComponent::OnDisabled() { impl->paused = true; }
} // namespace ChickenDodge
