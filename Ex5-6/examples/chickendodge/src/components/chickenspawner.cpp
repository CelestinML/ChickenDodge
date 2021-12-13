#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/chickenspawner.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, ChickenSpawnerComponent::Description& desc)
  {
    j.at("sourceArea").get_to(desc.sourceArea);
    j.at("targetArea").get_to(desc.targetArea);
    desc.spawnDelay = std::chrono::milliseconds{j.at("spawnDelay").get<int>()};
    j.at("spawnWaitFactor").get_to(desc.spawnWaitFactor);
    desc.chickenTemplate = j.at("chickenTemplate");
  }

  struct ChickenSpawnerImpl
  {
    Area sourceArea;
    Area targetArea;
    std::chrono::milliseconds spawnDelay{};
    float spawnWaitFactor{};
    nlohmann::json chickenTemplate;
    std::chrono::milliseconds elapsed{};

    void UpdateLogic(Entity& owner, const Timing& timing)
    {
      using namespace std::chrono_literals;

      elapsed += timing.delta;
      if (elapsed >= spawnDelay)
      {
        float nextSpawnInMS = std::max(150.F, spawnDelay.count() * spawnWaitFactor);
        spawnDelay = std::chrono::milliseconds((int) nextSpawnInMS);
        elapsed = 0ms;
        Spawn(owner, timing);
      }
    }

    void Spawn(Entity& owner, const Timing& timing)
    {
      float x, y;
      if (std::rand() % 2 == 0)
      {
        x = sourceArea.xMin();
        if (std::rand() % 2 == 0)
        {
          x += sourceArea.width();
        }
        y = (std::rand() / float(RAND_MAX)) * sourceArea.height() + sourceArea.yMin();
      }
      else
      {
        y = sourceArea.yMin();
        if (std::rand() % 2 == 0)
        {
          y += sourceArea.height();
        }
        x = (std::rand() / float(RAND_MAX)) * sourceArea.width() + sourceArea.xMin();
      }

      auto& chickenTgt = chickenTemplate.at("components").at("Chicken").at("target");
      chickenTgt["x"] = (std::rand() / float(RAND_MAX)) * targetArea.width() + targetArea.xMin();
      chickenTgt["y"] = (std::rand() / float(RAND_MAX)) * targetArea.height() + targetArea.yMin();

      auto& pos = chickenTemplate.at("components").at("Position");
      pos["x"] = x;
      pos["y"] = y;
      pos["z"] = 0;

      Scene::CreateChild(chickenTemplate, fmt::format("Chicken-{}", timing.frame), owner.shared_from_this());
    }
  };

  ChickenSpawnerComponent::ChickenSpawnerComponent(Entity& owner)
      : Component(owner), impl(std::make_unique<ChickenSpawnerImpl>())
  {
    AudioSystem::SendEvent("chicken_idle");
    AudioSystem::SetProperty("NbChicken", 0);
  }

  ChickenSpawnerComponent::~ChickenSpawnerComponent() = default;

  Component::SetupResult ChickenSpawnerComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult ChickenSpawnerComponent::Setup(const Description& descr)
  {
    impl->sourceArea = descr.sourceArea;
    impl->targetArea = descr.targetArea;
    impl->spawnDelay = descr.spawnDelay;
    impl->spawnWaitFactor = descr.spawnWaitFactor;
    impl->chickenTemplate = descr.chickenTemplate;

    return {this, {}};
  }

  void ChickenSpawnerComponent::UpdateLogic(const Timing& timing) { impl->UpdateLogic(Owner(), timing); }
} // namespace ChickenDodge
