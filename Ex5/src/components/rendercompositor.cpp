#include <simplege/simplege.h>

#include "compositor.h"

using json = nlohmann::json;

namespace SimpleGE
{
  struct RenderCompositorImpl
  {
    GLint positionAttrib;
    GLint uSampler;
    std::shared_ptr<Graphic::VertexBuffer> screenQuad;
  };

  struct RenderCompositorVertex
  {
    static constexpr auto ItemCount = 2;
    float x, y;
  };

  static constexpr std::array<RenderCompositorVertex, 6> Vertices{{
      {1.F, 1.F},
      {-1.F, 1.F},
      {-1.F, -1.F},
      {-1.F, -1.F},
      {1.F, -1.F},
      {1.F, 1.F},
  }};

  RenderCompositorComponent::RenderCompositorComponent(Entity& owner)
      : CompositorComponent(owner), Component(owner), impl(std::make_unique<RenderCompositorImpl>())
  {
  }

  RenderCompositorComponent::~RenderCompositorComponent() = default;

  Component::SetupResult RenderCompositorComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult RenderCompositorComponent::Setup(const Description& descr)
  {
    auto status = CompositorComponent::Setup(descr);
    Ensures(status.IsDone());
    Bind();

    auto shader = GetCompositorImpl().shader;

    impl->positionAttrib = glGetAttribLocation(*shader, "aPosition");
    impl->uSampler = glGetUniformLocation(*shader, "uSampler");

    impl->screenQuad = Graphic::VertexBuffer::Create(gsl::span<const RenderCompositorVertex>(Vertices),
                                                     Graphic::VertexBuffer::Usage::Static);

    Unbind();

    return {this, {}};
  }

  std::shared_ptr<Graphic::RenderTexture>
  RenderCompositorComponent::Compose(const std::shared_ptr<Graphic::RenderTexture>& texture)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Bind();

    glBindBuffer(GL_ARRAY_BUFFER, *impl->screenQuad);
    glEnableVertexAttribArray(impl->positionAttrib);
    constexpr auto PositionOffset = 0U;
    glVertexAttribPointer(impl->positionAttrib, RenderCompositorVertex::ItemCount, GL_FLOAT, GL_FALSE, 0,
                          (GLvoid*) PositionOffset); // NOLINT

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glUniform1i(impl->uSampler, 0);

    glDrawArrays(GL_TRIANGLES, 0, sizeof(Vertices) / sizeof(Vertices[0]));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisableVertexAttribArray(impl->positionAttrib);

    Unbind();

    // On ne s'en sert plus après ça
    return texture;
  }
} // namespace SimpleGE