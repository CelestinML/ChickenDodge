#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/buffer.h>

#include "graphicapi.h"

namespace SimpleGE::Graphic
{
  template<BufferType Type>
  struct BufferImplTraits
  {
  };

  template<>
  struct BufferImplTraits<BufferType::Vertices>
  {
    static constexpr auto GLBufferType = GL_ARRAY_BUFFER;
  };

  template<>
  struct BufferImplTraits<BufferType::Indices>
  {
    static constexpr auto GLBufferType = GL_ELEMENT_ARRAY_BUFFER;
  };

  template<BufferType Type>
  struct BufferImpl
  {
    using ImplTraits = BufferImplTraits<Type>;
    static constexpr auto GLBufferType = ImplTraits::GLBufferType;

    BufferImpl(typename Buffer<Type>::Usage usage) : usage(usage) { glCreateBuffers(1, &buffer); }

    ~BufferImpl() { glDeleteBuffers(1, &buffer); }

    void Set(gsl::span<const char> data)
    {
      glBindBuffer(GLBufferType, buffer);
      glBufferData(GLBufferType, data.size_bytes(), data.data(),
                   usage == Buffer<Type>::Usage::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    }

    void Update(gsl::span<const char> data, std::ptrdiff_t offset)
    {
      glBindBuffer(GLBufferType, buffer);
      glBufferSubData(GLBufferType, offset, data.size_bytes(), data.data());
    }

    GLuint buffer;
    typename Buffer<Type>::Usage usage;
  };
} // namespace SimpleGE::Graphic