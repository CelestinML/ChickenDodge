#include <simplege/simplege.h>
#include <vector>

#include "../graphics/graphicapi.h"

namespace SimpleGE
{
  static std::vector<gsl::not_null<SpriteComponent*>> ListSprites(const std::shared_ptr<Entity>& owner)
  {
    std::queue<std::shared_ptr<Entity>> queue;
    queue.push(owner);

    std::vector<gsl::not_null<SpriteComponent*>> sprites;

    while (!queue.empty())
    {
      auto node = queue.front();
      queue.pop();

      for (const auto& child : node->GetChildren())
      {
        if (child->Active())
        {
          queue.push(child);
        }

        auto* comp = child->GetComponent<SpriteComponent>();
        if (comp != nullptr)
        {
          sprites.emplace_back(comp);
        }
      }
    }

    return sprites;
  }

  void LayerComponent::Display(const Timing& timing)
  {
    //Sans batching autour de 130-140 appels de rendus moyens
    //Get the sprites
    auto layerSprites = ListSprites(Owner().shared_from_this());
    if (layerSprites.empty())
    {
      return;
    }
    auto spriteSheet = gsl::at(layerSprites, 0)->GetSpriteSheet();
    Ensures(spriteSheet.Ready());

    //Create VertexBuffer
    std::vector<Vertex> vertices{layerSprites.size() * 4};
    m_vertexBuffer = Graphic::VertexBuffer::Create(gsl::span<const Vertex>(vertices),
                                                   Graphic::VertexBuffer::Usage::Dynamic);

    //Create IndexBuffer
    std::vector<std::uint16_t> indices;
    indices.resize(layerSprites.size() * 6);
    m_indexBuffer = Graphic::IndexBuffer::Create(gsl::span<const std::uint16_t>(indices),
                                                 Graphic::IndexBuffer::Usage::Dynamic);

    int offset = 0;

    //Display the layer's sprites
    for (auto sprite : layerSprites) {
      sprite->UpdateComponents(m_vertexBuffer, offset * 4, m_indexBuffer, offset * 6);
      offset++;
    }
    spriteSheet->Bind(m_vertexBuffer, m_indexBuffer);
    glDrawElements(GL_TRIANGLES, (GLsizei) layerSprites.size() * 6, GL_UNSIGNED_SHORT, nullptr);
    spriteSheet->Unbind();
    
  }
} // namespace SimpleGE