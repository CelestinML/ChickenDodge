#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/math.h>
#include <simplege/timing.h>

struct GLFWwindow;

namespace SimpleGE
{
  struct CameraImpl;
  using GraphicAPIWindowType = GLFWwindow;

  class CompositorComponent;

  class CameraComponent : public Component
  {
  public:
    static constexpr auto Type = "Camera";

    struct ColorFloat
    {
      float r, g, b, a;
    };

    struct Description
    {
      std::string title;
      int viewHeight;
      int viewWidth;
      ColorFloat color;
      float height;
      float nearClip;
      float farClip;
      std::vector<ComponentReference<CompositorComponent>> compositors;
    };

    static CameraComponent* Current() { return current; }

    CameraComponent(Entity& owner);
    ~CameraComponent() override;

    void Render(const Timing& timing);

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] const Matrix<4, 4>& Projection();

    [[nodiscard]] int ViewWidth() const;
    [[nodiscard]] int ViewHeight() const;

    [[nodiscard]] GraphicAPIWindowType& GetNativeWindow() const;

  private:
    static inline CameraComponent* current;
    std::unique_ptr<CameraImpl> impl;
  };
} // namespace SimpleGE
