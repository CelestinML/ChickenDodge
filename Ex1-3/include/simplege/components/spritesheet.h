#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/texture.h>

#include <simplege/math.h>

namespace SimpleGE
{
  struct SpriteSheetImpl;

  class SpriteSheetComponent : public TextureComponent
  {
  public:
    static constexpr auto Type = "SpriteSheet";

    struct Frame
    {
      Area frame;
      Size<int> sourceSize;
      Area uv;
    };

    struct Meta
    {
      Size<int> size;
    };

    struct Description : public TextureComponent::Description
    {
      std::string description;
    };

    SpriteSheetComponent(Entity& owner);
    ~SpriteSheetComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] const Frame* GetSprite(std::string_view key) const;

  private:
    std::unique_ptr<SpriteSheetImpl> impl;
  };
} // namespace SimpleGE
