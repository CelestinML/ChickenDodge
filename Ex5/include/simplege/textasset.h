#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class TextAsset
  {
  public:
    static std::shared_ptr<TextAsset> Create(std::string&& content)
    {
      return std::make_shared<TextAsset>(std::move(content));
    }

    explicit TextAsset(std::string&& content) : m_Content(std::move(content)) {}

    [[nodiscard]] std::string_view Value() const { return m_Content; }

    operator std::string_view() const { return Value(); }

  private:
    std::string m_Content;

    friend std::shared_ptr<TextAsset> std::make_shared<TextAsset>();
  };
} // namespace SimpleGE