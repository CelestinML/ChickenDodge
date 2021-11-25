#include <simplege/simplege.h>

#include "program.h"

namespace SimpleGE::Graphic
{
  Program::Program() : impl(std::make_unique<ProgramImpl>()) {}

  Program::~Program() = default;

  Program::operator unsigned int() const { return impl->program; }

  template<ShaderType Type>
  void Program::Attach(const std::shared_ptr<Shader<Type>>& shader)
  {
    impl->Attach(shader);
  }

  template void Program::Attach(const std::shared_ptr<Shader<ShaderType::Fragment>>& shader);
  template void Program::Attach(const std::shared_ptr<Shader<ShaderType::Vertex>>& shader);

  void Program::Link() { impl->Link(); }
} // namespace SimpleGE::Graphic