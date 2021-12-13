#pragma once

#include "chickendodge/pch/precomp.h"
#include "simplege/math.h"

namespace ChickenDodge
{
  struct ChickenImpl;

  class ChickenComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Chicken";

    struct Description
    {
      float attack;
      float heartAttackChance;
      nlohmann::json heartTemplate;
      nlohmann::json rupeeTemplate;
      SimpleGE::Point<2> target;
    };

    ChickenComponent(SimpleGE::Entity& owner);
    ~ChickenComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

    void OnAttack();
    [[nodiscard]] float GetAttack() const;

  private:
    std::unique_ptr<ChickenImpl> impl;
  };
} // namespace ChickenDodge