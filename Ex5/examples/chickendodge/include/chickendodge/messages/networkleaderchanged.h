#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct NetworkLeaderChanged : public SimpleGE::Message<NetworkLeaderChanged>
  {
    static constexpr MessageType Type = FirstGameType + 4;

    int bestPlayer;
    int bestScore;

    NetworkLeaderChanged() = default;
    NetworkLeaderChanged(int bestPlayer, int bestScore) : bestPlayer(bestPlayer), bestScore(bestScore) {}

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      serializer.Write(bestPlayer);
      serializer.Write(bestScore);
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      deserializer.Read(bestPlayer);
      deserializer.Read(bestScore);
    }
  };
} // namespace ChickenDodge