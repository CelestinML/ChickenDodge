#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE::Graphic
{
  enum class ShaderType
  {
    Vertex,
    Fragment,
  };

  template<ShaderType Type>
  struct ShaderImpl;

  template<ShaderType Type>
  class Shader
  {
  public:
    static std::shared_ptr<Shader<Type>> Create(std::string_view source)
    {
      return std::make_shared<Shader<Type>>(source);
    }

    explicit Shader(std::string_view source);
    ~Shader();

    [[nodiscard]] ShaderImpl<Type>& GetImpl() { return *impl; }

  private:
    std::unique_ptr<ShaderImpl<Type>> impl;
  };

  using VertexShader = Shader<ShaderType::Vertex>;
  using FragmentShader = Shader<ShaderType::Fragment>;
} // namespace SimpleGE::Graphic