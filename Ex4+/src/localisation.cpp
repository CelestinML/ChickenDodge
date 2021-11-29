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

    //On cherche les clés à utiliser pour trouver la bonne traduction
    //Pour celan on créer un pattern de recherche contenant une accolade ouvrante, 
    //un mot constitué d'un certain nombre de caractères, et une accolade fermante
    std::regex search("\\{([^\\}]*)\\}");
    std::smatch match;
    //L'offset nous permettra de ne pas avoir à rechercher dans l'intégralité du mot à chaque boucle
    std::size_t offset = 0;
    while(std::regex_search(result.cbegin() + offset, result.cend(), match, search))
    {
      //On a trouvé un mot à remplacer
      //On récupère la traduction dans la langue actuelle
      std::string new_text = mergedContext.at(match.str(1));
      //On remplace le pattern trouvé (de l'accolade ouvrante à l'accolade fermante) par la traduction
      result.replace(match[0].first, match[0].first + match[0].length(),new_text);
      //On augmente l'offset pour ne pas re chercher dans l'intégralité du texte
      offset += match.position(0) + new_text.length();
    }

    //Une fois qu'on a trouvé et remplacé tous les patterns, on retourne enfin le résultat
    return result;
  }
} // namespace SimpleGE