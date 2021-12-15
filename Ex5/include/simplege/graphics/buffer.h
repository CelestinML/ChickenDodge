#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE::Graphic
{
  enum class BufferType
  {
    Vertices,
    Indices,
  };

  template<BufferType Type>
  struct BufferImpl;

  template<BufferType Type>
  class Buffer
  {
  public:
    enum class Usage
    {
      Static,
      Dynamic,
    };

    template<typename T>
    static std::shared_ptr<Buffer<Type>> Create(gsl::span<const T> data, Usage usage)
    {
      return std::make_shared<Buffer<Type>>(data, usage);
    }

    template<typename T>
    Buffer(gsl::span<const T> data, Usage usage) : Buffer(usage)
    {
      SetBytes(gsl::span<const char>{reinterpret_cast<const char*>(data.data()), data.size_bytes()});
    }

    ~Buffer();

    [[nodiscard]] BufferImpl<Type>& GetImpl() { return *impl; }

    template<typename T>
    void Update(gsl::span<const T> data, std::ptrdiff_t offset = 0)
    {
      UpdateBytes(gsl::span<const char>{reinterpret_cast<const char*>(data.data()), data.size_bytes()},
                  offset * sizeof(T));
    }

    operator unsigned int() const;

  private:
    explicit Buffer(Usage usage);

    void SetBytes(gsl::span<const char> data);
    void UpdateBytes(gsl::span<const char> data, std::ptrdiff_t offset);

    std::unique_ptr<BufferImpl<Type>> impl;
  };

  using VertexBuffer = Buffer<BufferType::Vertices>;
  using IndexBuffer = Buffer<BufferType::Indices>;
} // namespace SimpleGE::Graphic