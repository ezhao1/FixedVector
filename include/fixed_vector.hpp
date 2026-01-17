#pragma once
#include <cstddef>

namespace fixed_vector
{

template <typename T, size_t N>
class FixedVector
{
public:
    using value_type = T;
    using size_type  = std::size_t;
    using pointer = T*;
    using const_pointer = const T*;

    FixedVector() noexcept : size_(0) {};
    ~FixedVector() {
        // TODO: cleanup logic
        // Go through each element in storage and free it
        for (int i = 0; i < size_; i++) {
            data()[i].~
        }
    };

    void push_back(const T& value) {
        if (size_ == N) {
            throw;
        }

        storage_[size++] = value;
    };
    void pop_back() {
        if (size_ == 0) {
            throw;
        }

        size_--;
    };

    T& operator[](size_type index);
    const T& operator[](size_type index) const;

    size_type size() const noexcept {
        return size_;
    };
    constexpr size_type capacity() const noexcept {
        return N;
    }

    // Iterators
    T* begin() noexcept {
        return reinterpret_cast<T>([0]);
    };
    T* end() noexcept {
        return storage_[size_-1];
    };

private:
    alignas(T) unsigned char storage_[sizeof(T) * N];
    size_type size_;
    pointer data() noexcept {
        return reinterpret_cast<pointer>(storage_);
    }
}

}
