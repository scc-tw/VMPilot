#ifndef VMPILOT_COMMON_SMALL_VECTOR_HPP
#define VMPILOT_COMMON_SMALL_VECTOR_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <new>
#include <type_traits>

namespace VMPilot::Common {

/// A vector-like container with inline storage for up to N elements.
///
/// When the element count is <= N, all storage is inline (zero heap
/// allocations).  Beyond N, falls back to heap allocation with the
/// standard doubling growth policy.
///
/// Provides the same interface subset used by std::vector in this
/// project: push_back, emplace_back, reserve, operator[], iterators,
/// size/empty/capacity, and full value semantics (copy, move, swap).
///
/// Requires T to be a complete type (unlike std::vector).  This means
/// SmallVector<T, N> cannot be used as a member of T itself — use
/// std::vector<T> for self-referential / recursive tree structures.
///
/// Design notes:
///   - alignas(T) unsigned char buffer for inline storage (no union
///     tricks that would require default-constructible T).
///   - Trivially-destructible T skips per-element destructor calls.
///   - Trivially-copyable T uses memcpy in copy/grow paths.
template <typename T, std::size_t N>
class SmallVector {
    static_assert(N > 0, "SmallVector<T, 0> is not supported; use std::vector<T> instead");

public:
    // --- Standard container type aliases ---
    using value_type      = T;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using iterator        = T*;
    using const_iterator  = const T*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // -----------------------------------------------------------------
    //  Construction / Destruction
    // -----------------------------------------------------------------

    SmallVector() noexcept : data_(inline_ptr()), size_(0), capacity_(N) {}

    explicit SmallVector(size_type count) : SmallVector() {
        resize(count);
    }

    SmallVector(size_type count, const T& value) : SmallVector() {
        assign(count, value);
    }

    template <typename InputIt,
              typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    SmallVector(InputIt first, InputIt last) : SmallVector() {
        assign(first, last);
    }

    SmallVector(std::initializer_list<T> init) : SmallVector() {
        assign(init.begin(), init.end());
    }

    ~SmallVector() {
        destroy_range(data_, data_ + size_);
        free_heap();
    }

    // -----------------------------------------------------------------
    //  Copy
    // -----------------------------------------------------------------

    SmallVector(const SmallVector& other) : SmallVector() {
        reserve(other.size_);
        copy_construct_range(other.data_, other.data_ + other.size_, data_);
        size_ = other.size_;
    }

    SmallVector& operator=(const SmallVector& other) {
        if (this == &other) return *this;
        clear();
        reserve(other.size_);
        copy_construct_range(other.data_, other.data_ + other.size_, data_);
        size_ = other.size_;
        return *this;
    }

    // -----------------------------------------------------------------
    //  Move
    // -----------------------------------------------------------------

    SmallVector(SmallVector&& other)
        noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_(inline_ptr()), size_(0), capacity_(N)
    {
        move_from(std::move(other));
    }

    SmallVector& operator=(SmallVector&& other)
        noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        if (this == &other) return *this;
        clear();
        free_heap();
        data_     = inline_ptr();
        capacity_ = N;
        move_from(std::move(other));
        return *this;
    }

    // -----------------------------------------------------------------
    //  Assignment helpers
    // -----------------------------------------------------------------

    void assign(size_type count, const T& value) {
        clear();
        reserve(count);
        for (size_type i = 0; i < count; ++i)
            construct_at(data_ + i, value);
        size_ = count;
    }

    template <typename InputIt,
              typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    void assign(InputIt first, InputIt last) {
        clear();
        if constexpr (std::is_base_of_v<
                          std::random_access_iterator_tag,
                          typename std::iterator_traits<InputIt>::iterator_category>) {
            auto n = static_cast<size_type>(std::distance(first, last));
            reserve(n);
        }
        for (; first != last; ++first)
            push_back(*first);
    }

    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    // -----------------------------------------------------------------
    //  Element access
    // -----------------------------------------------------------------

    reference       operator[](size_type i)       noexcept { assert(i < size_); return data_[i]; }
    const_reference operator[](size_type i) const noexcept { assert(i < size_); return data_[i]; }

    reference       front()       noexcept { assert(size_ > 0); return data_[0]; }
    const_reference front() const noexcept { assert(size_ > 0); return data_[0]; }
    reference       back()        noexcept { assert(size_ > 0); return data_[size_ - 1]; }
    const_reference back()  const noexcept { assert(size_ > 0); return data_[size_ - 1]; }

    pointer         data()        noexcept { return data_; }
    const_pointer   data()  const noexcept { return data_; }

    // -----------------------------------------------------------------
    //  Iterators
    // -----------------------------------------------------------------

    iterator        begin()        noexcept { return data_; }
    const_iterator  begin()  const noexcept { return data_; }
    const_iterator  cbegin() const noexcept { return data_; }
    iterator        end()          noexcept { return data_ + size_; }
    const_iterator  end()    const noexcept { return data_ + size_; }
    const_iterator  cend()   const noexcept { return data_ + size_; }

    reverse_iterator       rbegin()        noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    reverse_iterator       rend()          noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }

    // -----------------------------------------------------------------
    //  Capacity
    // -----------------------------------------------------------------

    [[nodiscard]] bool      empty()    const noexcept { return size_ == 0; }
    [[nodiscard]] size_type size()     const noexcept { return size_; }
    [[nodiscard]] size_type capacity() const noexcept { return capacity_; }

    void reserve(size_type new_cap) {
        if (new_cap <= capacity_) return;
        grow_to(new_cap);
    }

    void shrink_to_fit() {
        if (is_inline() || size_ == capacity_) return;
        if (size_ <= N) {
            // Move back to inline storage
            pointer old = data_;
            data_ = inline_ptr();
            move_construct_range(old, old + size_, data_);
            destroy_range(old, old + size_);
            ::operator delete(old);
            capacity_ = N;
        } else {
            // Reallocate to exact size
            pointer new_data = allocate(size_);
            move_construct_range(data_, data_ + size_, new_data);
            destroy_range(data_, data_ + size_);
            ::operator delete(data_);
            data_ = new_data;
            capacity_ = size_;
        }
    }

    // -----------------------------------------------------------------
    //  Modifiers
    // -----------------------------------------------------------------

    void clear() noexcept {
        destroy_range(data_, data_ + size_);
        size_ = 0;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) grow_auto();
        construct_at(data_ + size_, value);
        ++size_;
    }

    void push_back(T&& value) {
        if (size_ == capacity_) grow_auto();
        construct_at(data_ + size_, std::move(value));
        ++size_;
    }

    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ == capacity_) grow_auto();
        construct_at(data_ + size_, std::forward<Args>(args)...);
        return data_[size_++];
    }

    void pop_back() noexcept {
        assert(size_ > 0);
        --size_;
        destroy_at(data_ + size_);
    }

    void resize(size_type count) {
        if (count < size_) {
            destroy_range(data_ + count, data_ + size_);
            size_ = count;
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i)
                construct_at(data_ + i);
            size_ = count;
        }
    }

    void resize(size_type count, const T& value) {
        if (count < size_) {
            destroy_range(data_ + count, data_ + size_);
            size_ = count;
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i)
                construct_at(data_ + i, value);
            size_ = count;
        }
    }

    iterator erase(const_iterator pos) {
        assert(pos >= begin() && pos < end());
        auto idx = static_cast<size_type>(pos - begin());
        // Shift left
        for (size_type i = idx; i + 1 < size_; ++i)
            data_[i] = std::move(data_[i + 1]);
        --size_;
        destroy_at(data_ + size_);
        return data_ + idx;
    }

    iterator erase(const_iterator first, const_iterator last) {
        assert(first >= begin() && last <= end() && first <= last);
        auto start = static_cast<size_type>(first - begin());
        auto count = static_cast<size_type>(last - first);
        if (count == 0) return data_ + start;
        // Shift elements
        for (size_type i = start; i + count < size_; ++i)
            data_[i] = std::move(data_[i + count]);
        destroy_range(data_ + size_ - count, data_ + size_);
        size_ -= count;
        return data_ + start;
    }

    iterator insert(const_iterator pos, const T& value) {
        auto idx = static_cast<size_type>(pos - begin());
        if (size_ == capacity_) {
            grow_auto();
        }
        // Shift right
        if (idx < size_) {
            construct_at(data_ + size_, std::move(data_[size_ - 1]));
            for (size_type i = size_ - 1; i > idx; --i)
                data_[i] = std::move(data_[i - 1]);
            data_[idx] = value;
        } else {
            construct_at(data_ + size_, value);
        }
        ++size_;
        return data_ + idx;
    }

    iterator insert(const_iterator pos, T&& value) {
        auto idx = static_cast<size_type>(pos - begin());
        if (size_ == capacity_) {
            grow_auto();
        }
        if (idx < size_) {
            construct_at(data_ + size_, std::move(data_[size_ - 1]));
            for (size_type i = size_ - 1; i > idx; --i)
                data_[i] = std::move(data_[i - 1]);
            data_[idx] = std::move(value);
        } else {
            construct_at(data_ + size_, std::move(value));
        }
        ++size_;
        return data_ + idx;
    }

    void swap(SmallVector& other)
        noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        if (this == &other) return;

        // Both heap: just swap pointers
        if (!is_inline() && !other.is_inline()) {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
            return;
        }

        // General case: move through a temporary
        SmallVector tmp(std::move(other));
        other = std::move(*this);
        *this = std::move(tmp);
    }

    // -----------------------------------------------------------------
    //  Comparison
    // -----------------------------------------------------------------

    friend bool operator==(const SmallVector& a, const SmallVector& b) noexcept {
        if (a.size_ != b.size_) return false;
        return std::equal(a.begin(), a.end(), b.begin());
    }

    friend bool operator!=(const SmallVector& a, const SmallVector& b) noexcept {
        return !(a == b);
    }

    friend bool operator<(const SmallVector& a, const SmallVector& b) noexcept {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
    }

private:
    // -----------------------------------------------------------------
    //  Inline storage
    // -----------------------------------------------------------------

    alignas(T) unsigned char inline_buf_[sizeof(T) * N];

    pointer       inline_ptr()       noexcept { return reinterpret_cast<pointer>(inline_buf_); }
    const_pointer inline_ptr() const noexcept { return reinterpret_cast<const_pointer>(inline_buf_); }

    bool is_inline() const noexcept { return data_ == inline_ptr(); }

    // -----------------------------------------------------------------
    //  Data members  (24 bytes on 64-bit: ptr + size + cap)
    // -----------------------------------------------------------------

    pointer   data_;
    size_type size_;
    size_type capacity_;

    // -----------------------------------------------------------------
    //  Allocation
    // -----------------------------------------------------------------

    static pointer allocate(size_type n) {
        return static_cast<pointer>(::operator new(sizeof(T) * n));
    }

    void free_heap() noexcept {
        if (!is_inline())
            ::operator delete(data_);
    }

    void grow_auto() {
        size_type new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
        grow_to(new_cap);
    }

    void grow_to(size_type new_cap) {
        assert(new_cap > capacity_);
        pointer new_data = allocate(new_cap);
        move_construct_range(data_, data_ + size_, new_data);
        destroy_range(data_, data_ + size_);
        free_heap();
        data_     = new_data;
        capacity_ = new_cap;
    }

    // -----------------------------------------------------------------
    //  Move-from helper  (used by move ctor / move assign)
    // -----------------------------------------------------------------

    void move_from(SmallVector&& other) {
        if (other.is_inline()) {
            // Element-wise move from other's inline buffer to ours
            move_construct_range(other.data_, other.data_ + other.size_, data_);
            destroy_range(other.data_, other.data_ + other.size_);
            size_ = other.size_;
            other.size_ = 0;
        } else {
            // Steal heap pointer
            data_     = other.data_;
            size_     = other.size_;
            capacity_ = other.capacity_;
            other.data_     = other.inline_ptr();
            other.size_     = 0;
            other.capacity_ = N;
        }
    }

    // -----------------------------------------------------------------
    //  Low-level element operations  (trivial-type fast paths)
    // -----------------------------------------------------------------

    template <typename... Args>
    static void construct_at(pointer p, Args&&... args) {
        ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
    }

    static void destroy_at(pointer p) noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>)
            p->~T();
    }

    static void destroy_range(pointer first, pointer last) noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (; first != last; ++first)
                first->~T();
        }
    }

    static void copy_construct_range(const_pointer src, const_pointer src_end,
                                     pointer dst) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            auto n = static_cast<std::size_t>(src_end - src);
            if (n > 0) std::memcpy(dst, src, n * sizeof(T));
        } else {
            for (; src != src_end; ++src, ++dst)
                construct_at(dst, *src);
        }
    }

    static void move_construct_range(pointer src, pointer src_end,
                                     pointer dst) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            auto n = static_cast<std::size_t>(src_end - src);
            if (n > 0) std::memcpy(dst, src, n * sizeof(T));
        } else {
            for (; src != src_end; ++src, ++dst)
                construct_at(dst, std::move(*src));
        }
    }
};

// ADL-discoverable swap
template <typename T, std::size_t N>
void swap(SmallVector<T, N>& a, SmallVector<T, N>& b)
    noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}

}  // namespace VMPilot::Common

#endif  // VMPILOT_COMMON_SMALL_VECTOR_HPP
