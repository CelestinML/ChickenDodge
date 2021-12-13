#include <simplege/simplege.h>

#include <simplege/messages/networkinputchanged.h>

using json = nlohmann::json;

namespace SimpleGE
{
  struct NetworkInputImpl
  {
    ComponentReference<InputComponent> localInput;
    bool isLocal{true};
    std::array<bool, (int) NetworkInputComponent::Action::Count> inputStatus{};

    void UpdateLocal()
    {
      bool changed = false;
      for (int k = 0; k < inputStatus.size(); k++)
      {
        bool newVal = localInput->IsActive((NetworkInputComponent::Action) k);
        if (newVal != gsl::at(inputStatus, k))
        {
          changed = true;
          gsl::at(inputStatus, k) = newVal;
        }
      }

      if (changed)
      {
        NetworkInputChanged msg(inputStatus);
        NetworkSystem::Send(msg);
      }
    }

    void UpdateRemote(const NetworkInputChanged& msg)
    {
      std::copy(msg.symbols.begin(), msg.symbols.end(), inputStatus.begin());
    }
  };

  static void from_json(const json& j, NetworkInputComponent::Description& desc)
  {
    j.at("localInput").get_to(desc.localInput);
  }

  NetworkInputComponent::NetworkInputComponent(Entity& owner)
      : Component(owner), impl(std::make_unique<NetworkInputImpl>())
  {
  }

  NetworkInputComponent::~NetworkInputComponent() = default;

  Component::SetupResult NetworkInputComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult NetworkInputComponent::Setup(const Description& descr)
  {
    impl->localInput = descr.localInput;

    return {this, {}};
  }

  void NetworkInputComponent::UpdateLogic(const Timing& timing)
  {
    if (IsLocal())
    {
      impl->UpdateLocal();
    }
  }

  void NetworkInputComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    if (IsLocal())
    {
      return;
    }

    if (msg.Is<NetworkInputChanged>())
    {
      impl->UpdateRemote(msg.Get<NetworkInputChanged>());
    }
  }

  [[nodiscard]] bool NetworkInputComponent::IsActive(Action action) const
  {
    return gsl::at(impl->inputStatus, (int) action);
  }

  void NetworkInputComponent::SetLocal(bool isLocal) { impl->isLocal = isLocal; }

  bool NetworkInputComponent::IsLocal() const { return impl->isLocal; }
} // namespace SimpleGE
