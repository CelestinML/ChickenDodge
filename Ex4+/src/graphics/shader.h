#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/shader.h>

#include "graphicapi.h"

#include <fmt/core.h>

namespace SimpleGE::Graphic
{
  template<ShaderType Type>
  struct ShaderImplTraits
  {
  };

  template<>
  struct ShaderImplTraits<ShaderType::Vertex>
  {
    static constexpr auto GLShaderType = GL_VERTEX_SHADER;
  };

  template<>
  struct ShaderImplTraits<ShaderType::Fragment>
  {
    static constexpr auto GLShaderType = GL_FRAGMENT_SHADER;
  };

  template<ShaderType Type>
  struct ShaderImpl
  {
    using ImplTraits = ShaderImplTraits<Type>;
    static constexpr auto GLShaderType = ImplTraits::GLShaderType;

    ShaderImpl(std::string_view source)
    {
      shader = glCreateShader(GLShaderType);
      const char* sourceStr = source.data();
      glShaderSource(shader, 1, &sourceStr, nullptr);
      glCompileShader(shader);

      GLint status = 0;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
      if (status != GL_TRUE)
      {
        constexpr auto ShaderMaxLogSize = 1024;
        std::array<char, ShaderMaxLogSize> buffer{};
        GLsizei len = 0;

        glGetShaderInfoLog(shader, buffer.max_size(), &len, buffer.data());
        fmt::print(stderr, "Erreur en compilant le shader: {}\n", std::string_view(buffer.data(), len));
        std::terminate();
      }
    }

    ~ShaderImpl() { glDeleteShader(shader); }

    GLuint shader;
  };
} // namespace SimpleGE::Graphic