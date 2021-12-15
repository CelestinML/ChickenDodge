#include <simplege/simplege.h>

#include "vertexarrays.h"

namespace SimpleGE::Graphic
{
  VertexArrays::VertexArrays() : impl(std::make_unique<VertexArraysImpl>()) {}

  VertexArrays::~VertexArrays() = default;

  VertexArrays::operator unsigned int() const { return impl->vao; }
} // namespace SimpleGE::Graphic