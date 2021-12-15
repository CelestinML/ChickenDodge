#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  template<std::size_t W>
  struct Vector;

  template<std::size_t W>
  struct Point
  {
    static Point<W> Origin() { return {}; }

    Point() = default;

    Point(const std::array<float, W>& val) : point(val) {}

    template<std::size_t W2>
    explicit Point(const Point<W2>& other)
    {
      for (int i = 0; i < std::min(W, W2); i++)
      {
        operator[](i) = other[i];
      }
    }

    explicit operator const float*() const { return point.data(); }

    float& operator[](int idx) { return gsl::at(point, idx); }

    float operator[](int idx) const { return gsl::at(point, idx); }

    inline Point<W>& operator+=(const Vector<W>& rhs);

    friend Point<W> operator+(const Vector<W>& lhs, const Point<W>& rhs) { return rhs + lhs; }

    std::array<float, W> point{};
  };

  template<std::size_t W>
  struct Vector
  {
    Vector() = default;

    Vector(const std::array<float, W>& val) : vector(val) {}

    template<std::size_t W2>
    explicit Vector(const Vector<W2>& other)
    {
      for (int i = 0; i < std::min(W, W2); i++)
      {
        operator[](i) = other[i];
      }
    }

    explicit operator const float*() const { return vector.data(); }

    float& operator[](int idx) { return gsl::at(vector, idx); }

    float operator[](int idx) const { return gsl::at(vector, idx); }

    Vector<W>& operator+=(const Vector<W>& rhs)
    {
      for (int i = 0; i < W; i++)
      {
        operator[](i) += rhs[i];
      }
      return *this;
    }

    friend Vector<W> operator+(Vector<W> lhs, const Vector<W>& rhs)
    {
      lhs += rhs;
      return lhs;
    }

    Vector<W>& operator*=(float scale)
    {
      for (int i = 0; i < W; i++)
      {
        operator[](i) *= scale;
      }
      return *this;
    }

    friend Vector<W> operator*(Vector<W> lhs, float scale)
    {
      lhs *= scale;
      return lhs;
    }

    [[nodiscard]] float MagnitudeSq() const
    {
      float sum{};
      for (int i = 0; i < W; i++)
      {
        sum += operator[](i) * operator[](i);
      }
      return sum;
    }

    [[nodiscard]] float Magnitude() const { return std::sqrt(MagnitudeSq()); }

    void Normalize()
    {
      float magnitude = Magnitude();
      Ensures(magnitude != 0.F);

      for (int i = 0; i < W; i++)
      {
        operator[](i) /= magnitude;
      }
    }

    std::array<float, W> vector{};
  };

  template<std::size_t W>
  inline Point<W>& Point<W>::operator+=(const Vector<W>& rhs)
  {
    for (int i = 0; i < W; i++)
    {
      operator[](i) += rhs[i];
    }
    return *this;
  }

  template<std::size_t W>
  static inline Point<W> operator+(Point<W> lhs, const Vector<W>& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  template<std::size_t W>
  static inline Vector<W> operator-(const Point<W>& lhs, const Point<W>& rhs)
  {
    Vector<W> vec;
    for (int i = 0; i < W; i++)
    {
      vec[i] = lhs[i] - rhs[i];
    }
    return vec;
  }

  template<std::size_t W, std::size_t H>
  struct Matrix
  {
    Matrix<W, H>& LoadIdentity()
    {
      for (int y = 0; y < H; y++)
      {
        for (int x = 0; x < W; x++)
        {
          matrix[y][x] = (x == y) ? 1.F : 0.F;
        }
      }

      return *this;
    }

    Matrix<W, H>& Ortho(float left, float right, float bottom, float top, float nearClip, float farClip)
    {
      static_assert(W == 4 && H == 4, "Ortho seulement implémenté pour les matrices 4x4");
      // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
      matrix[0][0] = 2.F / (right - left);
      matrix[0][1] = 0.F;
      matrix[0][2] = 0.F;
      matrix[0][3] = 0.F;

      matrix[1][0] = 0.F;
      matrix[1][1] = 2.F / (top - bottom);
      matrix[1][2] = 0.F;
      matrix[1][3] = 0.F;

      matrix[2][0] = 0.F;
      matrix[2][1] = 0.F;
      matrix[2][2] = -2.F / (farClip - nearClip);
      matrix[2][3] = 0.F;

      matrix[3][0] = -(right + left) / (right - left);
      matrix[3][1] = -(top + bottom) / (top - bottom);
      matrix[3][2] = -(farClip + nearClip) / (farClip - nearClip);
      matrix[3][3] = 1.F;
      // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

      return *this;
    }

    [[nodiscard]] gsl::span<const float, W * H> Flat() const
    {
      return gsl::span<const float, W * H>(matrix.data()->data(), W * H);
    }

    explicit operator const float*() const { return Flat().data(); }

    std::array<std::array<float, W>, H> matrix{};
  };

  template<typename T>
  struct Size
  {
    T width;
    T height;
  };

  struct Area
  {
    Area() = default;

    Area(float x, float y, float width, float height) : position({x, y}), size({width, height}) {}

    [[nodiscard]] float x() const { return position[0]; }
    [[nodiscard]] float y() const { return position[1]; }
    [[nodiscard]] float width() const { return size.width; }
    [[nodiscard]] float height() const { return size.height; }

    [[nodiscard]] float xMin() const { return x() - width() / 2.F; }
    [[nodiscard]] float xMax() const { return x() + width() / 2.F; }
    [[nodiscard]] float yMin() const { return y() - height() / 2.F; }
    [[nodiscard]] float yMax() const { return y() + height() / 2.F; }

    [[nodiscard]] bool Intersects(const Area& other) const
    {
      return !((xMin() >= other.xMax()) || (xMax() <= other.xMin()) || (yMin() >= other.yMax()) ||
               (yMax() <= other.yMin()));
    }

    Point<2> position{};
    Size<float> size{};
  };
} // namespace SimpleGE