#pragma once

#include <vector>
#include <cassert>
#include <cmath>
#include <stdexcept>

template <class T, int SIZE>
class Vector {
public:
    Vector(std::initializer_list<T> il) : mData(std::move(il))
    {
        assert(SIZE == mData.size());
    }

    Vector(std::vector<T> data) : mData(std::move(data))
    {
        assert(SIZE == mData.size());
    }

    Vector() : mData(SIZE)
    {
    }

    const T& operator[](int i) const
    {
        return mData[i];
    }

    T& operator[](int i)
    {
        return mData[i];
    }

    T& r()
    {
        return mData[0];
    }

    const T& r() const
    {
        return mData[0];
    }

    T& g()
    {
        return mData[1];
    }

    const T& g() const
    {
        return mData[1];
    }

    T& b()
    {
        return mData[2];
    }

    const T& b() const
    {
        return mData[2];
    }

    T& a()
    {
        return mData[3];
    }

    const T& a() const
    {
        return mData[3];
    }

    T& x()
    {
        return mData[0];
    }

    const T& x() const
    {
        return mData[0];
    }

    T& y()
    {
        return mData[1];
    }

    const T& y() const
    {
        return mData[1];
    }

    T& z()
    {
        return mData[2];
    }

    const T& z() const
    {
        return mData[2];
    }

    T& w()
    {
        return mData[3];
    }

    const T& w() const
    {
        return mData[3];
    }

    float length() const
    {
        float len = 0.f;
        for (const auto& item : mData)
        {
            len += item*item;
        }
        len=sqrtf(len);
        return len;
    }

    size_t size() const
    {
        return mData.size();
    }

    void normalize()
    {
        constexpr float epsilon = 1.0e-4;
        const auto len = length();
        if ( len > epsilon ) {
            for (auto& item : mData)
            {
                item /= len;
            }
        }
        else
        {
            throw std::runtime_error("error");
        }
    }

    Vector<T, SIZE>& operator=(const Vector<T, SIZE>& other) = default;

    Vector<T, SIZE>& operator*=(T scalar)
    {
        *this = *this * scalar;
        return *this;
    }

    Vector<T, SIZE>& operator/=(T scalar)
    {
        if (scalar == (T)0)
        {
            throw std::runtime_error("division by zero");
        }
        *this = *this / scalar;
        return *this;
    }

    Vector<T, SIZE>& operator-=(T scalar)
    {
        *this = *this - scalar;
        return *this;
    }

    Vector<T, SIZE>& operator+=(T scalar)
    {
        *this = *this + scalar;
        return *this;
    }

    Vector<T, SIZE>& operator+=(const Vector<T, SIZE>& other)
    {
        *this = *this + other;
        return *this;
    }

    Vector<T, SIZE>& operator-=(const Vector<T, SIZE>& other)
    {
        *this = *this - other;
        return *this;
    }

    Vector<T, SIZE>& operator*=(const Vector<T, SIZE>& other)
    {
        *this = *this * other;
        return *this;
    }

    Vector<T, SIZE>& operator/=(const Vector<T, SIZE>& other)
    {
        for (const auto& otherItem : other)
        {
            if (otherItem == (T)0)
            {
                throw std::runtime_error("division by zero");
            }
        }
        *this = *this / other;
        return *this;
    }

    Vector<T, SIZE> operator*(T scalar) const
    {
        auto dataCopy = mData;
        for (auto& item : dataCopy)
        {
            item *= scalar;
        }
        return {dataCopy};
    }

    Vector<T, SIZE> operator+(T scalar) const
    {
        auto dataCopy = mData;
        for (auto& item : dataCopy)
        {
            item += scalar;
        }
        return {dataCopy};
    }

    Vector<T, SIZE> operator-(T scalar) const
    {
        auto dataCopy = mData;
        for (auto& item : dataCopy)
        {
            item -= scalar;
        }
        return {dataCopy};
    }

    void negate()
    {
        *this=*this*((T)(-1));
    }

    Vector<T, SIZE> operator/(T scalar) const
    {
        if (scalar == (T)0)
        {
            throw std::runtime_error("division by zero");
        }
        return *this*((T)(1)/scalar);
    }

    Vector<T, SIZE> operator+(const Vector<T, SIZE>& other) const
    {
        auto dataCopy = mData;
        for (int i = 0; i < dataCopy.size(); i++)
        {
            dataCopy[i] += other[i];
        }
        return {dataCopy};
    }

    Vector<T, SIZE> operator-(const Vector<T, SIZE>& other) const
    {
        auto dataCopy = mData;
        for (int i = 0; i < dataCopy.size(); i++)
        {
            dataCopy[i] -= other[i];
        }
        return {dataCopy};
    }

//    Vector<T, SIZE> operator*(const Vector<T, SIZE>& other) const
//    {
//        auto dataCopy = mData;
//        for (int i = 0; i < dataCopy.size(); i++)
//        {
//            dataCopy[i] *= other[i];
//        }
//        return {dataCopy};
//    }
//
//    Vector<Vector<T, SIZE>, W> operator*(const Vector<Vector<T, K>, W>& other) const
//    {
//        Vector<Vector<T, 4>, 4> result;
//        for (int i = 0; i < mData.size(); i++)
//        {
//            result[i] += this->operator()[i] * other[i];
//        }
//        return result;
//    }

    Vector<T, SIZE> operator*(const Vector<T, SIZE>& other) const
    {
        Vector<T, SIZE> result;
        for (int j = 0; j < SIZE; j++)
        {
            for (int i = 0; i < size(); i++)
            {
                result[j] += mData[i] * other[j][i];
            }
        }
        return result;
    }

    /*
     * wektor wierszowy, macierz row-major
     */
    template <int N>
    Vector<T, N> operator*(const Vector<Vector<T, N>, SIZE>& other) const
    {
        Vector<T, N> result;
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < size(); i++)
            {
                result[j] += mData[i] * other[i][j];
            }
        }
        return result;
    }

    /*
     * wektor wierszowy, macierz row-major
     */
    template <int N>
    Vector<T, SIZE>& operator*=(const Vector<Vector<T, N>, SIZE>& other)
    {
        *this = *this * other;
        return *this;
    }

    Vector<T, SIZE> operator/(const Vector<T, SIZE>& other) const
    {
        for (const auto& otherItem : other)
        {
            if (otherItem == (T)0)
            {
                throw std::runtime_error("division by zero");
            }
        }

        auto dataCopy = mData;
        for (int i = 0; i < dataCopy.size(); i++)
        {
            dataCopy[i] /= other[i];
        }
        return {dataCopy};
    }

//    Vector Vector::crossProduct(const Vector<T, SIZE>& other) const {
//        return {mY * other.mZ - mZ * other.mY, mZ * other.mX - mX * other.mZ, mX * other.mY - mY * other.mX};
//    }

    T dotProduct(const Vector<T, SIZE>& other) const {
        T sum = 0;
        for (int i = 0; i < mData.size(); i++)
        {
           sum += mData[i] * other[i];
        }
        return sum;
    }

private:
    std::vector<T> mData;
};

using float3 = Vector<float, 3>;
using float4 = Vector<float, 4>;
using int3 = Vector<int, 3>;
using float4x4 = Vector<Vector<float, 4>, 4>;

float3 crossProduct(const float3& firstVector, const float3& secondVector);