#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

namespace SimpleGE
{
  struct EnablerImpl;

  class EnablerComponent : public Component
  {
  public:
    static constexpr auto Type = "Enabler";

    struct Description
    {
      std::vector<std::pair<ComponentReference<Component>, bool>> onStart;
      std::vector<std::pair<ComponentReference<Component>, bool>> onEvent;
    };

    EnablerComponent(Entity& owner);
    ~EnablerComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void OnEvent();

  private:
    std::unique_ptr<EnablerImpl> impl;
  };
} // namespace SimpleGE
