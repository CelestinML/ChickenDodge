#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/texture.h>
#include <simplege/components/visual.h>

namespace SimpleGE
{
  struct RawSpriteImpl;

  class RawSpriteComponent : public VisualComponent, public TextureComponent
  {
  public:
    static constexpr auto Type = "RawSprite";

    struct Description : public TextureComponent::Description
    {
      float width{};
      float height{};
      float scale{1.F};
    };

    RawSpriteComponent(Entity& owner);
    ~RawSpriteComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void Display(const Timing& timing) override;

  private:
    std::unique_ptr<RawSpriteImpl> impl;
  };
} // namespace SimpleGE
