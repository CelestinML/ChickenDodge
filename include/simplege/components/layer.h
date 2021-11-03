#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/visual.h>

#include <simplege/graphics/buffer.h>

namespace SimpleGE
{
  class LayerComponent : public VisualComponent
  {
  private:
    std::shared_ptr<Graphic::VertexBuffer> m_vertexBuffer;
    std::shared_ptr<Graphic::IndexBuffer> m_indexBuffer;

  public:
    static constexpr auto Type = "Layer";

    LayerComponent(Entity& owner) : Component(owner) {}

    void Display(const Timing& timing) override;
  };
} // namespace SimpleGE
