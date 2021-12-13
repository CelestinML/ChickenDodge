#include <simplege/simplege.h>

#include "rendertexture.h"

namespace SimpleGE::Graphic
{
  RenderTexture::RenderTexture(const Description& descr) : impl(std::make_unique<RenderTextureImpl>(descr)) {}

  RenderTexture::~RenderTexture() = default;

  RenderTexture::operator unsigned int() const { return impl->texture; }

  void RenderTexture::BindFramebuffer() { impl->BindFramebuffer(); }

  void RenderTexture::UnbindFramebuffer() { impl->UnbindFramebuffer(); }
} // namespace SimpleGE::Graphic