#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/heart.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, HeartComponent::Description& desc)
  {
    j.at("heal").get_to(desc.heal);
    desc.lifetime = std::chrono::milliseconds{j.at("lifetime").get<int>()};
  }

  struct HeartImpl
  {
    float heal{};
    std::chrono::milliseconds remaining{};
  };

  HeartComponent::HeartComponent(Entity& owner) : Component(owner), impl(std::make_unique<HeartImpl>()) {}

  HeartComponent::~HeartComponent() = default;

  Component::SetupResult HeartComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult HeartComponent::Setup(const Description& descr)
  {
    impl->heal = descr.heal;
    impl->remaining = descr.lifetime;

    return {this, {}};
  }

  void HeartComponent::UpdateLogic(const Timing& timing)
  {
    using namespace std::chrono_literals;

    impl->remaining -= timing.delta;
    if (impl->remaining <= 0ms)
    {
      auto& owner = Owner();
      owner.GetParent()->RemoveChild(owner);
    }
  }

  [[nodiscard]] float HeartComponent::GetHeal() const { return impl->heal; }

} // namespace ChickenDodge
