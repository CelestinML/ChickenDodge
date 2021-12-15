#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct ChickenSpawnerImpl;

  class ChickenSpawnerComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "ChickenSpawner";

    struct Description
    {
      SimpleGE::Area sourceArea;
      SimpleGE::Area targetArea;
      std::chrono::milliseconds spawnDelay;
      float spawnWaitFactor;
      nlohmann::json chickenTemplate;
    };

    ChickenSpawnerComponent(SimpleGE::Entity& owner);
    ~ChickenSpawnerComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

  private:
    std::unique_ptr<ChickenSpawnerImpl> impl;
  };
} // namespace ChickenDodge