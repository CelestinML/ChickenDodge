#include <simplege/simplege.h>

#include "buffer.h"

namespace SimpleGE::Graphic
{
  template<BufferType Type>
  Buffer<Type>::Buffer(Usage usage) : impl(std::make_unique<BufferImpl<Type>>(usage))
  {
  }

  template Buffer<BufferType::Vertices>::Buffer(Usage usage);
  template Buffer<BufferType::Indices>::Buffer(Usage usage);

  template<BufferType Type>
  Buffer<Type>::~Buffer() = default;

  template Buffer<BufferType::Vertices>::~Buffer();
  template Buffer<BufferType::Indices>::~Buffer();

  template<BufferType Type>
  Buffer<Type>::operator unsigned int() const
  {
    return impl->buffer;
  }

  // Issue resolving templated cast concretisation with MSVC
#ifndef _MSC_VER
  template Buffer<BufferType::Vertices>::operator unsigned int() const;
  template Buffer<BufferType::Indices>::operator unsigned int() const;
#else
  template<>
  Buffer<BufferType::Vertices>::operator unsigned int() const
  {
    return impl->buffer;
  }

  template<>
  Buffer<BufferType::Indices>::operator unsigned int() const
  {
    return impl->buffer;
  }
#endif

  template<BufferType Type>
  void Buffer<Type>::SetBytes(gsl::span<const char> data)
  {
    impl->Set(data);
  }

  template void Buffer<BufferType::Vertices>::SetBytes(gsl::span<const char> data);
  template void Buffer<BufferType::Indices>::SetBytes(gsl::span<const char> data);

  template<BufferType Type>
  void Buffer<Type>::UpdateBytes(gsl::span<const char> data, std::ptrdiff_t offset)
  {
    impl->Update(data, offset);
  }

  template void Buffer<BufferType::Vertices>::UpdateBytes(gsl::span<const char> data, std::ptrdiff_t offset);
  template void Buffer<BufferType::Indices>::UpdateBytes(gsl::span<const char> data, std::ptrdiff_t offset);
} // namespace SimpleGE::Graphic