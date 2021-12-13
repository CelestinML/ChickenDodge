#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/texture.h>

namespace SimpleGE::Graphic
{
  struct RenderTextureImpl;

  class RenderTexture
  {
  public:
    struct Description
    {
      int width;
      int height;
      Texture::Filter minFilter;
      Texture::Filter magFilter;
      Texture::Wrap horizWrap;
      Texture::Wrap vertWrap;
    };

    static std::shared_ptr<RenderTexture> Create(const Description& descr)
    {
      return std::make_shared<RenderTexture>(descr);
    }

    explicit RenderTexture(const Description& descr);
    ~RenderTexture();

    operator unsigned int() const;

    void BindFramebuffer();
    void UnbindFramebuffer();

  private:
    std::unique_ptr<RenderTextureImpl> impl;
  };

  struct RenderTextureScope
  {
  public:
    RenderTextureScope(const std::shared_ptr<RenderTexture>& target) : target(target) { target->BindFramebuffer(); }

    ~RenderTextureScope() { target->UnbindFramebuffer(); }

  private:
    const std::shared_ptr<RenderTexture>& target;
  };
} // namespace SimpleGE::Graphic