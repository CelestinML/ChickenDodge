#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/countdown.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, CountdownComponent::Description& desc)
  {
    j.at("sprites").get_to(desc.sprites);
    desc.spriteTemplate = j.at("spriteTemplate");
    desc.delay = std::chrono::milliseconds{j.at("delay").get<int>()};
  }

  struct CountdownImpl
  {
    std::vector<std::string> sprites;
    nlohmann::json spriteTemplate;
    std::chrono::milliseconds delay;

    int index{-1};
    std::chrono::milliseconds shownTime;
    std::shared_ptr<Entity> current;

    void UpdateLogic(CountdownComponent& comp, const Timing& timing)
    {
      shownTime += timing.delta;
      if (shownTime < delay)
      {
        return;
      }

      index++;
      if (current != nullptr)
      {
        comp.Owner().RemoveChild(current);
        current.reset();
      }

      if (index >= sprites.size())
      {
        auto* enabler = comp.Owner().GetComponent<EnablerComponent>();
        if (enabler != nullptr)
        {
          enabler->OnEvent();
        }
        comp.Disable();
      }
      else
      {
        ShowImage(comp.Owner());
      }
    }

    void ShowImage(Entity& owner)
    {
      shownTime = std::chrono::milliseconds::zero();
      ShowNamedImage(owner, gsl::at(sprites, index));
    }

    void ShowNamedImage(Entity& owner, const std::string& textureName)
    {
      spriteTemplate.at("components").at("RawSprite")["texture"] = textureName;
      current = Scene::CreateChild(spriteTemplate, "sprite", owner.shared_from_this());
    }
  };

  CountdownComponent::CountdownComponent(Entity& owner) : Component(owner), impl(std::make_unique<CountdownImpl>()) {}

  CountdownComponent::~CountdownComponent() = default;

  Component::SetupResult CountdownComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult CountdownComponent::Setup(const Description& descr)
  {
    std::transform(descr.sprites.begin(), descr.sprites.end(), std::back_inserter(impl->sprites),
                   [](const auto& s) { return Localisation::Get(s); });
    impl->delay = descr.delay;
    impl->spriteTemplate = descr.spriteTemplate;

    return {this, {}};
  }

  void CountdownComponent::UpdateLogic(const Timing& timing) { impl->UpdateLogic(*this, timing); }
} // namespace ChickenDodge
