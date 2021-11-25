#include <simplege/simplege.h>

#include <filesystem>

namespace SimpleGE::Resources
{
  template<typename T, typename E>
  [[nodiscard]] std::optional<T> GetContent(std::string_view name, bool binary)
  {
    std::filesystem::path filePath = DataPath;
    filePath.append(name);
    std::ifstream data(filePath, binary ? std::ios::binary : std::ios::in);
    if (!data.is_open())
    {
      return std::nullopt;
    }

    std::vector<E> buffer;
    std::transform(std::istreambuf_iterator<char>(data), std::istreambuf_iterator<char>(),
                   std::back_insert_iterator(buffer), [](char c) { return static_cast<E>(c); });

    return T{buffer.begin(), buffer.end()};
  }

  template<typename T, typename C, typename E>
  [[nodiscard]] std::shared_ptr<T> Get(std::string_view name, bool binary)
  {
    auto content = GetContent<C, E>(name, binary);
    if (content.has_value())
    {
      return T::Create(std::move(*content));
    }

    return {};
  }

  template<>
  [[nodiscard]] std::shared_ptr<Image> Get<Image>(std::string_view name)
  {
    return Get<Image, std::vector<std::byte>, std::byte>(name, true);
  }

  template<>
  [[nodiscard]] std::shared_ptr<TextAsset> Get<TextAsset>(std::string_view name)
  {
    return Get<TextAsset, std::string, char>(name, false);
  }
} // namespace SimpleGE::Resources