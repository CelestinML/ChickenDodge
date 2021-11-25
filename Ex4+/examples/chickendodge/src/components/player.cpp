#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/player.h"

#include "chickendodge/components/chicken.h"
#include "chickendodge/components/heart.h"
#include "chickendodge/components/life.h"
#include "chickendodge/components/rupee.h"
#include "chickendodge/components/score.h"

#include <fmt/core.h>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, PlayerComponent::Description& desc)
  {
    j.at("name").get_to(desc.name);
    j.at("input").get_to(desc.input);
    j.at("prefix").get_to(desc.prefix);
    j.at("score").get_to(desc.score);
    j.at("life").get_to(desc.life);
    j.at("gameArea").get_to(desc.gameArea);
    j.at("hurtMotion").get_to(desc.hurtMotion);
    j.at("speed").get_to(desc.speed);
    j.at("onHurtEnable").get_to(desc.onHurtEnable);
    desc.invulnerableDuration = std::chrono::milliseconds{j.at("invulnerableDuration").get<int>()};
    desc.hurtDuration = std::chrono::milliseconds{j.at("hurtDuration").get<int>()};
  }

  enum class Facing
  {
    Back,
    Front,
    Left,
    Right
  };
  static constexpr std::array<std::string_view, 4> FacingDir{"B", "F", "L", "R"};

  struct PlayerImpl
  {
    EventTrigger<PlayerComponent::DeadEventType> deadEvent;
    bool isDead{};
    ComponentReference<ScoreComponent> score;
    std::string name;
    std::string prefix;

    Area gameArea;
    Facing facing{Facing::Front};
    bool isAttacking{};
    bool isMoving{};
    bool isHurt{};
    bool isInvulnerable{};
    std::chrono::milliseconds invulnerableDuration{};
    std::chrono::milliseconds hurtDuration{};
    float hurtMotion{};
    float speed{};
    ComponentReference<InputComponent> input;
    ComponentReference<LifeComponent> life;
    SpriteComponent* sprite{};

    std::unordered_map<int, std::pair<std::chrono::milliseconds, std::function<void()>>> pendingActions;
    int nextPendingAction{};

    Component::SetupResult Setup(PlayerComponent& comp, const PlayerComponent::Description& descr)
    {
      auto& owner = comp.Owner();
      input = descr.input;
      score = descr.score;
      life = descr.life;
      speed = descr.speed;

      return {&comp,
              {{[this]() { return life.Ready(); },
                [this, descr, &owner]()
                {
                  life->RegisterDeadEvent([this]() { OnDead(); });
                  life->RegisterHurtEvent([this, &owner]() { OnHurt(owner); });
                  for (const auto& comp : descr.onHurtEnable)
                  {
                    life->RegisterHurtEvent([comp]() { comp->Enable(); });
                  }
                }},
               {[&owner]() { return owner.GetComponent<SpriteComponent>() != nullptr; },
                [this, &owner]()
                {
                  sprite = owner.GetComponent<SpriteComponent>();
                  Ensures(sprite != nullptr);
                  sprite->RegisterAnimationEndedEvent(
                      [this]()
                      {
                        isAttacking = false;
                        sprite->SetFrameSkip(2);
                        UpdateSprite();
                        sprite->UpdateMesh();
                      });

                  UpdateSprite();
                }}}};
    }

    void UpdateLogic(Entity& owner, const Timing& timing)
    {
      UpdatePendingActions(timing);

      Vector<3> delta;
      if (isDead)
      {
        delta = UpdateDead(owner);
      }
      else if (isHurt)
      {
        delta = UpdateHurt(owner);
      }
      else
      {
        delta = UpdateStandard(owner);
      }

      bool visible = (!isInvulnerable) || (timing.frame % 2 == 0);
      sprite->SetEnabled(visible);

      gsl::not_null<PositionComponent*> position = owner.GetComponent<PositionComponent>();

      delta *= std::chrono::duration_cast<std::chrono::duration<float>>(timing.delta).count();
      position->Translate(delta);
      position->Clamp(Point<2>({gameArea.xMin(), gameArea.yMin()}), Point<2>({gameArea.xMax(), gameArea.yMax()}));
    }

    void UpdatePendingActions(const Timing& timing)
    {
      using namespace std::chrono_literals;

      std::vector<int> elapsed;
      for (auto& pending : pendingActions)
      {
        auto actionId = pending.first;
        auto& remaining = pending.second.first;
        const auto& action = pending.second.second;

        remaining -= timing.delta;

        if (remaining <= 0ms)
        {
          elapsed.push_back(actionId);
          action();
        }
      }

      for (auto actionId : elapsed)
      {
        pendingActions.erase(actionId);
      }
    }

    void OnDead()
    {
      isDead = true;
      deadEvent.Trigger();
    }

    void OnHurt(Entity& owner)
    {
      gsl::not_null<ColliderComponent*> collider = owner.GetComponent<ColliderComponent>();

      isHurt = true;
      pendingActions[nextPendingAction++] = {hurtDuration, [this]() { isHurt = false; }};

      isInvulnerable = true;
      collider->Disable();
      pendingActions[nextPendingAction++] = {invulnerableDuration, [this, collider]()
                                             {
                                               isInvulnerable = false;
                                               collider->Enable();
                                             }};
    }

    void UpdateSprite()
    {
      using namespace std::literals::string_view_literals;

      bool isAnimated = isMoving || isAttacking;
      sprite->SetIsAnimated(isAnimated);
      std::string_view mod = isAttacking ? "A"sv : "M"sv;
      std::string_view frame = isAnimated ? ""sv : "1"sv;

      sprite->SetSpriteName(fmt::format("{}{}{}{}", prefix, mod, gsl::at(FacingDir, (int) facing), frame));
    }

    Vector<3> UpdateDead(Entity& owner)
    {
      isMoving = false;
      isAttacking = false;
      sprite->SetIsAnimated(false);
      sprite->SetSpriteName(fmt::format("{}D", prefix));
      sprite->UpdateMesh();

      gsl::not_null<ColliderComponent*> collider = owner.GetComponent<ColliderComponent>();
      collider->Disable();

      return {};
    }

    Vector<3> UpdateHurt(Entity& owner)
    {
      isMoving = false;
      isAttacking = false;
      sprite->SetIsAnimated(false);
      sprite->SetSpriteName(fmt::format("{}H{}", prefix, gsl::at(FacingDir, (int) facing)));
      sprite->UpdateMesh();

      Vector<3> delta{};

      switch (facing)
      {
      case Facing::Back:
        delta[1] = hurtMotion;
        break;
      case Facing::Front:
        delta[1] = -hurtMotion;
        break;
      case Facing::Left:
        delta[0] = hurtMotion;
        break;
      case Facing::Right:
        delta[0] = -hurtMotion;
        break;
      }
      return delta;
    }

    Vector<3> UpdateStandard(Entity& owner)
    {
      if (!isAttacking && input->IsActive(InputComponent::Action::Attack))
      {
        isAttacking = true;
        sprite->SetAnimationFrame(1);
        sprite->SetFrameSkip(1);
      }

      Vector<3> delta{};

      if (input->IsActive(InputComponent::Action::Up))
      {
        delta[1]--;
        facing = Facing::Back;
      }
      if (input->IsActive(InputComponent::Action::Down))
      {
        delta[1]++;
        facing = Facing::Front;
      }
      if (input->IsActive(InputComponent::Action::Left))
      {
        delta[0]--;
        facing = Facing::Left;
      }
      if (input->IsActive(InputComponent::Action::Right))
      {
        delta[0]++;
        facing = Facing::Right;
      }

      delta *= speed;

      isMoving = delta.MagnitudeSq() > 0;

      UpdateSprite();
      sprite->UpdateMesh();

      return delta;
    }
  };

  PlayerComponent::PlayerComponent(Entity& owner) : Component(owner), impl(std::make_unique<PlayerImpl>()) {}

  PlayerComponent::~PlayerComponent() = default;

  Component::SetupResult PlayerComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult PlayerComponent::Setup(const Description& descr)
  {
    impl->name = Localisation::GetContext(descr.name);
    impl->prefix = descr.prefix;
    impl->gameArea = descr.gameArea;
    impl->invulnerableDuration = descr.invulnerableDuration;
    impl->hurtDuration = descr.hurtDuration;
    impl->hurtMotion = descr.hurtMotion;
    return impl->Setup(*this, descr);
  }

  void PlayerComponent::UpdateLogic(const Timing& timing) { impl->UpdateLogic(Owner(), timing); }

  void PlayerComponent::OnCollision(const SimpleGE::ColliderComponent& other)
  {
    auto& obj = other.Owner();
    RupeeComponent* rupee = obj.GetComponent<RupeeComponent>();
    HeartComponent* heart = obj.GetComponent<HeartComponent>();
    ChickenComponent* chicken = obj.GetComponent<ChickenComponent>();

    if (rupee != nullptr)
    {
      impl->score->Increment(rupee->GetValue());
      obj.GetParent()->RemoveChild(obj);
    }
    if (heart != nullptr)
    {
      impl->life->Increment(heart->GetHeal());
      obj.GetParent()->RemoveChild(obj);
    }
    if (chicken != nullptr)
    {
      if (impl->isAttacking)
      {
        chicken->OnAttack();
      }
      else
      {
        impl->life->Decrement(chicken->GetAttack());
      }
    }
  }

  std::string_view PlayerComponent::GetName() const { return impl->name; }

  bool PlayerComponent::IsDead() const { return impl->isDead; }

  const ComponentReference<ScoreComponent>& PlayerComponent::GetScore() const { return impl->score; }

  void PlayerComponent::RegisterDeadEvent(const DeadEventType& onDead) { impl->deadEvent.Register(onDead); }
} // namespace ChickenDodge
