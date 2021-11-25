#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE::Graphic
{
  struct VertexArraysImpl;

  class VertexArrays
  {
  public:
    static std::shared_ptr<VertexArrays> Create() { return std::make_shared<VertexArrays>(); }

    VertexArrays();
    ~VertexArrays();

    operator unsigned int() const;

  private:
    std::unique_ptr<VertexArraysImpl> impl;
  };
} // namespace SimpleGE::Graphic