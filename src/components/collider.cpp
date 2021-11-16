#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE
{
  struct ColliderImpl
  {
    int flag{};
    int mask{};
    Size<int> size{};
    ComponentReference<CollisionComponent> handler;
  };

  static void from_json(const json& j, ColliderComponent::Description& desc)
  {
    j.at("flag").get_to(desc.flag);
    j.at("mask").get_to(desc.mask);
    j.at("size").get_to(desc.size);
    if (j.contains("handler"))
    {
      j.at("handler").get_to(desc.handler);
    }
  }

  ColliderComponent::ColliderComponent(Entity& owner) : Component(owner), impl(std::make_unique<ColliderImpl>())
  {
    PhysicSystem::Instance().Register(this);
  }

  ColliderComponent::~ColliderComponent() { PhysicSystem::Instance().Unregister(this); }

  Component::SetupResult ColliderComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult ColliderComponent::Setup(const Description& descr)
  {
    impl->flag = descr.flag;
    impl->mask = descr.mask;
    impl->size = descr.size;
    impl->handler = descr.handler;

    return {this, {}};
  }

  [[nodiscard]] bool ColliderComponent::Collides(const ColliderComponent& other) const
  {
    //On vérifie la pertinence de la collision grâce aux drapeaux et aux masques
    //(nous devons vérifier dans les deux sens car les masques ne sont pas symétriques)
    if (((impl->flag & other.impl->mask) == 0) && ((impl->mask & other.impl->flag) == 0)) {
      return false;
    }
    //Si la collision est pertinente, nous vérifions si la collision pourrait réellement avoir
    //lieu grâce aux rectangles englobants.
    //Si c'est le cas, alors nous examinerons la collision plus précisément.
    return GetArea().Intersects(other.GetArea());
  }

  void ColliderComponent::OnCollision(const ColliderComponent& other) const
  {
    auto* handler = impl->handler.Resolve();
    if (handler != nullptr)
    {
      handler->OnCollision(other);
    }
  }

  [[nodiscard]] Area ColliderComponent::GetArea() const
  {
    gsl::not_null<PositionComponent*> posComp = Owner().GetComponent<PositionComponent>();
    const auto position = posComp->WorldPosition();
    return Area{position[0], position[1], float(impl->size.width), float(impl->size.height)};
  }

} // namespace SimpleGE
