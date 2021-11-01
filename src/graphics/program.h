#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/program.h>

#include "graphicapi.h"
#include "shader.h"

#include <fmt/core.h>

namespace SimpleGE::Graphic
{
  struct ProgramImpl
  {
    ProgramImpl() { program = glCreateProgram(); }

    ~ProgramImpl() { glDeleteProgram(program); }

    template<ShaderType Type>
    void Attach(const std::shared_ptr<Shader<Type>>& shader)
    {
      Expects(shader != nullptr);
      glAttachShader(program, shader->GetImpl().shader);
    }

    void Link()
    {
      glLinkProgram(program);

      GLint status = 0;
      glGetProgramiv(program, GL_LINK_STATUS, &status);
      if (status != GL_TRUE)
      {
        constexpr auto ShaderMaxLogSize = 1024;
        std::array<char, ShaderMaxLogSize> buffer{};
        GLsizei len = 0;

        glGetProgramInfoLog(program, buffer.max_size(), &len, buffer.data());
        fmt::print(stderr, "Initialisation du shader échouée: {}\n", std::string_view(buffer.data(), len));
        std::terminate();
      }
    }

    GLuint program;
  };
} // namespace SimpleGE::Graphic