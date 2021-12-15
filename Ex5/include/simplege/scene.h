#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/entity.h>

namespace SimpleGE::Scene
{
  void Load(std::string_view description);
  void Clear();

  template<typename... CompTypes>
  inline std::shared_ptr<Entity> CreateChild(const typename CompTypes::Description&... descr, std::string_view name,
                                             const std::shared_ptr<Entity>& parent,
                                             std::vector<Component::SetupResult>& pendingSetup)
  {
    auto newObj = Entity::Create();
    parent->AddChild(name, newObj);
    (newObj->AddComponent<CompTypes>(descr, pendingSetup), ...);
    return newObj;
  }

  template<typename... CompTypes>
  inline std::shared_ptr<Entity> CreateChild(const typename CompTypes::Description&... descr, std::string_view name,
                                             const std::shared_ptr<Entity>& parent)
  {
    std::vector<Component::SetupResult> pendingSetup;
    auto newObj = CreateChild<CompTypes...>(descr..., name, parent, pendingSetup);
    bool success = Component::Resolve(pendingSetup);
    Ensures(success);
    return newObj;
  }

  std::shared_ptr<Entity> CreateChild(const nlohmann::json& desc, std::string_view name,
                                      const std::shared_ptr<Entity>& parent);

  std::shared_ptr<Entity> FindObject(std::string_view name);
} // namespace SimpleGE::Scene