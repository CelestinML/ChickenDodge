#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/chicken.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace
{
  int g_nbChicken = 0;
}

namespace ChickenDodge
{
  static void from_json(const json& j, ChickenComponent::Description& desc)
  {
    j.at("attack").get_to(desc.attack);
    j.at("heartAttackChance").get_to(desc.heartAttackChance);
    desc.heartTemplate = j.at("heartTemplate");
    desc.rupeeTemplate = j.at("rupeeTemplate");
    j.at("target").get_to(desc.target);
  }

  struct ChickenImpl
  {
    float attack{};
    bool dropped{};
    float distance{};
    Point<2> target;
    float heartAttackChance{};
    nlohmann::json heartTemplate;
    nlohmann::json rupeeTemplate;
    Vector<2> velocity;

    void Drop(Entity& owner, nlohmann::json dropTemplate)
    {
      static int dropId{};
      dropId++;

      gsl::not_null<PositionComponent*> position = owner.GetComponent<PositionComponent>();
      auto localPos = position->LocalPosition();

      auto& posTpl = dropTemplate.at("components").at("Position");
      posTpl["x"] = localPos[0];
      posTpl["y"] = localPos[1];
      posTpl["z"] = localPos[2];

      Scene::CreateChild(dropTemplate, fmt::format("{}", dropId), owner.GetParent());
      dropped = true;
    }
  };

  ChickenComponent::ChickenComponent(Entity& owner) : Component(owner), impl(std::make_unique<ChickenImpl>()) {}

  ChickenComponent::~ChickenComponent() = default;

  Component::SetupResult ChickenComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult ChickenComponent::Setup(const Description& descr)
  {
    impl->target = descr.target;
    impl->rupeeTemplate = descr.rupeeTemplate;
    impl->heartAttackChance = descr.heartAttackChance;
    impl->heartTemplate = descr.heartTemplate;
    impl->attack = descr.attack;

    AudioSystem::SetProperty("NbChicken", ++g_nbChicken);

    return {this,
            {{[this]() { return Owner().GetComponent<PositionComponent>()->Ready(); },
              [this]()
              {
                gsl::not_null<PositionComponent*> position = Owner().GetComponent<PositionComponent>();
                impl->velocity = impl->target - Point<2>(position->LocalPosition());
                impl->velocity.Normalize();
                impl->velocity *= float(std::rand()) / float(RAND_MAX) * 45.F + 30.F;
              }},
             {[this]() { return Owner().GetComponent<SpriteComponent>()->Ready(); },
              [this]()
              {
                using namespace std::literals::string_view_literals;
                gsl::not_null<SpriteComponent*> sprite = Owner().GetComponent<SpriteComponent>();
                std::string_view dir = (impl->velocity[0] > 0.F) ? "R"sv : "L"sv;
                sprite->SetSpriteName(fmt::format("C{}", dir));
              }}}};
  }

  void ChickenComponent::UpdateLogic(const Timing& timing)
  {
    gsl::not_null<PositionComponent*> position = Owner().GetComponent<PositionComponent>();
    auto targetDistanceSq = (impl->target - Point<2>(position->LocalPosition())).MagnitudeSq();

    auto delta = impl->velocity * std::chrono::duration_cast<std::chrono::duration<float>>(timing.delta).count();
    position->Translate(Vector<3>(delta));

    auto newTargetDistanceSq = (impl->target - Point<2>(position->LocalPosition())).MagnitudeSq();
    if ((!impl->dropped) && (newTargetDistanceSq > targetDistanceSq))
    {
      impl->Drop(Owner(), impl->rupeeTemplate);
    }

    impl->distance += delta.Magnitude();
    if (impl->distance > 500.F)
    {
      Owner().GetParent()->RemoveChild(Owner());
      AudioSystem::SetProperty("NbChicken", --g_nbChicken);
    }
  }

  void ChickenComponent::OnAttack()
  {
    auto& toDrop =
        (float(std::rand()) / float(RAND_MAX) < impl->heartAttackChance) ? impl->heartTemplate : impl->rupeeTemplate;
    impl->Drop(Owner(), toDrop);

    gsl::not_null<ColliderComponent*> collider = Owner().GetComponent<ColliderComponent>();
    collider->Disable();
    impl->velocity[0] *= -1.F;

    using namespace std::literals::string_view_literals;
    gsl::not_null<SpriteComponent*> sprite = Owner().GetComponent<SpriteComponent>();
    std::string_view dir = (impl->velocity[0] > 0.F) ? "R"sv : "L"sv;
    sprite->SetSpriteName(fmt::format("C{}", dir));
  }

  [[nodiscard]] float ChickenComponent::GetAttack() const { return impl->attack; }
} // namespace ChickenDodge
