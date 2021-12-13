#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class LifeComponent;
  class ScoreComponent;

  struct PlayerImpl;

  class PlayerComponent : public SimpleGE::LogicComponent, public SimpleGE::CollisionComponent
  {
  public:
    static constexpr auto Type = "Player";

    using DeadEventType = std::function<void()>;

    struct Description
    {
      SimpleGE::ComponentReference<SimpleGE::InputComponent> input;
      std::string prefix;
      SimpleGE::ComponentReference<ScoreComponent> score;
      SimpleGE::ComponentReference<LifeComponent> life;
      SimpleGE::Area gameArea;
      std::chrono::milliseconds invulnerableDuration;
      std::chrono::milliseconds hurtDuration;
      float hurtMotion;
      float speed;
      std::vector<SimpleGE::ComponentReference<SimpleGE::Component>> onHurtEnable;
    };

    PlayerComponent(SimpleGE::Entity& owner);
    ~PlayerComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;
    void OnCollision(const SimpleGE::ColliderComponent& other) override;

    void SetLocal(bool isLocal);
    void SetName(std::string_view name);
    [[nodiscard]] std::string_view GetName() const;
    [[nodiscard]] bool IsDead() const;
    [[nodiscard]] const SimpleGE::ComponentReference<ScoreComponent>& GetScore() const;

    void RegisterDeadEvent(const DeadEventType& onDead);

  private:
    std::unique_ptr<PlayerImpl> impl;
  };
} // namespace ChickenDodge