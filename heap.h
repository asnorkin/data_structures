//
// Created by alexaxnder on 31.10.17.
//

#ifndef DATA_STRUCTURES_HEAP_H
#define DATA_STRUCTURES_HEAP_H

#include <vector>
#include <cstdlib>
#include <iostream>

#include "hash_map.h"


template <class Item, class Compare>
class Heap {
public:
    Heap() = default;
    ~Heap() = default;
    Heap(const Heap&);
    explicit Heap(std::vector<Item>*);

    Heap& operator=(Heap);
    void Swap(Heap&);

    template <class U>
    size_t Insert(U&&);
    void Remove(size_t);

    Item GetHead();
    Item ExtractHead();
    size_t Size();
    bool Empty();
    void Print(std::ostream& out = std::cout);

private:
    std::vector<Item> data_;
    Compare cmp_;

    size_t SiftUp(size_t idx);
    size_t SiftDown(size_t idx);

    void MakeHeap();

    size_t Parent(size_t idx);
    size_t LeftChild(size_t idx);
    size_t RightChild(size_t idx);
    bool HasParent(size_t idx);
    bool HasLeftChild(size_t idx);
    bool HasRightChild(size_t idx);
};


template <class Item>
class MinHeap: public Heap<Item, std::less<Item>> {
public:
    Item GetMin();
    Item ExtractMin();
};


template <class Item>
class MaxHeap: public Heap<Item, std::greater<Item>> {
public:
    Item GetMax();
    Item ExtractMax();
};


/*
 *      Heap implementation
 */
template <class Item, class Compare>
Heap<Item, Compare>::Heap(const Heap& rhs) {
    data_ = rhs.data_;
    cmp_ = rhs.cmp_;
}

template <class Item, class Compare>
Heap<Item, Compare>& Heap<Item, Compare>::operator=(Heap rhs) {
    Swap(rhs);
    return *this;
}

template <class Item, class Compare>
void Heap<Item, Compare>::Swap(Heap& rhs) {
    if (this == &rhs) {
        return;
    }

    std::swap(data_, rhs.data_);
    std::swap(cmp_, rhs.cmp_);
}

template <class Item, class Compare>
Heap<Item, Compare>::Heap(std::vector<Item>* items) {
    data_ = std::move(*items);
    MakeHeap();
}

template <class Item, class Compare>
template <class U>
size_t Heap<Item, Compare>::Insert(U&& item) {
    data_.push_back(std::forward<U>(item));
    return SiftUp(Size() - 1);
}

template <class Item, class Compare>
void Heap<Item, Compare>::Remove(size_t idx) {
    //  If idx is the last element just remove it
    if (idx + 1 == Size()) {
        data_.pop_back();
        return;
    }

    std::swap(data_[idx], data_.back());
    data_.pop_back();
    if (HasParent(idx) && !cmp_(data_[Parent(idx)], data_[idx])) {
        SiftUp(idx);
    } else {
        SiftDown(idx);
    }
}

template <class Item, class Compare>
Item Heap<Item, Compare>::GetHead() {
    return data_.front();
}

template <class Item, class Compare>
Item Heap<Item, Compare>::ExtractHead() {
    std::swap(data_.front(), data_.back());
    Item result = data_.back();
    data_.pop_back();
    SiftDown(0);
    return result;
}

template <class Item, class Compare>
void Heap<Item, Compare>::MakeHeap() {
    for (size_t i = 0; i <= data_.size() / 2; ++i) {
        SiftDown(data_.size() / 2 - i);
    }
}

template <class Item, class Compare>
size_t Heap<Item, Compare>::Size() {
    return data_.size();
}

template <class Item, class Compare>
bool Heap<Item, Compare>::Empty() {
    return Size() == 0;
}

template <class Item, class Compare>
void Heap<Item, Compare>::Print(std::ostream& out) {
    for (const auto& item : data_) {
        out << item << " ";
    }
    out << "\n";
}

template <class Item, class Compare>
size_t Heap<Item, Compare>::SiftUp(size_t idx) {
    while (HasParent(idx) && !cmp_(data_[Parent(idx)], data_[idx])) {
        std::swap(data_[Parent(idx)], data_[idx]);
        idx = Parent(idx);
    }

    return idx;
}

template <class Item, class Compare>
size_t Heap<Item, Compare>::SiftDown(size_t idx) {
    while ((HasLeftChild(idx) && !cmp_(data_[idx], data_[LeftChild(idx)])) ||
           (HasRightChild(idx) && !cmp_(data_[idx], data_[RightChild(idx)]))) {

        if (HasRightChild(idx) && cmp_(data_[RightChild(idx)], data_[LeftChild(idx)])) {
            std::swap(data_[RightChild(idx)], data_[idx]);
            idx = RightChild(idx);

        } else {
            std::swap(data_[LeftChild(idx)], data_[idx]);
            idx = LeftChild(idx);
        }
    }

    return idx;
}

template <class Item, class Compare>
size_t Heap<Item, Compare>::Parent(size_t idx) {
    if (idx == 0) {
        return Size();
    }

    return (idx - 1) / 2;
}

template <class Item, class Compare>
size_t Heap<Item, Compare>::LeftChild(size_t idx) {
    return std::min(Size(), 2 * idx + 1);
}

template <class Item, class Compare>
size_t Heap<Item, Compare>::RightChild(size_t idx) {
    return std::min(Size(), 2 * idx + 2);
}

template <class Item, class Compare>
bool Heap<Item, Compare>::HasParent(size_t idx) {
    return Parent(idx) != Size();
}

template <class Item, class Compare>
bool Heap<Item, Compare>::HasLeftChild(size_t idx) {
    return LeftChild(idx) != Size();
}

template <class Item, class Compare>
bool Heap<Item, Compare>::HasRightChild(size_t idx) {
    return RightChild(idx) != Size();
}


/*
 *      MinHeap implementation
 */
template <class Item>
Item MinHeap<Item>::GetMin() {
    return this->GetHead();
}

template <class Item>
Item MinHeap<Item>::ExtractMin() {
    return this->ExtractHead();
}


/*
 *      MaxHeap implementation
 */
template <class Item>
Item MaxHeap<Item>::GetMax() {
    return this->GetHead();
}

template <class Item>
Item MaxHeap<Item>::ExtractMax() {
    return this->ExtractHead();
}

#endif //DATA_STRUCTURES_HEAP_H
