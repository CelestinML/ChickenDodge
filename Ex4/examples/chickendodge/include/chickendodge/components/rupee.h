#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct RupeeImpl;

  class RupeeComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Rupee";

    struct Description
    {
      std::unordered_map<std::string, int> values;
      std::chrono::milliseconds lifetime;
    };

    RupeeComponent(SimpleGE::Entity& owner);
    ~RupeeComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

    int GetValue() const;

  private:
    std::unique_ptr<RupeeImpl> impl;
  };
} // namespace ChickenDodge