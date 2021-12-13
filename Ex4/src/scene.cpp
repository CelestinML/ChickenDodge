#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE::Scene
{
  struct EntityDesc;
  using ComponentDesc = std::map<std::string, json>;
  using SceneDesc = std::map<std::string, std::unique_ptr<EntityDesc>>;

  struct EntityDesc
  {
    static constexpr auto ComponentField = "components";
    static constexpr auto ChildrenField = "children";

    EntityDesc(const json& desc)
    {
      if (desc.contains(ComponentField))
      {
        desc.at(ComponentField).get_to(components);
      }
      if (desc.contains(ChildrenField))
      {
        desc.at(ChildrenField).get_to(children);
      }
    }

    ComponentDesc components;
    SceneDesc children;
  };

  static void from_json(const json& j, SceneDesc& desc)
  {
    for (const auto& elem : j.items())
    {
      desc.emplace(elem.key(), std::make_unique<EntityDesc>(elem.value()));
    }
  }

  std::shared_ptr<Entity> RootEntity()
  {
    static auto root = Entity::Create();
    return root;
  }

  void CreateChildren(const SceneDesc& desc, const std::shared_ptr<Entity>& parent,
                      std::vector<Component::SetupResult>& pendingSetup);

  std::shared_ptr<Entity> CreateChildInternal(std::string_view name, const EntityDesc& desc,
                                              const std::shared_ptr<Entity>& parent,
                                              std::vector<Component::SetupResult>& pendingSetup)
  {
    auto newObj = Entity::Create();
    parent->AddChild(name, newObj);
    CreateChildren(desc.children, newObj, pendingSetup);

    for (const auto& item : desc.components)
    {
      auto newComp = newObj->AddComponent(item.first, item.second, pendingSetup);
    }
    return newObj;
  }

  void CreateChildren(const SceneDesc& desc, const std::shared_ptr<Entity>& parent,
                      std::vector<Component::SetupResult>& pendingSetup)
  {
    for (const auto& item : desc)
    {
      CreateChildInternal(item.first, *item.second, parent, pendingSetup);
    }
  }

  std::shared_ptr<Entity> CreateChild(const nlohmann::json& desc, std::string_view name,
                                      const std::shared_ptr<Entity>& parent)
  {
    EntityDesc descr(desc);
    std::vector<Component::SetupResult> pendingSetup;
    auto newObj = CreateChildInternal(name, descr, parent, pendingSetup);
    bool success = Component::Resolve(pendingSetup);
    Ensures(success);
    return newObj;
  }

  void Load(std::string_view description)
  {
    json desc = json::parse(description);
    SceneDesc sceneDesc = desc.get<SceneDesc>();

    std::vector<Component::SetupResult> pendingSetup;
    CreateChildren(sceneDesc, RootEntity(), pendingSetup);
    bool success = Component::Resolve(pendingSetup);
    Ensures(success);
  }

  std::shared_ptr<Entity> FindObjectRecursive(const std::shared_ptr<Entity>& parent, std::string_view name)
  {
    auto found = parent->GetChild(name);
    if (found != nullptr)
    {
      return found;
    }

    for (const auto& obj : parent->GetChildren())
    {
      found = FindObjectRecursive(obj, name);
      if (found)
      {
        return found;
      }
    }

    return {};
  }

  std::shared_ptr<Entity> FindObject(std::string_view name) { return FindObjectRecursive(RootEntity(), name); }

  void Clear() { RootEntity()->Clear(); }
} // namespace SimpleGE::Scene