#pragma once

template <typename K, typename V>
class Dict {
public:

    Dict();
    ~Dict() = default;

    void put(const K& key, const V& value);
    void remove(const K& key);
    bool contains(const K& key);

    const V& operator[](const K& key) const;
    V& operator[](const K& key);

    size_t size() const;

    class Iterator {
    public:
        friend class Dict<K, V>;

        const V& get() const;
        void set(const V& value);
        void next();
        void prev();
        void toKey(const K &key);
        bool hasNext() const;
        bool hasPrev() const;

    private:
        explicit Iterator(Dict<K, V>* owner);
        Dict<K, V>* owner_;
        ssize_t posInHashTable_;
        ssize_t posInHashTableList_;

        std::pair<ssize_t , ssize_t > getNextPos() const;
        std::pair<ssize_t , ssize_t > getPrevPos() const;
    };

    Iterator iterator();

private:
    using HashTablePair = std::pair<K, V>;
    using HashTableItem = std::vector<HashTablePair>;
    using HashTable = std::vector<HashTableItem>;

    size_t getHashFittedOnHashTable(const K &key) const;
    auto getPairHashTableItem_Iterator(const K &key);

    size_t size_;
    HashTable hashTable_;
    size_t hashTableCapacity_;
};

template<typename K, typename V>
const V &Dict<K, V>::Iterator::get() const {
    return owner_->hashTable_[posInHashTable_][posInHashTableList_].second;
}

template<typename K, typename V>
void Dict<K, V>::Iterator::set(const V &value) {
    owner_->hashTable_[posInHashTable_][posInHashTableList_].second = value;
}

template<typename K, typename V>
void Dict<K, V>::Iterator::next() {
    const auto pair = getNextPos();
    posInHashTable_ = pair.first;
    posInHashTableList_ = pair.second;
}

template<typename K, typename V>
void Dict<K, V>::Iterator::prev() {
    const auto pair = getPrevPos();
    posInHashTable_ = pair.first;
    posInHashTableList_ = pair.second;
}

template<typename K, typename V>
bool Dict<K, V>::Iterator::hasNext() const {
    return getNextPos().first != std::numeric_limits<ssize_t>::max();
}

template<typename K, typename V>
bool Dict<K, V>::Iterator::hasPrev() const {
    return getPrevPos().first != std::numeric_limits<ssize_t>::max();
}

template<typename K, typename V>
Dict<K, V>::Iterator::Iterator(Dict<K, V> *owner) : owner_(owner), posInHashTable_(0), posInHashTableList_(-1) {
    const auto pair = getNextPos();
    posInHashTable_ = pair.first;
    posInHashTableList_ = pair.second;
}

template<typename K, typename V>
void Dict<K, V>::Iterator::toKey(const K &key) {
    posInHashTable_ = owner_->getHashFittedOnHashTable(key);
    assert(owner_->hashTable_[posInHashTable_].size() >= 1);
    posInHashTableList_ = 0;
}

template<typename K, typename V>
std::pair<ssize_t , ssize_t > Dict<K, V>::Iterator::getNextPos() const {
    auto posInHashTable = posInHashTable_;
    auto posInHashTableList = posInHashTableList_;
    const auto& hashTable = owner_->hashTable_;
    do {
        ++posInHashTableList;
        auto& curHashTableList = hashTable[posInHashTable];
        if (posInHashTableList < curHashTableList.size()) {
            return {posInHashTable, posInHashTableList};
        } else {
            posInHashTableList = -1;
        }
        ++posInHashTable;
    } while(posInHashTable < hashTable.size());

    return {std::numeric_limits<ssize_t>::max(), std::numeric_limits<ssize_t>::max()};
}

template<typename K, typename V>
std::pair<ssize_t , ssize_t > Dict<K, V>::Iterator::getPrevPos() const {
    ssize_t posInHashTable = posInHashTable_;
    ssize_t posInHashTableList = posInHashTableList_;
    const auto& hashTable = owner_->hashTable_;
    auto& curHashTableList = hashTable[posInHashTable];
    do {
        --posInHashTableList;
        if (posInHashTableList >= 0) {
            return {posInHashTable, posInHashTableList};
        } else {
            --posInHashTable;
            if (posInHashTable >= 0) {
                posInHashTableList = hashTable[posInHashTable].size();
            } else {
                posInHashTableList = 0;
            }
        }
    } while(posInHashTable >= 0);

    return {std::numeric_limits<ssize_t>::max(), std::numeric_limits<ssize_t>::max()};
}

template<typename K, typename V>
Dict<K, V>::Dict() : size_(0), hashTableCapacity_(0) {

}

template<typename K, typename V>
void Dict<K, V>::put(const K &key, const V &value) {
    ++size_;

    bool needRecompute = false;

    while (size_ > hashTableCapacity_ * 0.75) {
        hashTableCapacity_ = hashTableCapacity_ == 0 ? 2 : hashTableCapacity_ * 2;
        needRecompute = true;
    }

    if (needRecompute) {
        HashTable nextHashTable;
        nextHashTable.resize(hashTableCapacity_);

        for (const auto &htItem : hashTable_) {
            for (const auto &htPair : htItem) {
                const auto hash = getHashFittedOnHashTable(htPair.first);
                nextHashTable[hash].emplace_back(std::move(htPair));
            }
        }

        hashTable_ = std::move(nextHashTable);
    }

    const auto hash = getHashFittedOnHashTable(key);
    hashTable_[hash].emplace_back(key, value);
}

template<typename K, typename V>
void Dict<K, V>::remove(const K &key) {
    const auto hash = getHashFittedOnHashTable(key);
    HashTableItem& htItem = hashTable_[hash];

    auto deleteIter = htItem.begin();
    while (deleteIter != htItem.end()) {
        if (deleteIter->first == key) {
            break;
        }
        ++deleteIter;
    }

    if (deleteIter == htItem.end()) {
        throw std::runtime_error("key is not exist in hashtable");
    }

    htItem.erase(deleteIter);
}

template<typename K, typename V>
bool Dict<K, V>::contains(const K &key) {
    if (size_ == 0) {
        return false;
    }

    const auto hash = getHashFittedOnHashTable(key);
    const HashTableItem& htItem = hashTable_[hash];

    auto foundedIter = htItem.begin();
    while (foundedIter != htItem.end()) {
        if (foundedIter->first == key) {
            break;
        }
        ++foundedIter;
    }

    return foundedIter != htItem.end();
}

template<typename K, typename V>
const V &Dict<K, V>::operator[](const K &key) const {
    const auto hash = getHashFittedOnHashTable(key);
    const HashTableItem& htItem = hashTable_[hash];

    auto foundedIter = htItem.begin();
    while (foundedIter != htItem.end()) {
        if (foundedIter->first == key) {
            break;
        }
        ++foundedIter;
    }

    if (foundedIter == htItem.end()) {
        throw std::runtime_error("key is not exist in hashtable");
    }

    return foundedIter->second;
}

template<typename K, typename V>
V &Dict<K, V>::operator[](const K &key) {
    const auto hash = getHashFittedOnHashTable(key);
    HashTableItem& htItem = hashTable_[hash];

    auto foundedIter = htItem.begin();
    while (foundedIter != htItem.end()) {
        if (foundedIter->first == key) {
            break;
        }
        ++foundedIter;
    }

    if (foundedIter == htItem.end()) {
        throw std::runtime_error("key is not exist in hashtable");
    }

    return foundedIter->second;
}

template<typename K, typename V>
size_t Dict<K, V>::size() const {
    return size_;
}

template<typename K, typename V>
size_t Dict<K, V>::getHashFittedOnHashTable(const K &key) const {
    return std::hash<K>()(key) % hashTableCapacity_;
}

template<typename K, typename V>
auto Dict<K, V>::getPairHashTableItem_Iterator(const K &key) {
    const auto hash = getHashFittedOnHashTable(key);
    HashTableItem htItem = hashTable_[hash];

    auto foundedIter = htItem.begin();
    while (foundedIter != htItem.end()) {
        if (foundedIter->first == key) {
            break;
        }
        ++foundedIter;
    }

    return std::make_pair(htItem, foundedIter);
}

template<typename K, typename V>
typename Dict<K, V>::Iterator Dict<K, V>::iterator() {
    return Dict::Iterator(this);
}
