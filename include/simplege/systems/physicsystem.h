#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/collider.h>
#include <simplege/entity.h>
#include <simplege/systems/system.h>

#include <quadtree/Quadtree.h>

#include <functional>
#include <iostream>

namespace SimpleGE
{
  class PhysicSystem : public ISystem
  {
  public:
    static PhysicSystem& Instance()
    {
      static PhysicSystem instance;
      return instance;
    }

    PhysicSystem(const PhysicSystem&) = delete;
    PhysicSystem(PhysicSystem&&) = delete;
    PhysicSystem& operator=(const PhysicSystem&) = delete;
    PhysicSystem& operator=(PhysicSystem&&) = delete;

    void Register(gsl::not_null<ColliderComponent*> comp) { colliders.insert(comp); }

    void Unregister(gsl::not_null<ColliderComponent*> comp) { colliders.erase(comp); }

    void Iterate(const Timing& timing) override
    {
      std::vector<gsl::not_null<ColliderComponent*>> collidersVec;
      collidersVec.reserve(colliders.size());
      for (auto c : colliders)
      {
        if (c->Enabled() && c->Owner().Active())
        {
          collidersVec.push_back(c);
        }
      }

      const auto* currentCamera = CameraComponent::Current();
      Ensures(currentCamera != nullptr);

      const auto viewWidth = currentCamera->ViewWidth();
      const auto viewHeight = currentCamera->ViewHeight();

      std::size_t nCollision = 0;

      using Type = gsl::not_null<ColliderComponent*>;
      using GetBox = std::function<quadtree::Box<float>(const Type &)>;
      using Collides = std::function<bool(const Type &, const Type &)>;

      quadtree::Quadtree<Type, GetBox, Collides, 4> quadtree {
          quadtree::Box<float>(0.f, 0.f, viewWidth, viewHeight),
          [](const Type & c){ return c->GetArea(); },
          [&nCollision](const Type & c1, const Type & c2){ ++nCollision; return c1->Collides(*c2); }
        };

      for(auto c : collidersVec)
      {
        if(quadtree.getBox().contains(c->GetArea())) quadtree.add(c);
      }

      auto collisions = quadtree.findAllIntersections();

      std::cout << "Collisions évalués cette frame : " << nCollision << std::endl;

      for (auto col : collisions)
      {
        col.first->OnCollision(*col.second);
        col.second->OnCollision(*col.first);
      }
    }

  private:
    PhysicSystem() = default;

    ~PhysicSystem() override { Expects(colliders.empty()); }

    std::unordered_set<gsl::not_null<ColliderComponent*>> colliders;
  };
} // namespace SimpleGE