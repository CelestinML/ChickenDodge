#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/image.h>

namespace SimpleGE::Graphic
{
  struct TextureImpl;

  class Texture
  {
  public:
    enum class Filter
    {
      Nearest,
      Linear,
    };

    enum class Wrap
    {
      Clamp,
      Wrap,
    };

    struct Description
    {
      const std::shared_ptr<Image>& image;
      Filter minFilter;
      Filter magFilter;
      Wrap horizWrap;
      Wrap vertWrap;
    };

    static std::shared_ptr<Texture> Create(const Description& descr) { return std::make_shared<Texture>(descr); }

    explicit Texture(const Description& descr);
    ~Texture();

    operator unsigned int() const;

  private:
    std::unique_ptr<TextureImpl> impl;
  };
} // namespace SimpleGE::Graphic