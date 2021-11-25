#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

namespace SimpleGE
{
  class SpriteSheetComponent;

  struct TextSpriteImpl;

  class TextSpriteComponent : public Component
  {
  public:
    static constexpr auto Type = "TextSprite";

    enum class Align
    {
      Left,
      Right,
    };

    struct Description
    {
      ComponentReference<SpriteSheetComponent> spriteSheet;
      Align align{};
    };

    TextSpriteComponent(Entity& owner);
    ~TextSpriteComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void SetText(std::string_view text);
    void SetCharacters(gsl::span<std::string_view> array);

  private:
    std::unique_ptr<TextSpriteImpl> impl;
  };
} // namespace SimpleGE
