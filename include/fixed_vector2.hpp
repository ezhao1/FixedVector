#pragma once
#include <cstddef>

namespace fixed_vector
{
    template <typename T, size_t N>
    class FixedVector
    {
        public:
            using value_type = T;
            using size_type = size_t;

            FixedVector() m_size(0) {}
            ~FixedVector() {
                // explicitly call destructor on all elements
                for (int i = 0; i < m_size; i++) {
                    data()[i].~T();
                }
            }

            void push_back(const T& value) {
                if (m_size >= N) {
                    throw;
                }

                data()[m_size] = value;
                m_size++;
            }

            void erase(T* iterator) {
            }

            void insert(const T& value) {

            }

            T* begin() {
                return data();
            }

            T* end() {
                return data() + m_size * sizeof(T);
            }
        private:
            alignas(T) unsigned char m_storage[sizeof(T) * N];
            size_t m_size;
            T* data() noexcept {
                return reinterpret_cast<T*>(m_storage);
            } 
    }
}