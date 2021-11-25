#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

#include <simplege/graphics/buffer.h>

namespace SimpleGE
{
  class Image;
  struct TextureComponentImpl;

  class TextureComponent : public virtual Component
  {
  public:
    struct Description
    {
      std::string texture;
      std::string vertexShader;
      std::string fragmentShader;
    };

    TextureComponent(Entity& owner);
    ~TextureComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void Bind(const std::shared_ptr<Graphic::VertexBuffer>& vertexBuffer,
              const std::shared_ptr<Graphic::IndexBuffer>& indexBuffer);
    void Unbind();

    [[nodiscard]] std::shared_ptr<Image> GetTexture() const;

  private:
    std::unique_ptr<TextureComponentImpl> impl;
  };
} // namespace SimpleGE
