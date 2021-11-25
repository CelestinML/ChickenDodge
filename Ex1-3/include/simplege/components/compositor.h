#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/graphics/rendertexture.h>

namespace SimpleGE
{
  struct CompositorImpl;

  class CompositorComponent : public virtual Component
  {
  public:
    struct Description
    {
      std::string vertexShader;
      std::string fragmentShader;
    };

    CompositorComponent(Entity& owner);
    ~CompositorComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& desc);

    virtual std::shared_ptr<Graphic::RenderTexture> Compose(const std::shared_ptr<Graphic::RenderTexture>& texture) = 0;

  protected:
    void Bind();
    void Unbind();

    CompositorImpl& GetCompositorImpl() { return *impl; }

  private:
    std::unique_ptr<CompositorImpl> impl;
  };
} // namespace SimpleGE
