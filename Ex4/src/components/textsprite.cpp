#include <simplege/simplege.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, TextSpriteComponent::Description& desc)
  {
    desc.align = (j.at("align").get<std::string_view>() == "left") ? TextSpriteComponent::Align::Left
                                                                   : TextSpriteComponent::Align::Right;
    j.at("spriteSheet").get_to(desc.spriteSheet);
  }

  struct TextSpriteImpl
  {
    TextSpriteComponent::Align align{};
    ComponentReference<SpriteSheetComponent> spriteSheet;

    std::vector<std::shared_ptr<Entity>> sprites;
    std::vector<std::string> text;

    // On crée de nouvelles sprites pour chaque caractère de la
    // chaîne, on les positionne correctement, et on détruit les
    // anciens sprites.
    void UpdateTextSprites(Entity& owner)
    {
      for (auto& s : sprites)
      {
        s->GetParent()->RemoveChild(s);
      }
      sprites.clear();

      std::vector<std::string> characters = text;
      if (align == TextSpriteComponent::Align::Right)
      {
        std::reverse(characters.begin(), characters.end());
      }
      float dir = (align == TextSpriteComponent::Align::Left) ? 1.F : -1.F;

      float offset{};
      int index = 0;
      for (const auto& c : characters)
      {
        const auto* glyph = spriteSheet->GetSprite(c);
        if (glyph == nullptr)
        {
          continue;
        }

        float x = offset;
        offset += float(glyph->sourceSize.width) * dir;

        auto newSpriteObj = Scene::CreateChild<PositionComponent, SpriteComponent>(
            {x, 0.F, 0.F}, {0, false, 0, c, spriteSheet}, fmt::format("{}_{}", fmt::join(text, ""), index),
            owner.shared_from_this());

        sprites.emplace_back(newSpriteObj);

        index++;
      }
    }
  };

  TextSpriteComponent::TextSpriteComponent(Entity& owner) : Component(owner), impl(std::make_unique<TextSpriteImpl>())
  {
  }

  TextSpriteComponent::~TextSpriteComponent() = default;

  Component::SetupResult TextSpriteComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult TextSpriteComponent::Setup(const Description& descr)
  {
    impl->align = descr.align;
    impl->spriteSheet = descr.spriteSheet;
    return {this, {{[this]() { return impl->spriteSheet.Ready(); }, [this]() { impl->UpdateTextSprites(Owner()); }}}};
  }

  void TextSpriteComponent::SetText(std::string_view text)
  {
    std::vector<std::string_view> characters;
    characters.reserve(text.size());
    for (const auto& c : text)
    {
      characters.emplace_back(&c, 1);
    }

    SetCharacters(characters);
  }

  void TextSpriteComponent::SetCharacters(gsl::span<std::string_view> array)
  {
    if (std::equal(impl->text.begin(), impl->text.end(), array.begin(), array.end(),
                   [](const auto& a, const auto& b) { return a == b; }))
    {
      return;
    }

    impl->text.clear();
    std::transform(array.begin(), array.end(), std::back_inserter(impl->text),
                   [](const auto& c) { return std::string(c.data(), c.size()); });

    impl->UpdateTextSprites(Owner());
  }
} // namespace SimpleGE
