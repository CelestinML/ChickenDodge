#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

namespace SimpleGE
{
  struct InputImpl;

  class InputComponent : public Component
  {
  public:
    static constexpr auto Type = "Input";

    enum class Action
    {
      Up,
      Down,
      Left,
      Right,
      Attack,
    };

    struct Description
    {
      std::unordered_map<Action, std::string> mapping;
    };

    InputComponent(Entity& owner);
    ~InputComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] bool IsActive(Action action) const;

  private:
    std::unique_ptr<InputImpl> impl;
  };
} // namespace SimpleGE
