#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/compositor.h>

namespace SimpleGE
{
  struct RenderCompositorImpl;

  class RenderCompositorComponent : public CompositorComponent
  {
  public:
    static constexpr auto Type = "RenderCompositor";

    using Description = CompositorComponent::Description;

    RenderCompositorComponent(Entity& owner);
    ~RenderCompositorComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    std::shared_ptr<Graphic::RenderTexture> Compose(const std::shared_ptr<Graphic::RenderTexture>& texture) override;

  private:
    std::unique_ptr<RenderCompositorImpl> impl;
  };
} // namespace SimpleGE
