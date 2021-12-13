#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/image.h>
#include <simplege/textasset.h>

namespace SimpleGE::Resources
{
  constexpr auto DataPath = "data";

  template<typename T>
  [[nodiscard]] std::shared_ptr<T> Get(std::string_view name);

  template<>
  [[nodiscard]] std::shared_ptr<Image> Get<Image>(std::string_view name);

  template<>
  [[nodiscard]] std::shared_ptr<TextAsset> Get<TextAsset>(std::string_view name);
} // namespace SimpleGE::Resources