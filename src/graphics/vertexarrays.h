#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/vertexarrays.h>

#include "graphicapi.h"

namespace SimpleGE::Graphic
{
  struct VertexArraysImpl
  {
    VertexArraysImpl() { glGenVertexArrays(1, &vao); }

    ~VertexArraysImpl() { glDeleteVertexArrays(1, &vao); }

    GLuint vao;
  };
} // namespace SimpleGE::Graphic