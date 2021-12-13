#pragma once

#include <simplege/pch/precomp.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SimpleGE
{
  using DrawArraysFuncType = decltype(glDrawArrays);
  template<typename... Args>
  static void OrigGlDrawArrays(Args&&... args)
  {
    glDrawArrays(std::forward<Args>(args)...);
  }

  using DrawElementsFuncType = decltype(glDrawElements);
  template<typename... Args>
  static void OrigGlDrawElements(Args&&... args)
  {
    glDrawElements(std::forward<Args>(args)...);
  }

  struct APIDebug
  {
    static APIDebug& Instance()
    {
      static APIDebug instance;
      return instance;
    }

    std::function<DrawArraysFuncType> drawArrays;
    std::function<DrawElementsFuncType> drawElements;

  private:
    APIDebug()
        : drawArrays([](GLenum mode, GLint first, GLsizei count) { OrigGlDrawArrays(mode, first, count); }),
          drawElements([](GLenum mode, GLsizei count, GLenum type, const void* indices)
                       { OrigGlDrawElements(mode, count, type, indices); })
    {
    }
  };

  template<typename... Args>
  static void WrappedGlDrawArrays(Args&&... args)
  {
    APIDebug::Instance().drawArrays(std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void WrappedGlDrawElements(Args&&... args)
  {
    APIDebug::Instance().drawElements(std::forward<Args>(args)...);
  }

#define glDrawArrays ::SimpleGE::WrappedGlDrawArrays
#define glDrawElements ::SimpleGE::WrappedGlDrawElements

  using GraphicAPIWindowType = GLFWwindow;
} // namespace SimpleGE