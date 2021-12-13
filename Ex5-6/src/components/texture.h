#pragma once

#include <simplege/pch/precomp.h>

#include <GL/glew.h>

namespace SimpleGE
{
  static void from_json(const nlohmann::json& j, TextureComponent::Description& desc)
  {
    j.at("texture").get_to(desc.texture);
    j.at("vertexShader").get_to(desc.vertexShader);
    j.at("fragmentShader").get_to(desc.fragmentShader);
  }
} // namespace SimpleGE