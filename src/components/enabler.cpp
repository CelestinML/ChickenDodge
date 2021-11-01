#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, EnablerComponent::Description& desc)
  {
    desc.onStart.clear();
    for (const auto& entry : j.at("onStart").get<std::unordered_map<std::string, bool>>())
    {
      desc.onStart.emplace_back(entry);
    }
    desc.onEvent.clear();
    for (const auto& entry : j.at("onEvent").get<std::unordered_map<std::string, bool>>())
    {
      desc.onEvent.emplace_back(entry);
    }
  }

  struct EnablerImpl
  {
    decltype(EnablerComponent::Description::onEvent) onEvent;
  };

  EnablerComponent::EnablerComponent(Entity& owner) : Component(owner), impl(std::make_unique<EnablerImpl>()) {}

  EnablerComponent::~EnablerComponent() = default;

  Component::SetupResult EnablerComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult EnablerComponent::Setup(const Description& descr)
  {
    impl->onEvent = descr.onEvent;

    return {this,
            {{[descr]()
              {
                return std::all_of(descr.onStart.begin(), descr.onStart.end(),
                                   [](const auto& entry) { return entry.first.Ready(); });
              },
              [descr]()
              {
                for (const auto& entry : descr.onStart)
                {
                  entry.first->SetEnabled(entry.second);
                }
              }}}};
  }

  void EnablerComponent::OnEvent()
  {
    for (const auto& entry : impl->onEvent)
    {
      entry.first->SetEnabled(entry.second);
    }
  }
} // namespace SimpleGE