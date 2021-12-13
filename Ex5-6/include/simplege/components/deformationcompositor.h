#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/compositor.h>

namespace SimpleGE
{
  struct DeformationCompositorImpl;

  class DeformationCompositorComponent : public CompositorComponent
  {
  public:
    static constexpr auto Type = "DeformationCompositor";

    struct Description : public CompositorComponent::Description
    {
      float speed;
      float scale;
      std::string source;
      std::string intensity;
    };

    DeformationCompositorComponent(Entity& owner);
    ~DeformationCompositorComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    std::shared_ptr<Graphic::RenderTexture> Compose(const std::shared_ptr<Graphic::RenderTexture>& texture) override;

  private:
    void OnEnabled() override;

    std::unique_ptr<DeformationCompositorImpl> impl;
  };
} // namespace SimpleGE