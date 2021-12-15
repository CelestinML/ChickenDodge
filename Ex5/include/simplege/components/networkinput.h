#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/input.h>
#include <simplege/components/logic.h>
#include <simplege/components/network.h>

namespace SimpleGE
{
  struct NetworkInputImpl;

  class NetworkInputComponent : public NetworkComponent, public InputComponent, public LogicComponent
  {
  public:
    static constexpr auto Type = "NetworkInput";

    struct Description
    {
      ComponentReference<InputComponent> localInput;
    };

    NetworkInputComponent(Entity& owner);
    ~NetworkInputComponent() override;

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const Timing& timing) override;
    void OnMessage(Network::Connection& connection, const BaseMessage& msg) override;
    [[nodiscard]] bool IsActive(Action action) const override;

    void SetLocal(bool isLocal);
    [[nodiscard]] bool IsLocal() const;

  private:
    std::unique_ptr<NetworkInputImpl> impl;
  };
} // namespace SimpleGE
