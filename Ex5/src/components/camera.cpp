#include <simplege/simplege.h>

#include "../graphics/graphicapi.h"

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, CameraComponent::ColorFloat& desc)
  {
    j.at("r").get_to(desc.r);
    j.at("g").get_to(desc.g);
    j.at("b").get_to(desc.b);
    j.at("a").get_to(desc.a);
  }

  static void from_json(const json& j, CameraComponent::Description& desc)
  {
    j.at("title").get_to(desc.title);
    j.at("viewHeight").get_to(desc.viewHeight);
    j.at("viewWidth").get_to(desc.viewWidth);
    j.at("color").get_to(desc.color);
    j.at("height").get_to(desc.height);
    j.at("near").get_to(desc.nearClip);
    j.at("far").get_to(desc.farClip);
    j.at("compositors").get_to(desc.compositors);
  }

  struct CameraImpl
  {
    int viewWidth{};
    int viewHeight{};
    float height{};
    float nearClip{};
    float farClip{};
    GraphicAPIWindowType* window{};
    std::shared_ptr<Graphic::RenderTexture> renderTexture;
    Matrix<4, 4> projection{};
    std::vector<ComponentReference<CompositorComponent>> compositors;
  };

  static void InitDisplaySystem()
  {
    static bool initialized = false;
    if (initialized)
    {
      return;
    }

    if (glfwInit() != GLFW_TRUE)
    {
      fmt::print(stderr, "Échec à l'initialisation de GLFW\n");
      std::terminate();
    }

    initialized = true;
  }

  static void GLAPIENTRY OnGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                          const GLchar* message, const void* userParam)
  {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
      return;
    }

    fmt::print(stderr, "GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
               (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    Expects(type != GL_DEBUG_TYPE_ERROR);
  }

  CameraComponent::CameraComponent(Entity& owner) : Component(owner), impl(std::make_unique<CameraImpl>())
  {
    DisplaySystem::Instance().Register(this);
  }

  CameraComponent::~CameraComponent() { DisplaySystem::Instance().Unregister(this); }

  Component::SetupResult CameraComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult CameraComponent::Setup(const Description& descr)
  {
    current = this;

    InitDisplaySystem();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    impl->viewWidth = descr.viewWidth;
    impl->viewHeight = descr.viewHeight;
    impl->height = descr.height;
    impl->nearClip = descr.nearClip;
    impl->farClip = descr.farClip;
    impl->window = glfwCreateWindow(ViewWidth(), ViewHeight(), descr.title.c_str(), nullptr, nullptr);
    Ensures(impl->window);
    glfwMakeContextCurrent(impl->window);

    if (auto err = glewInit(); err != GLEW_OK)
    {
      fmt::print(stderr, "Erreur: {}\n", glewGetErrorString(err));
      glfwTerminate();
      std::terminate();
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&OnGLDebugMessage, nullptr);

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(descr.color.r, descr.color.g, descr.color.b, descr.color.a);

    glViewport(0, 0, ViewWidth(), ViewHeight());

    {
      using namespace Graphic;

      impl->renderTexture =
          RenderTexture::Create({ViewWidth(), ViewHeight(), Texture::Filter::Nearest, Texture::Filter::Nearest,
                                 Texture::Wrap::Clamp, Texture::Wrap::Clamp});
    }

    {
      Graphic::RenderTextureScope rtScope(impl->renderTexture);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    impl->compositors = std::move(descr.compositors);

    glfwMakeContextCurrent(impl->window);

    glfwSwapInterval(1);

    return {this, {}};
  }

  void CameraComponent::Render(const Timing& timing)
  {
    current = this;

    auto rt = impl->renderTexture;
    for (const auto& comp : impl->compositors)
    {
      if (comp->Enabled())
      {
        rt = comp->Compose(rt);
      }
    }

    impl->renderTexture->BindFramebuffer();

    glfwSwapBuffers(impl->window);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (glfwWindowShouldClose(impl->window) == GLFW_TRUE)
    {
      Game::Close();
    }
  }

  const Matrix<4, 4>& CameraComponent::Projection()
  {
    gsl::not_null<PositionComponent*> posComp = Owner().GetComponent<PositionComponent>();
    auto position = posComp->WorldPosition();
    float x = position[0];
    float y = position[1];
    float z = position[2];
    float ratio = float(ViewWidth()) / float(ViewHeight());
    float height = impl->height;
    float width = ratio * height;
    float nearClip = impl->nearClip;
    float farClip = impl->farClip;

    return impl->projection.Ortho(x - width, x + width, -y + height, -y - height, z + nearClip, z + farClip);
  }

  [[nodiscard]] int CameraComponent::ViewWidth() const { return impl->viewWidth; }

  [[nodiscard]] int CameraComponent::ViewHeight() const { return impl->viewHeight; }

  [[nodiscard]] GraphicAPIWindowType& CameraComponent::GetNativeWindow() const
  {
    Expects(impl->window != nullptr);
    return *impl->window;
  }
} // namespace SimpleGE