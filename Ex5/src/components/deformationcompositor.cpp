#include <simplege/simplege.h>

#include "compositor.h"

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, DeformationCompositorComponent::Description& desc)
  {
    from_json(j, static_cast<CompositorComponent::Description&>(desc));

    j.at("speed").get_to(desc.speed);
    j.at("scale").get_to(desc.scale);
    j.at("source").get_to(desc.source);
    j.at("intensity").get_to(desc.intensity);
  }

  struct DeformationCompositorImpl
  {
    float speed{};
    float scale{};
    std::chrono::time_point<std::chrono::steady_clock> start;
    std::shared_ptr<Graphic::Texture> deformation;
    std::shared_ptr<Graphic::Texture> intensity;
    GLint positionAttrib{};
    GLint uSampler{};
    GLint uDeformation{};
    GLint uIntensity{};
    GLint uTime{};
    GLint uScale{};
    std::shared_ptr<Graphic::VertexBuffer> screenQuad;
    std::shared_ptr<Graphic::RenderTexture> renderTexture;
  };

  struct DeformationCompositorVertex
  {
    static constexpr auto ItemCount = 2;
    float x, y;
  };

  static constexpr std::array<DeformationCompositorVertex, 6> Vertices{{
      {1.F, 1.F},
      {-1.F, 1.F},
      {-1.F, -1.F},
      {-1.F, -1.F},
      {1.F, -1.F},
      {1.F, 1.F},
  }};

  DeformationCompositorComponent::DeformationCompositorComponent(Entity& owner)
      : CompositorComponent(owner), Component(owner), impl(std::make_unique<DeformationCompositorImpl>())
  {
  }

  DeformationCompositorComponent::~DeformationCompositorComponent() = default;

  void DeformationCompositorComponent::OnEnabled() { impl->start = std::chrono::steady_clock::now(); }

  Component::SetupResult DeformationCompositorComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult DeformationCompositorComponent::Setup(const Description& descr)
  {
    return {this,
            {{[]() { return CameraComponent::Current() != nullptr; },
              [this, descr]()
              {
                const auto* currentCamera = CameraComponent::Current();
                Ensures(currentCamera != nullptr);

                const auto Width = currentCamera->ViewWidth();
                const auto Height = currentCamera->ViewHeight();

                impl->speed = descr.speed;
                impl->scale = descr.scale;
                impl->start = std::chrono::steady_clock::now();

                auto status = CompositorComponent::Setup(descr);
                Ensures(status.IsDone());
                Bind();

                auto deformationImage = Resources::Get<Image>(descr.source);
                auto intensityImage = Resources::Get<Image>(descr.intensity);

                Ensures(deformationImage != nullptr);
                Ensures(intensityImage != nullptr);

                {
                  using namespace Graphic;

                  impl->deformation =
                      Texture::Create({deformationImage, Texture::Filter::Linear, Texture::Filter::Linear,
                                       Texture::Wrap::Wrap, Texture::Wrap::Wrap});

                  impl->intensity = Texture::Create({intensityImage, Texture::Filter::Linear, Texture::Filter::Linear,
                                                     Texture::Wrap::Clamp, Texture::Wrap::Clamp});

                  impl->renderTexture =
                      RenderTexture::Create({Width, Height, Texture::Filter::Nearest, Texture::Filter::Nearest,
                                             Texture::Wrap::Clamp, Texture::Wrap::Clamp});

                  impl->screenQuad = VertexBuffer::Create(gsl::span<const DeformationCompositorVertex>(Vertices),
                                                          VertexBuffer::Usage::Static);
                }

                auto shader = GetCompositorImpl().shader;

                impl->positionAttrib = glGetAttribLocation(*shader, "aPosition");
                impl->uSampler = glGetUniformLocation(*shader, "uSampler");
                impl->uDeformation = glGetUniformLocation(*shader, "uDeformation");
                impl->uIntensity = glGetUniformLocation(*shader, "uIntensity");
                impl->uTime = glGetUniformLocation(*shader, "uTime");
                impl->uScale = glGetUniformLocation(*shader, "uScale");

                {
                  Graphic::RenderTextureScope rtScope(impl->renderTexture);
                  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }

                glBindTexture(GL_TEXTURE_2D, 0);

                Unbind();
              }}}};
  }

  std::shared_ptr<Graphic::RenderTexture>
  DeformationCompositorComponent::Compose(const std::shared_ptr<Graphic::RenderTexture>& texture)
  {
    Graphic::RenderTextureScope rtScope(impl->renderTexture);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Bind();

    glBindBuffer(GL_ARRAY_BUFFER, *impl->screenQuad);
    glEnableVertexAttribArray(impl->positionAttrib);
    constexpr auto PositionOffset = 0U;
    glVertexAttribPointer(impl->positionAttrib, DeformationCompositorVertex::ItemCount, GL_FLOAT, GL_FALSE, 0,
                          (GLvoid*) PositionOffset); // NOLINT

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glUniform1i(impl->uSampler, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *impl->deformation);
    glUniform1i(impl->uDeformation, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, *impl->intensity);
    glUniform1i(impl->uIntensity, 2);

    const auto elapsed =
        std::chrono::duration<float>(std::chrono::steady_clock::now() - impl->start).count() * impl->speed;
    glUniform1f(impl->uTime, elapsed);

    glUniform1f(impl->uScale, impl->scale);

    glDrawArrays(GL_TRIANGLES, 0, sizeof(Vertices) / sizeof(Vertices[0]));
    glDisableVertexAttribArray(impl->positionAttrib);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    Unbind();

    if (elapsed >= 1.F)
    {
      Disable();
    }

    return impl->renderTexture;
  }
} // namespace SimpleGE