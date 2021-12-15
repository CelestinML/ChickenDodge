#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/score.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, ScoreComponent::Description& desc)
  {
    j.at("scoreSprite").get_to(desc.scoreSprite);
  }

  struct ScoreImpl
  {
    EventTrigger<ScoreComponent::ScoreChangedEventType> scoreChangedEvent;
    int value{};
    ComponentReference<TextSpriteComponent> scoreSprite;
  };

  ScoreComponent::ScoreComponent(Entity& owner) : Component(owner), impl(std::make_unique<ScoreImpl>()) {}

  ScoreComponent::~ScoreComponent() = default;

  Component::SetupResult ScoreComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult ScoreComponent::Setup(const Description& descr)
  {
    impl->scoreSprite = descr.scoreSprite;
    return {this, {{[this]() { return impl->scoreSprite.Ready(); }, [this]() { SetValue(0); }}}};
  }

  int ScoreComponent::GetValue() const { return impl->value; }

  void ScoreComponent::SetValue(int value)
  {
    impl->value = value;
    impl->scoreChangedEvent.Trigger(value);

    impl->scoreSprite->SetText(fmt::format("{}", value));
  }

  void ScoreComponent::RegisterScoreChangedEvent(const ScoreChangedEventType& onScoreChanged)
  {
    impl->scoreChangedEvent.Register(onScoreChanged);
  }
} // namespace ChickenDodge
