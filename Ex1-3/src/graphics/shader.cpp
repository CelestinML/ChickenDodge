#include <simplege/simplege.h>

#include "shader.h"

namespace SimpleGE::Graphic
{
  template<ShaderType Type>
  Shader<Type>::Shader(std::string_view source) : impl(std::make_unique<ShaderImpl<Type>>(source))
  {
  }

  template Shader<ShaderType::Fragment>::Shader(std::string_view source);
  template Shader<ShaderType::Vertex>::Shader(std::string_view source);

  template<ShaderType Type>
  Shader<Type>::~Shader() = default;

  template Shader<ShaderType::Fragment>::~Shader();
  template Shader<ShaderType::Vertex>::~Shader();
} // namespace SimpleGE::Graphic