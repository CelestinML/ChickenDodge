#include <simplege/simplege.h>

#include "../graphics/graphicapi.h"

#include <fmt/core.h>

namespace SimpleGE
{
  using namespace std::chrono_literals;

  static constexpr auto DebugInterval = 5s;

  struct DebugDrawCallsImpl
  {
    DebugDrawCallsImpl()
    {
      auto& apiDebug = APIDebug::Instance();
      origDrawArrays = apiDebug.drawArrays;
      origDrawElements = apiDebug.drawElements;
      apiDebug.drawArrays = [this](GLenum mode, GLint first, GLsizei count)
      {
        calls++;
        origDrawArrays(mode, first, count);
      };
      apiDebug.drawElements = [this](GLenum mode, GLsizei count, GLenum type, const void* indices)
      {
        calls++;
        origDrawElements(mode, count, type, indices);
      };
    }

    ~DebugDrawCallsImpl()
    {
      auto& apiDebug = APIDebug::Instance();
      apiDebug.drawArrays = origDrawArrays;
      apiDebug.drawElements = origDrawElements;
    }

    decltype(APIDebug::drawArrays) origDrawArrays;
    decltype(APIDebug::drawElements) origDrawElements;
    int calls{};
    int frames{};
    std::chrono::milliseconds elapsed{};
  };

  DebugDrawCallsComponent::DebugDrawCallsComponent(Entity& owner)
      : Component(owner), impl(std::make_unique<DebugDrawCallsImpl>())
  {
  }

  DebugDrawCallsComponent::~DebugDrawCallsComponent() = default;

  void DebugDrawCallsComponent::UpdateLogic(const Timing& timing)
  {
    impl->frames++;
    impl->elapsed += timing.delta;
    if (impl->elapsed > DebugInterval)
    {
      float avgCalls = float(impl->calls) / float(impl->frames);
      fmt::print(stderr, "Nb appels de rendu moyens par trame: {}\n", avgCalls);
      impl->calls = 0;
      impl->frames = 0;
      impl->elapsed = {};
    }
  }
} // namespace SimpleGE