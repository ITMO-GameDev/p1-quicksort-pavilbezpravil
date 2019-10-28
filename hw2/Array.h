#pragma once

#include <cstring>
#include <cassert>

template <typename T>
class Array final {
public:
    class Iterator;

    Array();
    explicit Array(size_t capacity);

    Array<T>& operator=(const Array<T>& other) = delete;

    ~Array();

    void reserve(size_t size);

    void insert(const T& value);
    void insert(int index, const T& value);

    void remove(int index);

    const T& operator[](int index) const;
    T& operator[](int index);

    int size() const;

    const T* data() const;
    T* data();

    Iterator iterator();
    const Iterator iterator() const;

    class Iterator {
    public:
        friend class Array;

        const T& get() const;
        void set(const T& value);
        void insert(const T& value);
        void remove();
        void next();
        void prev();
        void toIndex(int index);
        bool hasNext() const;
        bool hasPrev() const;

    private:
        explicit Iterator(Array<T>* owner);
        Array<T>* owner_;
        size_t pos_;
    };

private:
    size_t capacity_;
    size_t size_;
    T* data_;
};

template<typename T>
const T &Array<T>::Iterator::get() const {
    return (*owner_)[pos_];
}

template<typename T>
void Array<T>::Iterator::set(const T &value) {
    (*owner_)[pos_] = value;
}

template<typename T>
void Array<T>::Iterator::insert(const T &value) {
    owner_->insert(pos_, value);
}

template<typename T>
void Array<T>::Iterator::remove() {
    owner_->remove(pos_);
}

template<typename T>
void Array<T>::Iterator::next() {
    ++pos_;
    assert(pos_ < owner_->size());
}

template<typename T>
void Array<T>::Iterator::prev() {
    assert(pos_);
    --pos_;
}

template<typename T>
void Array<T>::Iterator::toIndex(int index) {
    assert(index < owner_->size());
    pos_ = index;
}

template<typename T>
bool Array<T>::Iterator::hasNext() const {
    return pos_ + 1 < owner_->size();
}

template<typename T>
bool Array<T>::Iterator::hasPrev() const {
    return pos_ > 0;
}

template<typename T>
Array<T>::Iterator::Iterator(Array<T> *owner) : owner_(owner), pos_(0) {

}

template<typename T>
Array<T>::Array() : Array(0) {

}

template<typename T>
Array<T>::Array(size_t capacity) : data_(nullptr), capacity_(capacity), size_(0) {
    if (capacity_) {
        reserve(capacity_);
    }
}

template<typename T>
Array<T>::~Array() {
    if (data_) {
        delete[](data_);
    }
}

template<typename T>
void Array<T>::insert(const T &value) {
    insert(size_, value);
}

template<typename T>
void Array<T>::insert(int index, const T &value) {
    assert(index <= size_);
    // can be optimized by copy not all data in reserve function
    reserve(size_ + 1);
    size_t moveBytes = (size_ - index) * sizeof(T);
    std::memmove(data_ + index + 1, data_ + index, moveBytes);
    data_[index] = value;
    ++size_;
}

template<typename T>
void Array<T>::remove(int index) {
    assert(index < size_);
    size_t moveBytes = (size_ - index- 1) * sizeof(T);
    std::memmove(data_ + index, data_ + index + 1, moveBytes);
    --size_;
}

template<typename T>
const T &Array<T>::operator[](int index) const {
    return data_[index];
}

template<typename T>
T &Array<T>::operator[](int index) {
    return data_[index];
}

template<typename T>
int Array<T>::size() const {
    return size_;
}

template<typename T>
void Array<T>::reserve(size_t size) {
    if (capacity_ < size) {
        size_t newCapacity = capacity_ == 0 ? 2 : capacity_;
        while ((newCapacity <<= 1) < size);

        T* newData = new T[newCapacity];
        if (capacity_) {
            std::memcpy(newData, data_, capacity_ * sizeof(T));
            delete[](data_);
        }

        capacity_ = newCapacity;
        data_ = newData;
    }
}

template<typename T>
const T *Array<T>::data() const {
    return data_;
}

template<typename T>
T *Array<T>::data() {
    return data_;
}

template<typename T>
typename Array<T>::Iterator Array<T>::iterator() {
    return Array::Iterator(this);
}

template<typename T>
const typename Array<T>::Iterator Array<T>::iterator() const {
    return Array::Iterator(this);
}

