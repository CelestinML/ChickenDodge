#include <simplege/simplege.h>

#include "texture.h"

using json = nlohmann::json;

namespace SimpleGE
{
  struct TextureComponentImpl
  {
    std::shared_ptr<Image> image;
    std::shared_ptr<Graphic::Texture> texture;
    std::shared_ptr<Graphic::VertexShader> vertexShader;
    std::shared_ptr<Graphic::FragmentShader> fragmentShader;
    std::shared_ptr<Graphic::VertexArrays> vao;
    std::shared_ptr<Graphic::Program> program;
    GLint vertexPositionAttrib{};
    GLint textureCoordAttrib{};
    GLint pUniform{};
    GLint mvUniform{};
    GLint uSampler{};
  };

  TextureComponent::TextureComponent(Entity& owner) : Component(owner), impl(std::make_unique<TextureComponentImpl>())
  {
  }

  TextureComponent::~TextureComponent() = default;

  Component::SetupResult TextureComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult TextureComponent::Setup(const Description& descr)
  {
    impl->image = Resources::Get<Image>(descr.texture);
    Ensures(impl->image != nullptr);

    {
      using namespace Graphic;
      impl->texture = Texture::Create({impl->image, Texture::Filter::Nearest, Texture::Filter::Nearest,
                                       Texture::Wrap::Clamp, Texture::Wrap::Clamp});
    }

    auto vsSource = Resources::Get<TextAsset>(descr.vertexShader);
    Ensures(vsSource != nullptr);
    impl->vertexShader = Graphic::VertexShader::Create(*vsSource);

    auto fsSource = Resources::Get<TextAsset>(descr.fragmentShader);
    Ensures(fsSource != nullptr);
    impl->fragmentShader = Graphic::FragmentShader::Create(*fsSource);

    impl->program = Graphic::Program::Create(impl->vertexShader, impl->fragmentShader);
    impl->vao = Graphic::VertexArrays::Create();

    impl->vertexPositionAttrib = glGetAttribLocation(*impl->program, "aVertexPosition");
    impl->textureCoordAttrib = glGetAttribLocation(*impl->program, "aTextureCoord");

    impl->pUniform = glGetUniformLocation(*impl->program, "uPMatrix");
    impl->mvUniform = glGetUniformLocation(*impl->program, "uMVMatrix");
    impl->uSampler = glGetUniformLocation(*impl->program, "uSampler");

    return {this, {}};
  }

  void TextureComponent::Bind(const std::shared_ptr<Graphic::VertexBuffer>& vertexBuffer,
                              const std::shared_ptr<Graphic::IndexBuffer>& indexBuffer)
  {
    // ## Constante *vertexSize*
    // Cette constante représente le nombre d'éléments d'un vertex,
    // soit 3 valeurs pour la position, et 2 pour la texture
    constexpr auto VertexSize = 3 + 2; // position(3d), texture(2d)
    constexpr auto Stride = VertexSize * sizeof(float);

    glBindVertexArray(*impl->vao);
    glUseProgram(*impl->program);

    glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);

    glEnableVertexAttribArray(impl->vertexPositionAttrib);
    glEnableVertexAttribArray(impl->textureCoordAttrib);
    constexpr auto PositionOffset = 0U;
    constexpr auto TexCoordOffset = 3U * sizeof(float);
    glVertexAttribPointer(impl->vertexPositionAttrib, 3, GL_FLOAT, GL_FALSE, Stride,
                          (GLvoid*) PositionOffset);                                                          // NOLINT
    glVertexAttribPointer(impl->textureCoordAttrib, 2, GL_FLOAT, GL_FALSE, Stride, (GLvoid*) TexCoordOffset); // NOLINT

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBuffer);

    Expects(CameraComponent::Current() != nullptr);
    glUniformMatrix4fv(impl->pUniform, 1, GL_FALSE, (const float*) CameraComponent::Current()->Projection());
    Matrix<4, 4> identity{};
    glUniformMatrix4fv(impl->mvUniform, 1, GL_FALSE, (const float*) identity.LoadIdentity());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *impl->texture);
    glUniform1i(impl->uSampler, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void TextureComponent::Unbind()
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisableVertexAttribArray(impl->vertexPositionAttrib);
    glDisableVertexAttribArray(impl->textureCoordAttrib);
    glBindVertexArray(0);
  }

  [[nodiscard]] std::shared_ptr<Image> TextureComponent::GetTexture() const { return impl->image; }
} // namespace SimpleGE