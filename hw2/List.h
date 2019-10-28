#pragma once

#include <cstdlib>
#include <cassert>

template<typename T>
class List {
public:
    struct Node {
        Node* prev;
        Node* next;
        T val;

        Node(Node *prev, Node *next, T val);
    };

    List();
    ~List();

    List& operator=(const List& other) = delete;

    void insert_head(const T& value);
    void insert_tail(const T& value);
    Node* insert(Node* node, const T& value);
    void remove(Node* node);

    void remove_head();
    void remove_tail();

    const T& head() const;
    const T& tail() const;

    int size() const;
    bool empty() const;

    void clear();

    class Iterator {
    public:
        friend class List;

        const T& get() const;
        void set(const T& value);
        void insert(const T& value);
        void remove();
        void next();
        void prev();
        bool hasNext() const;
        bool hasPrev() const;

    private:
        explicit Iterator(List<T>* owner, Node* node);
        Node* node_;
        List<T>* owner_;
    };

    class ConstIterator {
    public:
        friend class List;

        const T& get() const;
        void next();
        void prev();
        bool hasNext() const;
        bool hasPrev() const;

    private:
        explicit ConstIterator(const List<T>* owner, const Node* node);
        const Node* node_;
        const List<T>* owner_;
    };

    Iterator iterator();
    const ConstIterator iterator() const;

private:
    Node* head_;
    Node* tail_;
    size_t size_;
};

template<typename T>
const T &List<T>::ConstIterator::get() const {
    return node_->val;
}

template<typename T>
void List<T>::ConstIterator::next() {
    node_ = node_->next;
}

template<typename T>
void List<T>::ConstIterator::prev() {
    node_ = node_->prev;
}

template<typename T>
bool List<T>::ConstIterator::hasNext() const {
    return node_;
}

template<typename T>
bool List<T>::ConstIterator::hasPrev() const {
    return node_;
}

template<typename T>
List<T>::ConstIterator::ConstIterator(const List<T> *owner, const List::Node *node) : owner_(owner), node_(node) {

}

template<typename T>
const T &List<T>::Iterator::get() const {
    return node_->val;
}

template<typename T>
void List<T>::Iterator::set(const T &value) {
    node_->val = value;
}

template<typename T>
void List<T>::Iterator::insert(const T &value) {
    node_ = owner_->insert(node_, value);
}

template<typename T>
void List<T>::Iterator::remove() {
    Node* nextNode = node_->next;
    owner_->remove(node_);
    node_ = nextNode;
}

template<typename T>
void List<T>::Iterator::next() {
    node_ = node_->next;
}

template<typename T>
void List<T>::Iterator::prev() {
    node_ = node_->prev;
}

template<typename T>
bool List<T>::Iterator::hasNext() const {
    return node_;
}

template<typename T>
bool List<T>::Iterator::hasPrev() const {
    return node_;
}

template<typename T>
List<T>::Iterator::Iterator(List<T> *owner, List::Node *node) : owner_(owner), node_(node) {

}

template<typename T>
List<T>::Node::Node(List::Node *prev, List::Node *next, T val) : prev(prev), next(next), val(val) {

}

template<typename T>
List<T>::List() : head_(nullptr), tail_(nullptr), size_(0) {

}

template<typename T>
List<T>::~List() {
    clear();
}

template<typename T>
void List<T>::insert_head(const T &value) {
    Node* node = new Node(nullptr, head_, value);

    if (head_) {
        head_->prev = node;
        head_ = node;
    } else {
        assert(!tail_);
        assert(empty());
        head_ = node;
        tail_ = node;
    }

    assert(!head_->prev);

    ++size_;
}

template<typename T>
void List<T>::insert_tail(const T &value) {
    Node* node = new Node(tail_, nullptr, value);

    if (tail_) {
        tail_->next = node;
        tail_ = node;
    } else {
        assert(!head_);
        assert(empty());
        head_ = node;
        tail_ = node;
    }

    assert(!tail_->next);

    ++size_;
}


template<typename T>
typename List<T>::Node* List<T>::insert(Node *node, const T& value) {
    bool isHead = node == head_;
    if (isHead) {
        insert_head(value);
        return head_;
    } else {
        Node* prevNode = node->prev;
        Node* insertedNode = new Node(prevNode, node, value);
        prevNode->next = insertedNode;
        node->prev = insertedNode;

        ++size_;
        return insertedNode;
    }
}

template<typename T>
void List<T>::remove(Node *node) {
    bool isHead = node == head_;
    bool isTail = node == tail_;
    if (isHead && isTail) {
        remove_head();
    } else if (isHead) {
        remove_head();
    } else if (isTail) {
        remove_tail();
    } else {
        Node* prevNode = node->prev;
        Node* nextNode = node->next;
        assert(prevNode);
        assert(nextNode);
        prevNode->next = nextNode;
        nextNode->prev = prevNode;

        delete node;
        --size_;
    }
}

template<typename T>
void List<T>::remove_head() {
    Node* nextHead = head_->next;
    delete head_;
    head_ = nextHead;
    if (head_) {
        head_->prev = nullptr;
    } else {
        assert(size_ == 1);
        tail_ = nullptr;
    }

    --size_;
}

template<typename T>
void List<T>::remove_tail() {
    Node* nextTail = tail_->prev;
    delete tail_;
    tail_ = nextTail;
    if (tail_) {
        tail_->next = nullptr;
    } else {
        assert(size_ == 1);
        head_ = nullptr;
    }

    --size_;
}

template<typename T>
const T &List<T>::head() const {
    return head_->val;
}

template<typename T>
const T &List<T>::tail() const {
    return tail_->val;
}

template<typename T>
int List<T>::size() const {
    return size_;
}

template<typename T>
bool List<T>::empty() const {
    return size_ == 0;
}

template<typename T>
void List<T>::clear() {
    for (Node* deleteIter = head_; deleteIter;) {
        Node* next = deleteIter->next;
        delete deleteIter;
        deleteIter = next;
    }
}

template<typename T>
typename List<T>::Iterator List<T>::iterator() {
    return List::Iterator(this, head_);
}

template<typename T>
const typename List<T>::ConstIterator List<T>::iterator() const {
    return List::ConstIterator(this, head_);
}
