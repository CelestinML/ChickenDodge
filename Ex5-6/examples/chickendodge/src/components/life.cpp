#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/life.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, LifeComponent::Description& desc)
  {
    j.at("max").get_to(desc.max);
    desc.count = float(desc.max);
    if (j.contains("default"))
    {
      j.at("default").get_to(desc.count);
    }
    j.at("lifeSprite").get_to(desc.lifeSprite);
    j.at("sprites").get_to(desc.sprites);
  }

  struct LifeImpl
  {
    EventTrigger<LifeComponent::DeadEventType> deadEvent;
    EventTrigger<LifeComponent::HurtEventType> hurtEvent;
    int max{};
    float value{};
    decltype(LifeComponent::Description::sprites) sprites;
    ComponentReference<TextSpriteComponent> lifeSprite;
  };

  LifeComponent::LifeComponent(Entity& owner) : Component(owner), impl(std::make_unique<LifeImpl>()) {}

  LifeComponent::~LifeComponent() = default;

  Component::SetupResult LifeComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult LifeComponent::Setup(const Description& descr)
  {
    impl->max = descr.max;
    impl->sprites = descr.sprites;
    impl->lifeSprite = descr.lifeSprite;

    return {this, {{[this]() { return impl->lifeSprite.Ready(); }, [this, descr]() { SetValue(descr.count); }}}};
  }

  float LifeComponent::GetValue() const { return impl->value; }

  void LifeComponent::SetValue(float value)
  {
    if (value < 0.F)
    {
      value = 0.F;
    }
    if (value > float(impl->max))
    {
      value = float(impl->max);
    }

    if (value == 0.F)
    {
      impl->deadEvent.Trigger();
    }
    else if (value < impl->value)
    {
      impl->hurtEvent.Trigger();
    }

    impl->value = value;
    std::vector<std::string_view> hearts;
    hearts.reserve(impl->max);
    for (int i = 0; i < impl->max; i++)
    {
      HeartSprites sIndex = HeartSprites::Empty;
      if (float(i) < impl->value)
      {
        sIndex = HeartSprites::Half;
      }
      if (float(i + 1) <= impl->value)
      {
        sIndex = HeartSprites::Full;
      }
      hearts.push_back(impl->sprites[(int) sIndex]);
    }

    impl->lifeSprite->SetCharacters(hearts);
  }

  void LifeComponent::RegisterDeadEvent(const DeadEventType& onDead) { impl->deadEvent.Register(onDead); }

  void LifeComponent::RegisterHurtEvent(const HurtEventType& onHurt) { impl->hurtEvent.Register(onHurt); }
} // namespace ChickenDodge
