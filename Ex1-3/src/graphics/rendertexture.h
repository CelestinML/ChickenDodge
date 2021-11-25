#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/rendertexture.h>

#include "graphicapi.h"

namespace SimpleGE::Graphic
{
  struct RenderTextureImpl
  {
    RenderTextureImpl(const RenderTexture::Description& descr)
    {
      glCreateTextures(GL_TEXTURE_2D, 1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, descr.width, descr.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                      descr.magFilter == Texture::Filter::Nearest ? GL_NEAREST : GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      descr.minFilter == Texture::Filter::Nearest ? GL_NEAREST : GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                      descr.horizWrap == Texture::Wrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                      descr.vertWrap == Texture::Wrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
      glBindTexture(GL_TEXTURE_2D, 0);

      glCreateFramebuffers(1, &rttFrameBuffer);
      glCreateRenderbuffers(1, &renderBuffer);

      BindFramebuffer();
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, descr.width, descr.height);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
      UnbindFramebuffer();
    }

    ~RenderTextureImpl()
    {
      glDeleteRenderbuffers(1, &renderBuffer);
      glDeleteFramebuffers(1, &rttFrameBuffer);
      glDeleteTextures(1, &texture);
    }

    void BindFramebuffer()
    {
      glBindFramebuffer(GL_FRAMEBUFFER, rttFrameBuffer);
      glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    }

    void UnbindFramebuffer()
    {
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint texture;
    GLuint rttFrameBuffer;
    GLuint renderBuffer;
  };
} // namespace SimpleGE::Graphic