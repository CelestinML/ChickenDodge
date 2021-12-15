#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct NetworkScore : public SimpleGE::Message<NetworkScore>
  {
    static constexpr MessageType Type = FirstGameType + 3;

    int score;

    NetworkScore() = default;
    NetworkScore(int score) : score(score) {}

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      serializer.Write(score);
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      deserializer.Read(score);
    }
  };
} // namespace ChickenDodge