#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/input.h>

namespace SimpleGE
{
  struct KeyboardInputImpl;

  class KeyboardInputComponent : public InputComponent
  {
  public:
    static constexpr auto Type = "KeyboardInput";

    struct Description
    {
      std::unordered_map<Action, std::string> mapping;
    };

    KeyboardInputComponent(Entity& owner);
    ~KeyboardInputComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] bool IsActive(Action action) const override;

  private:
    std::unique_ptr<KeyboardInputImpl> impl;
  };
} // namespace SimpleGE
