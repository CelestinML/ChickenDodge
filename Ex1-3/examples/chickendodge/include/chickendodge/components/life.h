#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct LifeImpl;

  class LifeComponent : public SimpleGE::Component
  {
  public:
    static constexpr auto Type = "Life";

    using DeadEventType = std::function<void()>;
    using HurtEventType = std::function<void()>;

    enum class HeartSprites
    {
      Empty = 0,
      Half = 1,
      Full = 2,

      Count = Full + 1
    };

    struct Description
    {
      int max{};
      float count{};
      SimpleGE::ComponentReference<SimpleGE::TextSpriteComponent> lifeSprite;
      std::array<std::string, (int) HeartSprites::Count> sprites;
    };

    LifeComponent(SimpleGE::Entity& owner);
    ~LifeComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    float GetValue() const;
    void SetValue(float value);

    void Increment(float value) { SetValue(GetValue() + value); }
    void Decrement(float value) { SetValue(GetValue() - value); }

    void RegisterDeadEvent(const DeadEventType& onDead);
    void RegisterHurtEvent(const HurtEventType& onHurt);

  private:
    std::unique_ptr<LifeImpl> impl;
  };
} // namespace ChickenDodge