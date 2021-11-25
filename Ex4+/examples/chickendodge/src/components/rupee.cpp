#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/rupee.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, RupeeComponent::Description& desc)
  {
    j.at("values").get_to(desc.values);
    desc.lifetime = std::chrono::milliseconds{j.at("lifetime").get<int>()};
  }

  struct RupeeImpl
  {
    std::string type{};
    int value{};
    std::chrono::milliseconds remaining{};
  };

  RupeeComponent::RupeeComponent(Entity& owner) : Component(owner), impl(std::make_unique<RupeeImpl>()) {}

  RupeeComponent::~RupeeComponent() = default;

  Component::SetupResult RupeeComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult RupeeComponent::Setup(const Description& descr)
  {
    int randVal = std::rand() % descr.values.size();
    const auto randomType = std::next(descr.values.begin(), randVal);
    impl->type = randomType->first;
    impl->value = randomType->second;
    impl->remaining = descr.lifetime;

    return {this,
            {{[this]() { return Owner().GetComponent<SpriteComponent>()->Ready(); },
              [this]()
              {
                gsl::not_null<SpriteComponent*> sprite = Owner().GetComponent<SpriteComponent>();
                sprite->SetSpriteName(impl->type);
                sprite->SetAnimationFrame(1);
                sprite->UpdateMesh();
              }}}};
  }

  void RupeeComponent::UpdateLogic(const Timing& timing)
  {
    using namespace std::chrono_literals;

    impl->remaining -= timing.delta;
    if (impl->remaining <= 0ms)
    {
      auto& owner = Owner();
      owner.GetParent()->RemoveChild(owner);
    }
  }

  int RupeeComponent::GetValue() const { return impl->value; }
} // namespace ChickenDodge
