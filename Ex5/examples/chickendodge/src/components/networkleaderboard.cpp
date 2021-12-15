#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/networkleaderboard.h"
#include "chickendodge/messages/networkleaderchanged.h"

#include <mutex>
#include <thread>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  using namespace std::chrono_literals;

  struct NetworkLeaderboardImpl;

  // Cette structure met à jour un score fictif afin de valider le
  // fonctionnement du système. À effacer lorsque l'implémentation
  // est complète.
  struct DebugLeaderboardTest
  {
    DebugLeaderboardTest(NetworkLeaderboardImpl& owner, std::string_view name, int score,
                         std::chrono::milliseconds freq)
        : owner(owner), name(name), score(score), freq(freq), thread([this]() { Run(); })
    {
    }

    ~DebugLeaderboardTest()
    {
      done = true;
      thread.join();
    }

    inline void Run();

    NetworkLeaderboardImpl& owner;
    std::string name;
    int score;
    std::chrono::milliseconds freq;

    std::thread thread;
    bool done{};
  };

  struct NetworkLeaderboardImpl
  {
    static constexpr auto LeaderboardSize = 10;
    std::unordered_map<std::string, int> scores;
    std::mutex mutex;

    void SetScore(const std::string& name, int score)
    {
      std::lock_guard<std::mutex> lock(mutex);
      scores[name] = score;
      Display();
    }

    void Display()
    {
      std::vector<std::pair<std::string, int>> sorted(scores.begin(), scores.end());
      std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
      fmt::print("{0:=^{1}}\n", " Leaderboard ", 30);
      for (int i = 0; i < LeaderboardSize && i < sorted.size(); i++)
      {
        fmt::print("  {:<15}: {:>5}\n", sorted[i].first, sorted[i].second);
      }
      fmt::print("{0:=^{1}}\n\n", "", 30);
    }

    // TODO: À enlever lorsque l'implémentation est complète
    DebugLeaderboardTest debug1 = {*this, "Test 1", 1234, 5s};
    DebugLeaderboardTest debug2 = {*this, "Test 2", 750, 3s};
  };

  // TODO: À enlever lorsque l'implémentation est complète
  inline void DebugLeaderboardTest::Run()
  {
    done = false;
    while (!done)
    {
      owner.SetScore(name, score);
      score += 250;
      std::this_thread::sleep_for(freq);
    }
  }

  NetworkLeaderboardComponent::NetworkLeaderboardComponent(Entity& owner)
      : Component(owner), impl(std::make_unique<NetworkLeaderboardImpl>())
  {
  }

  NetworkLeaderboardComponent::~NetworkLeaderboardComponent() = default;

  void NetworkLeaderboardComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    if (msg.Is<NetworkLeaderChanged>()) {
      NetworkLeaderChanged bestPlayerScore = msg.Get<NetworkLeaderChanged>();
      fmt::print("Le joueur {} est en tête avec un score de {}\n", bestPlayerScore.bestPlayer, bestPlayerScore.bestScore);
    }
  }
} // namespace ChickenDodge
