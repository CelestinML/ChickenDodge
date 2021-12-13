#pragma once

#include <simplege/pch/precomp.h>

#include "shader.h"

namespace SimpleGE::Graphic
{
  struct ProgramImpl;

  class Program
  {
  public:
    template<ShaderType... Types>
    static std::shared_ptr<Program> Create(const std::shared_ptr<Shader<Types>>&... shaders)
    {
      return std::make_shared<Program>(shaders...);
    }

    template<ShaderType... Types>
    Program(const std::shared_ptr<Shader<Types>>&... shaders) : Program()
    {
      (Attach(shaders), ...);
      Link();
    }

    ~Program();

    operator unsigned int() const;

  private:
    Program();

    template<ShaderType Type>
    void Attach(const std::shared_ptr<Shader<Type>>& shader);

    void Link();

    static void DefineSymbols();

    std::unique_ptr<ProgramImpl> impl;
  };
} // namespace SimpleGE::Graphic