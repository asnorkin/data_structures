//
// Created by alexaxnder on 01.11.17.
//

#ifndef DATA_STRUCTURES_UNORDERED_SET_H
#define DATA_STRUCTURES_UNORDERED_SET_H

#include <cstdint>
#include <cstdlib>
#include <list>
#include <vector>
#include <stdexcept>


constexpr size_t default_buckets_number = 32;
constexpr double min_fulness = 0.125;
constexpr double max_fulness = 0.5;
constexpr uint32_t factor = 2;


template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    typedef std::pair<const KeyType, ValueType> KeyValuePair;

public:
    class iterator;
    class const_iterator;
    friend class iterator;
    friend class const_iterator;

    explicit HashMap(Hash hasher = Hash());
    HashMap(const HashMap& rhs);
    HashMap(HashMap&& rhs) noexcept;

    template <class ForwardIterator>
    HashMap(ForwardIterator, ForwardIterator, Hash hasher = Hash());

    HashMap(std::initializer_list<KeyValuePair>, Hash hasher = Hash());

    HashMap& operator=(HashMap rhs);

    size_t size() const;
    double fulness() const;
    bool empty() const;
    Hash hash_function() const;

    template <class U>
    void insert(U&&);
    void erase(const KeyType&);
    iterator find(const KeyType&);
    const_iterator find(const KeyType&) const;
    void swap(HashMap& rhs);

    ValueType& operator[](const KeyType&);
    const ValueType& at(const KeyType&) const;
    void clear();

    class iterator: public std::iterator<std::forward_iterator_tag, KeyValuePair> {
    public:
        iterator() = default;
        explicit iterator(HashMap<KeyType, ValueType, Hash>*);
        iterator(HashMap<KeyType, ValueType, Hash>*,
                 typename std::vector<std::list<KeyValuePair>>::iterator,
                 typename std::list<KeyValuePair>::iterator);

        iterator& operator++();
        iterator operator++(int);

        KeyValuePair& operator*();
        KeyValuePair* operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        HashMap<KeyType, ValueType, Hash>* hash_map_;
        typename std::vector<std::list<KeyValuePair>>::iterator cur_bucket_;
        typename std::list<KeyValuePair>::iterator cur_;
    };

    class const_iterator: public std::iterator<std::forward_iterator_tag, const KeyValuePair> {
    public:
        const_iterator() = default;
        explicit const_iterator(const HashMap<KeyType, ValueType, Hash>*);
        const_iterator(const HashMap<KeyType, ValueType, Hash>*,
                       typename std::vector<std::list<KeyValuePair>>::const_iterator,
                       typename std::list<KeyValuePair>::const_iterator);

        const_iterator& operator++();
        const_iterator operator++(int);

        const KeyValuePair& operator*();
        const KeyValuePair* operator->();

        bool operator==(const const_iterator& rhs) const;
        bool operator!=(const const_iterator& rhs) const;

    private:
        const HashMap<KeyType, ValueType, Hash>* hash_map_;
        typename std::vector<std::list<KeyValuePair>>::const_iterator cur_bucket_;
        typename std::list<KeyValuePair>::const_iterator cur_;
    };

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

private:
    std::vector<std::list<KeyValuePair>> buckets_;
    Hash hasher_;
    size_t size_;

    void Rehash(size_t new_size);
};

/*
 *
 *      HashMap implementation
 *
 */

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>&
HashMap<KeyType, ValueType, Hash>::operator=(HashMap rhs)  {
    swap(rhs);
    return *this;
};


template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(Hash hasher):
        buckets_{std::vector<std::list<KeyValuePair>>(default_buckets_number)},
        hasher_{hasher},
        size_{0}
{}

template <class KeyType, class ValueType, class Hash>
template <class ForwardIterator>
HashMap<KeyType, ValueType, Hash>::HashMap(ForwardIterator first,
                                           ForwardIterator last,
                                           Hash hasher):
        HashMap(hasher) {

    while (first != last) {
        insert(*first);
        first++;
    }
}

template <class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(std::initializer_list<KeyValuePair> list, Hash hasher):
        HashMap(list.begin(), list.end(), hasher)
{}

template <class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(const HashMap& rhs):
    buckets_{rhs.buckets_},
    hasher_{rhs.hasher_},
    size_{rhs.size_}
{}

template <class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(HashMap&& rhs) noexcept:
        buckets_{std::move(rhs.buckets_)},
        hasher_{std::move(rhs.hasher_)},
        size_{std::move(rhs.size_)}
{}

template <class KeyType, class ValueType, class Hash>
size_t HashMap<KeyType, ValueType, Hash>::size() const {
    return size_;
}

template <class KeyType, class ValueType, class Hash>
double HashMap<KeyType, ValueType, Hash>::fulness() const {
    return size_ / buckets_.size();
}

template <class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::empty() const {
    return size_ == 0;
}

template <class KeyType, class ValueType, class Hash>
Hash HashMap<KeyType, ValueType, Hash>::hash_function() const {
    return hasher_;
}

template <class KeyType, class ValueType, class Hash>
template <class U>
void HashMap<KeyType, ValueType, Hash>::insert(U&& key_value_pair) {
    //  Check if key already in map do nothing
    if (find(key_value_pair.first) != end()) {
        return;
    }

    size_t bucket_idx = hasher_(key_value_pair.first) % buckets_.size();
    buckets_[bucket_idx].push_front(std::forward<U>(key_value_pair));
    size_++;

    if (fulness() >= max_fulness) {
        Rehash(buckets_.size() * factor);
    }
}

template <class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(const KeyType& key) {
    size_t bucket_idx = hasher_(key) % buckets_.size();
    for (auto it = buckets_[bucket_idx].begin(); it != buckets_[bucket_idx].end(); ++it) {
        if (it->first == key) {
            it = buckets_[bucket_idx].erase(it);
            size_--;
            break;
        }
    }

    /*if (fulness() < min_fulness && buckets_.size() > default_buckets_number) {
        Rehash(buckets_.size() / factor);
    }*/
}

template <class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator
HashMap<KeyType, ValueType, Hash>::find(const KeyType& key) {
    size_t idx = hasher_(key) % buckets_.size();
    auto it = buckets_[idx].begin();
    while (it != buckets_[idx].end()) {
        if (it->first == key) {
            return HashMap<KeyType, ValueType, Hash>
                   ::iterator(this, this->buckets_.begin() + idx, it);
        }

        it++;
    }

    return end();
}

template <class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator
HashMap<KeyType, ValueType, Hash>::find(const KeyType& key) const {
    size_t idx = hasher_(key) % buckets_.size();
    auto it = buckets_[idx].begin();
    while (it != buckets_[idx].end()) {
        if (it->first == key) {
            return HashMap<KeyType, ValueType, Hash>
                   ::const_iterator(this, this->buckets_.begin() + idx, it);
        }

        it++;
    }

    return end();
}

template <class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::swap(HashMap &rhs) {
    if (&rhs == this) {
        return;
    }

    std::swap(buckets_, rhs.buckets_);
    std::swap(hasher_, rhs.hasher_);
    //std::swap(size_, rhs.size_);
    size_t tmp = size_;
    size_ = rhs.size_;
    rhs.size_ = tmp;
}

template <class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[](const KeyType& key) {
    auto it = find(key);
    if (it != end()) {
        return it->second;
    }

    size_t bucket_idx = hasher_(key) % buckets_.size();
    buckets_[bucket_idx].emplace_front(key, ValueType());
    size_++;

    if (fulness() >= max_fulness) {
        Rehash(buckets_.size() * factor);
    }

    return find(key)->second;
}

template <class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType, Hash>::at(const KeyType& key) const {
    auto it = find(key);
    if (it != end()) {
        return it->second;
    }

    throw std::out_of_range("No matching key!");
}

template <class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::clear() {
    buckets_ = std::vector<std::list<KeyValuePair>>(default_buckets_number);
    size_ = 0;
}


template <class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator
HashMap<KeyType, ValueType, Hash>::begin() {
    return iterator(this);
}

template <class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator
HashMap<KeyType, ValueType, Hash>::end() {
    return iterator(this, this->buckets_.end(), this->buckets_.front().end());
}

template <class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator
HashMap<KeyType, ValueType, Hash>::begin() const {
    return const_iterator(this);
}

template <class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator
HashMap<KeyType, ValueType, Hash>::end() const {
    return const_iterator(this, this->buckets_.end(), this->buckets_.front().end());
}

template <class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::Rehash(size_t new_size) {
    std::vector<std::list<KeyValuePair>> new_buckets(new_size);
    for (auto& bucket : buckets_) {
        for (auto& item : bucket) {
            size_t new_bucket_idx = hasher_(item.first) % new_buckets.size();
            new_buckets[new_bucket_idx].push_front(std::move(item));
        }
    }

    buckets_ = std::move(new_buckets);
}



/*
 *
 *      iterator implementation
 *
 */

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::iterator
::iterator(HashMap<KeyType, ValueType, Hash>* hash_map):
        hash_map_{hash_map},
        cur_bucket_{hash_map_->buckets_.begin()},
        cur_{cur_bucket_->begin()}
{
    //  Find first non empty bucket
    while (cur_bucket_ != hash_map_->buckets_.end() && cur_bucket_->empty()) {
        cur_bucket_++;
    }

    if (cur_bucket_ != hash_map_->buckets_.end()){
        cur_ = cur_bucket_->begin();
    } else {
        cur_ = hash_map_->buckets_.front().end();
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::iterator
::iterator(HashMap<KeyType, ValueType, Hash>* hash_map,
           typename std::vector<std::list<KeyValuePair>>::iterator bit,
           typename std::list<KeyValuePair>::iterator lit):
    hash_map_{hash_map},
    cur_bucket_{bit},
    cur_{lit}
{}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator&
HashMap<KeyType, ValueType, Hash>::iterator
::operator++() {
    //  Trying to go next in current bucket
    if (cur_ != cur_bucket_->end()) {
        cur_++;
    }

    //  Current bucket is over
    if (cur_ == cur_bucket_->end()) {
        cur_bucket_++;

        //  Find next non empty bucket (if exist)
        while (cur_bucket_ != hash_map_->buckets_.end() && cur_bucket_->empty()) {
            cur_bucket_++;
        }

        //  If all next buckets are empty
        if (cur_bucket_ == hash_map_->buckets_.end()) {
            cur_ = hash_map_->buckets_.front().end();

        //  Or traverse non empty bucket
        } else {
            cur_ = cur_bucket_->begin();
        }
    }

    return *this;
};

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator
HashMap<KeyType, ValueType, Hash>::iterator
::operator++(int) {
    iterator cpy(*this);
    this->operator++();
    return cpy;
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::KeyValuePair&
HashMap<KeyType, ValueType, Hash>::iterator
::operator*() {
    return cur_.operator*();
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::KeyValuePair*
HashMap<KeyType, ValueType, Hash>::iterator
::operator->() {
    return cur_.operator->();
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::iterator
::operator==(const iterator &rhs) const {
    return hash_map_ == rhs.hash_map_ &&
           cur_bucket_ == rhs.cur_bucket_ &&
           cur_ == rhs.cur_;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::iterator
::operator!=(const iterator &rhs) const {
    return hash_map_ != rhs.hash_map_ ||
           cur_bucket_ != rhs.cur_bucket_ ||
           cur_ != rhs.cur_;
}

/*
 *
 *      ConstIterator implementation
 *
 */

template <class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::const_iterator
::const_iterator(const HashMap<KeyType, ValueType, Hash>* hash_map):
    hash_map_{hash_map},
    cur_bucket_{hash_map->buckets_.begin()},
    cur_{cur_bucket_->begin()}
{
    //  Find first non empty bucket
    while (cur_bucket_ != hash_map_->buckets_.end() && cur_bucket_->empty()) {
        cur_bucket_++;
    }

    if (cur_bucket_ != hash_map_->buckets_.end()) {
        cur_ = cur_bucket_->begin();
    } else {
        cur_ = hash_map_->buckets_.front().end();
    }
}

template <class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::const_iterator
::const_iterator(const HashMap<KeyType, ValueType, Hash>* hash_map,
                 typename std::vector<std::list<KeyValuePair>>::const_iterator bit,
                 typename std::list<KeyValuePair>::const_iterator lit):
    hash_map_{hash_map},
    cur_bucket_{bit},
    cur_{lit}
{}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator&
HashMap<KeyType, ValueType, Hash>::const_iterator
::operator++() {
    //  Trying to go next in current bucker
    if (cur_ != cur_bucket_->end()) {
        cur_++;
    }

    //  Current bucket is over
    if (cur_ == cur_bucket_->end()) {
        cur_bucket_++;

        //  Find next non empty bucket (if exist)
        while (cur_bucket_ != hash_map_->buckets_.end() && cur_bucket_->empty()) {
            cur_bucket_++;
        }

        //  If all next buckets are empty
        if (cur_bucket_ == hash_map_->buckets_.end()) {
            cur_ = hash_map_->buckets_.front().end();

            //  Or traverse non empty bucket
        } else {
            cur_ = cur_bucket_->begin();
        }
    }

    return *this;
};

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator
HashMap<KeyType, ValueType, Hash>::const_iterator
::operator++(int) {
    const_iterator cpy(*this);
    this->operator++();
    return cpy;
}

template<class KeyType, class ValueType, class Hash>
const typename HashMap<KeyType, ValueType, Hash>::KeyValuePair&
HashMap<KeyType, ValueType, Hash>::const_iterator
::operator*() {
    return cur_.operator*();
}

template<class KeyType, class ValueType, class Hash>
const typename HashMap<KeyType, ValueType, Hash>::KeyValuePair*
HashMap<KeyType, ValueType, Hash>::const_iterator
::operator->() {
    return cur_.operator->();
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::const_iterator
::operator==(const const_iterator &rhs) const {
    return hash_map_ == rhs.hash_map_ &&
           cur_bucket_ == rhs.cur_bucket_ &&
           cur_ == rhs.cur_;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::const_iterator
::operator!=(const const_iterator &rhs) const {
    return hash_map_ != rhs.hash_map_ ||
           cur_bucket_ != rhs.cur_bucket_ ||
           cur_ != rhs.cur_;
}

#endif //DATA_STRUCTURES_UNORDERED_SET_H
