#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/texture.h>

#include "graphicapi.h"

namespace SimpleGE::Graphic
{
  struct TextureImpl
  {
    TextureImpl(const Texture::Description& descr)
    {
      glCreateTextures(GL_TEXTURE_2D, 1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei) descr.image->Width(), (GLsizei) descr.image->Height(), 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, descr.image->Pixels().data());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                      descr.magFilter == Texture::Filter::Nearest ? GL_NEAREST : GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      descr.minFilter == Texture::Filter::Nearest ? GL_NEAREST : GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                      descr.horizWrap == Texture::Wrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                      descr.vertWrap == Texture::Wrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~TextureImpl() { glDeleteTextures(1, &texture); }

    GLuint texture;
  };
} // namespace SimpleGE::Graphic