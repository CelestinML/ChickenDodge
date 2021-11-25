#pragma once

#include <simplege/pch/precomp.h>

#include "../graphics/graphicapi.h"

namespace SimpleGE
{
  static void from_json(const nlohmann::json& j, CompositorComponent::Description& desc)
  {
    j.at("vertexShader").get_to(desc.vertexShader);
    j.at("fragmentShader").get_to(desc.fragmentShader);
  }

  struct CompositorImpl
  {
    std::shared_ptr<Graphic::Program> shader;
    std::shared_ptr<Graphic::VertexArrays> vao;
  };
} // namespace SimpleGE