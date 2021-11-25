#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/camera.h>
#include <simplege/components/visual.h>
#include <simplege/entity.h>
#include <simplege/systems/system.h>

namespace SimpleGE
{
  class DisplaySystem : public ISystem
  {
  public:
    static DisplaySystem& Instance()
    {
      static DisplaySystem instance;
      return instance;
    }

    DisplaySystem(const DisplaySystem&) = delete;
    DisplaySystem(DisplaySystem&&) = delete;
    DisplaySystem& operator=(const DisplaySystem&) = delete;
    DisplaySystem& operator=(DisplaySystem&&) = delete;

    void Register(gsl::not_null<CameraComponent*> comp) { cameras.insert(comp); }

    void Unregister(gsl::not_null<CameraComponent*> comp) { cameras.erase(comp); }

    void Register(gsl::not_null<VisualComponent*> comp) { visuals.insert(comp); }

    void Unregister(gsl::not_null<VisualComponent*> comp) { visuals.erase(comp); }

    void Iterate(const Timing& timing) override
    {
      std::vector<gsl::not_null<VisualComponent*>> sortedVisuals(visuals.begin(), visuals.end());
      std::sort(sortedVisuals.begin(), sortedVisuals.end(),
                [](gsl::not_null<VisualComponent*> a, gsl::not_null<VisualComponent*> b)
                {
                  return a->Owner().GetComponent<PositionComponent>()->WorldPosition()[2] <
                         b->Owner().GetComponent<PositionComponent>()->WorldPosition()[2];
                });

      for (const auto& c : sortedVisuals)
      {
        if (c->Enabled() && c->Owner().Active())
        {
          c->Display(timing);
        }
      }

      for (const auto& c : cameras)
      {
        if (c->Enabled() && c->Owner().Active())
        {
          c->Render(timing);
        }
      }
    }

  private:
    DisplaySystem() = default;

    ~DisplaySystem() override
    {
      Expects(cameras.empty());
      Expects(visuals.empty());
    }

    std::unordered_set<gsl::not_null<CameraComponent*>> cameras;
    std::unordered_set<gsl::not_null<VisualComponent*>> visuals;
  };

  inline VisualComponent::VisualComponent() { DisplaySystem::Instance().Register(this); }

  inline VisualComponent::~VisualComponent() { DisplaySystem::Instance().Unregister(this); }
} // namespace SimpleGE