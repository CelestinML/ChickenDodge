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

      //We get the game window's dimensions
      const auto* currentCamera = CameraComponent::Current();
      Ensures(currentCamera != nullptr);

      const auto viewWidth = currentCamera->ViewWidth();
      const auto viewHeight = currentCamera->ViewHeight();

      std::size_t nCollision = 0;

      //We create a quad tree, specifying all the required functions (GetArea, Collides...)
      using Type = gsl::not_null<ColliderComponent*>;
      using GetBox = std::function<quadtree::Box<float>(const Type &)>;
      using Collides = std::function<bool(const Type &, const Type &)>;

      quadtree::Quadtree<Type, GetBox, Collides, 4> quadtree {
          quadtree::Box<float>(0.f, 0.f, viewWidth, viewHeight),
          [](const Type & c){ return c->GetArea(); },
          [&nCollision](const Type & c1, const Type & c2){ ++nCollision; return c1->Collides(*c2); }
        };

      //We add all our colliders to the quadtree
      //Once there are more than 4 objects in a quad, a smaller quad will be created
      for(auto c : collidersVec)
      {
        if(quadtree.getBox().contains(c->GetArea())) quadtree.add(c);
      }

      //We iterate through the quadtree to find intersections, using the bounding boxes colliders
      auto collisions = quadtree.findAllIntersections();

      //We calculate the mean number of collisions and print it every 5 seconds
      using namespace std::chrono_literals;
      constexpr auto printThreshold = 5s;

      static std::size_t nCollisionTot = 0;
      static auto deltaTot = 0ms;
      static unsigned int frameTot = 0;
      nCollisionTot += nCollision;
      deltaTot += timing.delta;
      ++frameTot;

      if(deltaTot > printThreshold)
      {
        std::cout << "Nombre moyen de Collisions evaluees par frame : " << static_cast<float>(nCollisionTot) / static_cast<float>(frameTot) << std::endl;
        nCollisionTot = 0;
        deltaTot = 0ms;
        frameTot = 0;
      }

      //We start the collision callbacks for both object
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