#include <simplege/simplege.h>

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
    //Display the layer's sprites
    for (auto sprite : layerSprites) {
      spriteSheet->Bind(sprite->GetVertexBuffer(), sprite->GetIndexBuffer());
      glDrawElements(GL_TRIANGLES, (GLsizei) sprite->GetIndices().size(), GL_UNSIGNED_SHORT, nullptr);
      spriteSheet->Unbind();
    }
    
  }
} // namespace SimpleGE