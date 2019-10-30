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

//    Iterator iterator();
//    ConstIterator iterator() const;

private:
    using HashTablePair = std::pair<K, V>;
    using HashTableItem = std::vector<HashTablePair>;
    using HashTable = std::vector<HashTableItem>;

    size_t getHashFittedOnHashTable(const K &key) const;
    auto getPairHasTableItem_Iterator(const K &key);

    size_t size_;
    HashTable hashTable_;
    size_t hashTableCapacity_;
};

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
auto Dict<K, V>::getPairHasTableItem_Iterator(const K &key) {
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
