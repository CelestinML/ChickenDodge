#include <simplege/simplege.h>

#include "texture.h"

namespace SimpleGE::Graphic
{
  Texture::Texture(const Description& descr) : impl(std::make_unique<TextureImpl>(descr)) {}

  Texture::~Texture() = default;

  Texture::operator unsigned int() const { return impl->texture; }
} // namespace SimpleGE::Graphic