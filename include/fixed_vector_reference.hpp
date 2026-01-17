#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <new>
#include <type_traits>
#include <iterator>
#include <byte>

template <typename T, std::size_t N>
class FixedVector {
public:
    // ===== Standard container type aliases =====
    using value_type      = T;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using iterator        = T*;
    using const_iterator  = const T*;

    // ===== Construction / Destruction =====
    constexpr FixedVector() noexcept = default;

    FixedVector(const FixedVector& other) {
        for (size_type i = 0; i < other.size_; ++i) {
            emplace_back(other[i]);
        }
    }

    FixedVector& operator=(const FixedVector& other) {
        if (this == &other) return *this;
        clear();
        for (size_type i = 0; i < other.size_; ++i) {
            emplace_back(other[i]);
        }
        return *this;
    }

    FixedVector(FixedVector&& other) noexcept(
        std::is_nothrow_move_constructible_v<T>) {
        for (size_type i = 0; i < other.size_; ++i) {
            emplace_back(std::move(other[i]));
        }
        other.clear();
    }

    FixedVector& operator=(FixedVector&& other) noexcept(
        std::is_nothrow_move_constructible_v<T>) {
        if (this == &other) return *this;
        clear();
        for (size_type i = 0; i < other.size_; ++i) {
            emplace_back(std::move(other[i]));
        }
        other.clear();
        return *this;
    }

    ~FixedVector() {
        clear();
    }

    // ===== Capacity =====
    static constexpr size_type capacity() noexcept { return N; }
    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    bool full() const noexcept { return size_ == N; }

    // ===== Element access =====
    reference operator[](size_type index) noexcept {
        return *ptr(index);
    }

    const_reference operator[](size_type index) const noexcept {
        return *ptr(index);
    }

    reference at(size_type index) {
        if (index >= size_) {
            throw std::out_of_range("FixedVector::at");
        }
        return (*this)[index];
    }

    const_reference at(size_type index) const {
        if (index >= size_) {
            throw std::out_of_range("FixedVector::at");
        }
        return (*this)[index];
    }

    reference front() noexcept { return (*this)[0]; }
    const_reference front() const noexcept { return (*this)[0]; }

    reference back() noexcept { return (*this)[size_ - 1]; }
    const_reference back() const noexcept { return (*this)[size_ - 1]; }

    pointer data() noexcept { return begin(); }
    const_pointer data() const noexcept { return begin(); }

    // ===== Iterators =====
    iterator begin() noexcept { return ptr(0); }
    const_iterator begin() const noexcept { return ptr(0); }
    const_iterator cbegin() const noexcept { return ptr(0); }

    iterator end() noexcept { return ptr(size_); }
    const_iterator end() const noexcept { return ptr(size_); }
    const_iterator cend() const noexcept { return ptr(size_); }

    // ===== Modifiers =====
    void clear() noexcept {
        for (size_type i = 0; i < size_; ++i) {
            ptr(i)->~T();
        }
        size_ = 0;
    }

    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ >= N) {
            throw std::length_error("FixedVector capacity exceeded");
        }

        T* location = ptr(size_);
        ::new (location) T(std::forward<Args>(args)...);
        ++size_;
        return *location;
    }

    void push_back(const T& value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    void pop_back() {
        if (empty()) {
            throw std::out_of_range("FixedVector::pop_back");
        }
        --size_;
        ptr(size_)->~T();
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        if (first < begin() || last > end() || first > last) {
            throw std::out_of_range("FixedVector::erase");
        }

        size_type start = first - begin();
        size_type count = last - first;

        for (size_type i = start; i + count < size_; ++i) {
            (*this)[i] = std::move((*this)[i + count]);
        }

        for (size_type i = 0; i < count; ++i) {
            pop_back();
        }

        return begin() + start;
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        if (size_ >= N) {
            throw std::length_error("FixedVector capacity exceeded");
        }
        if (pos < begin() || pos > end()) {
            throw std::out_of_range("FixedVector::insert");
        }

        size_type index = pos - begin();

        for (size_type i = size_; i > index; --i) {
            T* dst = ptr(i);
            T* src = ptr(i - 1);
            ::new (dst) T(std::move(*src));
            src->~T();
        }

        ::new (ptr(index)) T(std::forward<Args>(args)...);
        ++size_;
        return begin() + index;
    }

    iterator insert(const_iterator pos, const T& value) {
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value) {
        return emplace(pos, std::move(value));
    }

private:
    // ===== Raw storage =====
    alignas(T) std::byte buffer_[sizeof(T) * N];
    size_type size_ = 0;

    T* ptr(size_type index) noexcept {
        return std::launder(
            reinterpret_cast<T*>(buffer_ + index * sizeof(T))
        );
    }

    const T* ptr(size_type index) const noexcept {
        return std::launder(
            reinterpret_cast<const T*>(buffer_ + index * sizeof(T))
        );
    }
};
