#include <simplege/simplege.h>

#include "compositor.h"

#include <fmt/core.h>

using json = nlohmann::json;

namespace SimpleGE
{
  CompositorComponent::CompositorComponent(Entity& owner) : Component(owner), impl(std::make_unique<CompositorImpl>())
  {
  }

  CompositorComponent::~CompositorComponent() = default;

  Component::SetupResult CompositorComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult CompositorComponent::Setup(const Description& descr)
  {
    auto vsSource = Resources::Get<TextAsset>(descr.vertexShader);
    auto fsSource = Resources::Get<TextAsset>(descr.fragmentShader);

    Ensures(vsSource != nullptr);
    Ensures(fsSource != nullptr);

    auto vs = Graphic::VertexShader::Create(*vsSource);
    auto fs = Graphic::FragmentShader::Create(*fsSource);

    impl->shader = Graphic::Program::Create(vs, fs);
    impl->vao = Graphic::VertexArrays::Create();

    return {this, {}};
  }

  void CompositorComponent::Bind()
  {
    glBindVertexArray(*impl->vao);
    glUseProgram(*impl->shader);
  }

  void CompositorComponent::Unbind() { glBindVertexArray(0); }
} // namespace SimpleGE