#include <simplege/simplege.h>

#include <fmt/core.h>

#include <regex>

using json = nlohmann::json;

namespace SimpleGE
{
  void Localisation::Init(const LocaleFiles& locales, std::string_view lang)
  {
    auto curLocale = locales.find(std::string(lang));
    if (curLocale == locales.end())
    {
      curLocale = locales.begin();
      fmt::print(stderr, "Config pour {} introuvable, on va utiliser {} à la place.\n", lang, curLocale->first);
    }

    auto content = Resources::Get<TextAsset>(curLocale->second);
    Ensures(content != nullptr);
    json::parse(content->Value()).get_to(Instance().localeStrings);
  }

  [[nodiscard]] std::string Localisation::GetImpl(std::string_view key, const LocaleContext& queryContext) const
  {
    LocaleContext mergedContext;
    std::set_union(queryContext.begin(), queryContext.end(), globalContext.begin(), globalContext.end(),
                   std::inserter(mergedContext, mergedContext.end()));

    auto localized = localeStrings.find(std::string(key));
    if (localized == localeStrings.end())
    {
      fmt::print(stderr, "Clé régionalisée {} introuvable.\n", key);
      return std::string(key);
    }

    std::string result = localized->second;

    for(const auto & [key, value] : mergedContext)
    {
      std::regex regex("\\{" + key + "\\}");
      result = std::regex_replace(result, regex, value);
    }

    return result;
  }
} // namespace SimpleGE