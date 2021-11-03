#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/logic.h>
#include <simplege/components/visual.h>

namespace SimpleGE
{
  class SpriteSheetComponent;

  struct SpriteImpl;

  class SpriteComponent : public LogicComponent, public VisualComponent
  {
  public:
    static constexpr auto Type = "Sprite";

    using AnimationEndedEventType = std::function<void()>;

    struct Description
    {
      int frameSkip{1};
      bool isAnimated{};
      int animWait{};
      std::string spriteName{};
      ComponentReference<SpriteSheetComponent> spriteSheet;
    };

    SpriteComponent(Entity& owner);
    ~SpriteComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const Timing& timing) override;
    void Display(const Timing& timing) override;

    void SetFrameSkip(int val);
    void SetAnimationFrame(int val);
    void SetIsAnimated(bool val);
    void UpdateMesh();

    void SetSpriteName(std::string_view spriteName);

    void RegisterAnimationEndedEvent(const AnimationEndedEventType& onAnimationEnded);

    [[nodiscard]] ComponentReference<SpriteSheetComponent> GetSpriteSheet() const;

    std::shared_ptr<Graphic::VertexBuffer> GetVertexBuffer() const;
    std::shared_ptr<Graphic::IndexBuffer> GetIndexBuffer() const;
    std::array<std::uint16_t, 6> GetIndices() const;

  private:
    std::unique_ptr<SpriteImpl> impl;
  };
} // namespace SimpleGE
