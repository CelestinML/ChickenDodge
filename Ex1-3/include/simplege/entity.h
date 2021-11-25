#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class Component;

  class Entity : public std::enable_shared_from_this<Entity>
  {
  public:
    static std::shared_ptr<Entity> Create() { return std::make_shared<Entity>(); }

    void AddChild(std::string_view name, const std::shared_ptr<Entity>& child);
    void RemoveChild(std::shared_ptr<Entity> child);
    void RemoveChild(Entity& child) { RemoveChild(child.shared_from_this()); }

    gsl::not_null<Component*> AddComponent(std::string_view type, const nlohmann::json& desc,
                                           std::vector<Component::SetupResult>& pendingSetup);

    template<typename T>
    gsl::not_null<T*> AddComponent(const typename T::Description& descr,
                                   std::vector<Component::SetupResult>& pendingSetup)
    {
      auto result = components.emplace(T::Type, std::make_unique<T>(*this));
      auto iterator = result.first;
      gsl::not_null<T*> comp = dynamic_cast<T*>(iterator->second.get());

      pendingSetup.emplace_back(comp->Setup(descr));

      return comp;
    }

    template<typename T>
    [[nodiscard]] T* GetComponent() const
    {
      return dynamic_cast<T*>(GetComponent(T::Type));
    }

    [[nodiscard]] Component* GetComponent(std::string_view type) const
    {
      auto it = components.find((std::string) type);
      if (it == components.end())
      {
        return {};
      }

      return it->second.get();
    }

    [[nodiscard]] bool Active() const { return active; }

    void SetActive(bool active) { this->active = active; }

    [[nodiscard]] std::shared_ptr<Entity> GetChild(std::string_view name) const
    {
      const auto found = childrenByName.find(std::string{name});
      if (found != childrenByName.end())
      {
        return found->second.lock();
      }

      return {};
    }

    [[nodiscard]] std::shared_ptr<Entity> GetParent() const { return parent.lock(); }

    void Clear();

    [[nodiscard]] gsl::span<std::shared_ptr<Entity>> GetChildren() { return children; }

  private:
    bool active{true};
    std::weak_ptr<Entity> parent;
    std::vector<std::shared_ptr<Entity>> children;
    std::map<std::string, std::weak_ptr<Entity>> childrenByName;
    std::unordered_map<std::string, std::unique_ptr<Component>> components;
  };
} // namespace SimpleGE