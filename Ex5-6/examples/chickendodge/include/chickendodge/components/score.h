#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct ScoreImpl;

  class ScoreComponent : public SimpleGE::Component
  {
  public:
    static constexpr auto Type = "Score";

    using ScoreChangedEventType = std::function<void(int)>;

    struct Description
    {
      SimpleGE::ComponentReference<SimpleGE::TextSpriteComponent> scoreSprite;
    };

    ScoreComponent(SimpleGE::Entity& owner);
    ~ScoreComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    int GetValue() const;
    void SetValue(int value);

    void Increment(int value) { SetValue(GetValue() + value); }

    void Decrement(int value) { SetValue(GetValue() - value); }

    void RegisterScoreChangedEvent(const ScoreChangedEventType& onScoreChanged);

  private:
    std::unique_ptr<ScoreImpl> impl;
  };
} // namespace ChickenDodge