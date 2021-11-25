#include <simplege/simplege.h>

#include <fmt/core.h>

#include "../graphics/graphicapi.h"

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, SpriteComponent::Description& desc)
  {
    if (j.contains("frameSkip"))
    {
      j.at("frameSkip").get_to(desc.frameSkip);
    }
    if (j.contains("isAnimated"))
    {
      j.at("isAnimated").get_to(desc.isAnimated);
    }
    if (j.contains("animWait"))
    {
      j.at("animWait").get_to(desc.animWait);
    }
    if (j.contains("spriteName"))
    {
      j.at("spriteName").get_to(desc.spriteName);
    }
    j.at("spriteSheet").get_to(desc.spriteSheet);
  }

  struct Vertex
  {
    float x, y, z;
    float u, v;
  };

  struct SpriteImpl
  {
    ComponentReference<SpriteSheetComponent> spriteSheet;
    std::string spriteName;
    EventTrigger<SpriteComponent::AnimationEndedEventType> animationEndedEvent;
    bool isAnimated{};
    int frameSkip{};
    int animationFrame{};
    int animWait{};
    int animWaitCounter{};
    const SpriteSheetComponent::Frame* descr{};
    Size<int> spriteSize{};
    std::shared_ptr<Graphic::VertexBuffer> vertexBuffer;
    std::array<Vertex, 4> vertices{};
    static constexpr std::array<std::uint16_t, 6> Indices{0, 1, 2, 2, 3, 0};
    std::shared_ptr<Graphic::IndexBuffer> indexBuffer;

    // Cette méthode met à jour les informations relatives à la sprite
    // à afficher.
    void UpdateMesh()
    {
      const auto& newSpriteName = isAnimated ? FindNextFrameName() : spriteName;
      Expects(spriteSheet != nullptr);
      descr = spriteSheet->GetSprite(newSpriteName);
      if (!descr)
        return; // HACK
      Ensures(descr);
      spriteSize = descr->sourceSize;
    }

    // La fonction *FindNextFrameName* détermine le nom de la sprite
    // à afficher dans une animation, et déclenche des événements
    // enregistrés si on atteint la fin de l'animation.
    std::string FindNextFrameName()
    {
      auto animationSprite = fmt::format("{}{}", spriteName, animationFrame);
      if (spriteSheet->GetSprite(animationSprite) != nullptr)
      {
        animationFrame++;
        return animationSprite;
      }
      if (animationFrame == 1)
      {
        return spriteName;
      }
      else
      {
        animationFrame = 1;
        animWaitCounter = animWait;
        animationEndedEvent.Trigger();
        return FindNextFrameName();
      }
    }

    // Cette méthode met à jour le contenu de chaque vertex, soient
    // leurs position et les coordonnées de texture, en tenant compte
    // des transformations et de la sprite courante.
    void UpdateComponents(Entity& owner)
    {
      if (!descr)
        return; // HACK
      gsl::not_null<PositionComponent*> posComp = owner.GetComponent<PositionComponent>();
      const auto position = posComp->WorldPosition();

      const auto z = position[2];
      const auto xMin = position[0];
      const auto xMax = xMin + float(descr->frame.width());
      const auto yMax = position[1];
      const auto yMin = yMax - float(descr->frame.height());
      const auto uMin = descr->uv.x();
      const auto uMax = uMin + float(descr->uv.width());
      const auto vMin = descr->uv.y();
      const auto vMax = vMin + float(descr->uv.height());

      std::array<Vertex, 4> v{{
          {xMin, yMin, z, uMin, vMin},
          {xMax, yMin, z, uMax, vMin},
          {xMax, yMax, z, uMax, vMax},
          {xMin, yMax, z, uMin, vMax},
      }};

      std::copy(v.begin(), v.end(), vertices.begin());
      vertexBuffer->Update(gsl::span<const Vertex>(vertices), 0);
      indexBuffer->Update(gsl::span<const std::uint16_t>(Indices), 0);
    }
  };

  SpriteComponent::SpriteComponent(Entity& owner) : Component(owner), impl(std::make_unique<SpriteImpl>()) {}

  SpriteComponent::~SpriteComponent() = default;

  Component::SetupResult SpriteComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult SpriteComponent::Setup(const Description& descr)
  {
    impl->frameSkip = descr.frameSkip;
    impl->isAnimated = descr.isAnimated;
    impl->animWait = descr.animWait;
    impl->spriteName = descr.spriteName;
    impl->animationFrame = 1;
    impl->animWaitCounter = impl->animWait;
    impl->spriteSheet = descr.spriteSheet;

    return {this,
            {{[this]() { return impl->spriteSheet.Ready(); },
              [this]()
              {
                // On crée ici un tableau de 4 vertices permettant de représenter
                // le rectangle à afficher.
                impl->vertexBuffer = Graphic::VertexBuffer::Create(gsl::span<const Vertex>(impl->vertices),
                                                                   Graphic::VertexBuffer::Usage::Dynamic);

                // On crée ici un tableau de 6 indices, soit 2 triangles, pour
                // représenter quels vertices participent à chaque triangle:
                // ```
                // 0    1
                // +----+
                // |\   |
                // | \  |
                // |  \ |
                // |   \|
                // +----+
                // 3    2
                // ```
                impl->indexBuffer = Graphic::IndexBuffer::Create(gsl::span<const std::uint16_t>(impl->Indices),
                                                                 Graphic::IndexBuffer::Usage::Dynamic);

                // Et on initialise le contenu des vertices
                impl->UpdateMesh();
                impl->UpdateComponents(Owner());
              }}}};
  }

  // Cette méthode met à jour l'animation de la sprite, si il
  // y a lieu, et met à jour le contenu des vertices afin de tenir
  // compte des changements de position et autres.
  void SpriteComponent::UpdateLogic(const Timing& timing)
  {
    if (impl->isAnimated)
    {
      if (impl->animWaitCounter > 0)
      {
        impl->animWaitCounter--;
      }
      else if ((timing.frame % impl->frameSkip) == 0)
      {
        impl->UpdateMesh();
      }
    }

    impl->UpdateComponents(Owner());
  }

  // La méthode *display* choisit le shader et la texture appropriée
  // via la méthode *bind* de la feuille de sprite, sélectionne le
  // tableau de vertices et d'indices et fait l'appel de rendu.
  void SpriteComponent::Display(const Timing& timing)
  {
    impl->spriteSheet->Bind(impl->vertexBuffer, impl->indexBuffer);
    glDrawElements(GL_TRIANGLES, (GLsizei) impl->Indices.size(), GL_UNSIGNED_SHORT, nullptr);
    impl->spriteSheet->Unbind();
  }

  void SpriteComponent::SetFrameSkip(int val) { impl->frameSkip = val; }

  void SpriteComponent::SetAnimationFrame(int val) { impl->animationFrame = val; }

  void SpriteComponent::SetIsAnimated(bool val) { impl->isAnimated = val; }

  void SpriteComponent::UpdateMesh() { impl->UpdateMesh(); }

  void SpriteComponent::SetSpriteName(std::string_view spriteName)
  {
    impl->spriteName = spriteName;
    impl->UpdateComponents(Owner());
  }

  void SpriteComponent::RegisterAnimationEndedEvent(const AnimationEndedEventType& onAnimationEnded)
  {
    impl->animationEndedEvent.Register(onAnimationEnded);
  }

  [[nodiscard]] ComponentReference<SpriteSheetComponent> SpriteComponent::GetSpriteSheet() const
  {
    return impl->spriteSheet;
  }

} // namespace SimpleGE