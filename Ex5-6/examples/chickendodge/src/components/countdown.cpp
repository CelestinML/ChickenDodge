#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/countdown.h"

#include "chickendodge/components/networkplayermanager.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, CountdownComponent::Description& desc)
  {
    j.at("sprites").get_to(desc.sprites);
    j.at("waitSprite").get_to(desc.waitSprite);
    j.at("playerSpritePrefix").get_to(desc.playerSpritePrefix);
    desc.spriteTemplate = j.at("spriteTemplate");
    desc.delay = std::chrono::milliseconds{j.at("delay").get<int>()};
    j.at("playerWait").get_to(desc.playerWait);
  }

  struct CountdownImpl
  {
    std::vector<std::string> sprites;
    std::string waitSprite;
    std::string playerSpritePrefix;
    nlohmann::json spriteTemplate;
    std::chrono::milliseconds delay;

    int index{-1};
    std::chrono::milliseconds shownTime;
    std::shared_ptr<Entity> current;

    Component::SetupResult Setup(CountdownComponent& comp, const CountdownComponent::Description& descr)
    {
      std::transform(descr.sprites.begin(), descr.sprites.end(), std::back_inserter(sprites),
                     [](const auto& s) { return Localisation::Get(s); });
      waitSprite = Localisation::Get(descr.waitSprite);
      playerSpritePrefix = descr.playerSpritePrefix;
      delay = descr.delay;
      spriteTemplate = descr.spriteTemplate;

      return {&comp,
              {{[descr]() { return descr.playerWait.Ready(); },
                [this, descr, &comp]()
                {
                  auto& owner = comp.Owner();
                  ShowNamedImage(owner, waitSprite);
                  comp.Disable();
                  descr.playerWait->RegisterPlayerReadyEvent([this, &comp](int localIndex)
                                                             { OnPlayerReady(comp, localIndex); });
                }}}};
    }

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

      AudioSystem::SendEvent("countdown");
    }

    void ShowNamedImage(Entity& owner, const std::string& textureName)
    {
      spriteTemplate.at("components").at("RawSprite")["texture"] = textureName;
      current = Scene::CreateChild(spriteTemplate, "sprite", owner.shared_from_this());
    }

    void OnPlayerReady(CountdownComponent& comp, int localIndex)
    {
      auto sprite = Localisation::Get(fmt::format("{}{}", playerSpritePrefix, localIndex));
      sprites.insert(sprites.begin(), sprite);
      comp.Enable();
    }
  };

  CountdownComponent::CountdownComponent(Entity& owner) : Component(owner), impl(std::make_unique<CountdownImpl>()) {}

  CountdownComponent::~CountdownComponent() = default;

  Component::SetupResult CountdownComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult CountdownComponent::Setup(const Description& descr) { return impl->Setup(*this, descr); }

  void CountdownComponent::UpdateLogic(const Timing& timing) { impl->UpdateLogic(*this, timing); }
} // namespace ChickenDodge
