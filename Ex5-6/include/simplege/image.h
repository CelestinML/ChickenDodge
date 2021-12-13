#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  union PixelRGBA8
  {
    unsigned int pixel;
    struct
    {
      std::uint8_t r;
      std::uint8_t g;
      std::uint8_t b;
      std::uint8_t a;
    };
  };

  class Image
  {
  public:
    static std::shared_ptr<Image> Create(const std::vector<std::byte>& content)
    {
      return std::make_shared<Image>(content);
    }

    explicit Image(const std::vector<std::byte>& content);

    [[nodiscard]] std::uint32_t Width() const { return width; }
    [[nodiscard]] std::uint32_t Height() const { return height; }
    [[nodiscard]] gsl::span<const PixelRGBA8> Pixels() const { return m_Content; }
    operator gsl::span<const PixelRGBA8>() const { return Pixels(); }

  private:
    std::uint32_t width{};
    std::uint32_t height{};
    std::vector<PixelRGBA8> m_Content;
  };
} // namespace SimpleGE