//
// Created by alexaxnder on 02.12.17.
//

#ifndef DATA_STRUCTURES_RBTREE_H
#define DATA_STRUCTURES_RBTREE_H

#include <algorithm>
#include <iostream>
#include <initializer_list>
#include <stdexcept>
#include <vector>
#include <set>

template <class ValueType>
class Set {
    struct Node;
    enum class Color;

public:
    class iterator:
            public std::iterator<std::bidirectional_iterator_tag, ValueType> {
    public:
        iterator(): cur_node_{nullptr}, set_{nullptr} {};
        explicit iterator(Node* node, const Set* set);

        iterator& operator++();
        iterator operator++(int);

        iterator& operator--();
        iterator operator--(int);

        const ValueType& operator*();
        const ValueType* operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        Node* cur_node_;
        const Set* set_;
    };

    iterator begin() const;
    iterator end() const;

    Set();

    template <class BidirectionalIterator>
    Set(BidirectionalIterator first, BidirectionalIterator last);

    Set(std::initializer_list<ValueType> list);
    Set(const Set<ValueType>& rhs);

    Set& operator=(const Set<ValueType>& rhs);

    ~Set();
    void insert(const ValueType& value);
    void erase(const ValueType& value);
    iterator find(const ValueType& value) const;
    iterator lower_bound(const ValueType& value) const;


    size_t size() const;
    bool empty() const;

private:

    struct Node {
        const ValueType value;
        Color color;
        Node* left;
        Node* right;
        Node* parent;

        explicit Node(ValueType value);
        Node(const ValueType& value, Color color);
        Node(const ValueType&, Color color, Node* left, Node* right, Node* parent);
    };

    enum class Color {
        kRed, kBlack
    };

    Node* TreeMinimum(Node*) const;
    Node* TreeMaximum(Node*) const;
    Node* TreeSuccessor(Node*) const;
    Node* TreePredecessor(Node*) const;
    Node* TreeFind(const ValueType&) const;
    Node* TreeLowerBound(const ValueType&) const;

    bool Geq(const ValueType&, const ValueType&) const;
    bool Greater(const ValueType&, const ValueType&) const;
    bool Eq(const ValueType&, const ValueType&) const;
    bool Neq(const ValueType&, const ValueType&) const;

    bool IsNil(Node*) const;
    bool IsRoot(Node*) const;
    bool IsRed(Node*) const;
    bool IsBlack(Node*) const;
    void SetRed(Node*);
    void SetBlack(Node*);

    void DeleteSubtree(Node*&);

    void LeftRotate(Node*);
    void RightRotate(Node*);

    void RBInsert(Node*&);
    void RBInsertFixup(Node*&);

    void RBTransplant(Node*&, Node*&);
    void RBDelete(Node*&);
    void RBDeleteFixup(Node*&);

    Node* nil_;
    Node* root_;
    size_t size_;
};


/*
 *
 *              Set implementation
 *            Based on Red-Black tree
 *
 *
 *      All variable names like "x_node" are from the book
 *      "Introduction to Algorithms, third edition"
 *      by Cormen et al
 *
 */

template <class ValueType>
Set<ValueType>
::Set():
        nil_{new Node(ValueType(), Color::kBlack)},
        size_{0}
{
    root_ = nil_->left = nil_->right = nil_->parent = nil_;
}

template <class ValueType>
template <class BidirectionalIterator>
Set<ValueType>
::Set(
        BidirectionalIterator first,
        BidirectionalIterator last
):
        Set()
{
    while (first != last) {
        insert(*first);
        ++first;
    }
}

template <class ValueType>
Set<ValueType>
::Set(
        std::initializer_list<ValueType> list
):
        Set(list.begin(), list.end())
{}

template <class ValueType>
Set<ValueType>
::Set(
        const Set& rhs
):
        Set(rhs.begin(), rhs.end())
{
    size_ = rhs.size_;
}

template <class ValueType>
Set<ValueType>&
Set<ValueType>
::operator=(
        const Set& rhs
) {
    if (&rhs == this) {
        return *this;
    }

    DeleteSubtree(root_);
    root_ = nil_;

    auto it = rhs.begin();
    while (it != rhs.end()) {
        insert(*it);
        ++it;
    }

    size_ = rhs.size_;
    return *this;
}

template <class ValueType>
Set<ValueType>
::~Set() {
    if (!IsNil(root_)) {
        DeleteSubtree(root_);
    }

    delete nil_;
}

template <class ValueType>
void
Set<ValueType>
::insert(
        const ValueType& value
) {
    if (!IsNil(TreeFind(value))) {
        return;
    }

    auto val_node = new Node(value);
    return RBInsert(val_node);
}

template <class ValueType>
void
Set<ValueType>
::erase(
        const ValueType& value
) {
    auto val_node = TreeFind(value);
    if (IsNil(val_node)) {
        return;
    }

    return RBDelete(val_node);
}

template <class ValueType>
typename Set<ValueType>::iterator
Set<ValueType>
::find(
        const ValueType& value
) const {
    return iterator(TreeFind(value), this);
}

template <class ValueType>
typename Set<ValueType>::iterator
Set<ValueType>
::lower_bound(
        const ValueType &value
) const {
    return iterator(TreeLowerBound(value), this);
}

template <class ValueType>
size_t
Set<ValueType>
::size() const {
    return size_;
}

template <class ValueType>
bool
Set<ValueType>
::empty() const {
    return size_ == 0;
}

template <class ValueType>
typename Set<ValueType>::Node*
Set<ValueType>
::TreeMinimum(
        Node* root
) const {
    Node* cur = root;

    while (!IsNil(cur->left)) {
        cur = cur->left;
    }

    return cur;
}

template <class ValueType>
typename Set<ValueType>::Node*
Set<ValueType>
::TreeMaximum(
        Node* root
) const {
    Node* cur = root;

    while (!IsNil(cur->right)) {
        cur = cur->right;
    }

    return cur;
}

template <class ValueType>
typename Set<ValueType>::Node*
Set<ValueType>
::TreeSuccessor(
        Node* root
) const {
    if (!IsNil(root->right)) {
        return TreeMinimum(root->right);
    }

    auto y_node = root->parent;
    while (!IsNil(y_node) && root == y_node->right) {
        root = y_node;
        y_node = y_node->parent;
    }

    return y_node;
}

template <class ValueType>
typename Set<ValueType>::Node*
Set<ValueType>
::TreePredecessor(
        Node* root
) const {
    if (!IsNil(root->left)) {
        return TreeMaximum(root->left);
    }

    auto y_node = root->parent;
    while (!IsNil(y_node) && root == y_node->left) {
        root = y_node;
        y_node = y_node->parent;
    }

    return y_node;
}

template <class ValueType>
typename Set<ValueType>::Node*
Set<ValueType>
::TreeFind(
        const ValueType& value
) const {
    auto cur = root_;
    while (!IsNil(cur) && Neq(cur->value, value)) {
        if (value < cur->value) {
            cur = cur->left;

        } else {
            cur = cur->right;
        }
    }

    return cur;
}

template <class ValueType>
typename Set<ValueType>::Node*
Set<ValueType>
::TreeLowerBound(
        const ValueType& value
) const {
    Node* cur = root_;
    Node* cur_parent = cur;

    while (!IsNil(cur) && Neq(cur->value, value)) {
        cur_parent = cur;
        if (value < cur->value) {
            cur = cur->left;

        } else {
            cur = cur->right;
        }
    }

    if (!IsNil(cur)) {
        return cur;
    }

    if (cur_parent->value < value) {
        return TreeSuccessor(cur_parent);
    }

    return cur_parent;
}

template <class ValueType>
bool
Set<ValueType>
::Greater(
        const ValueType& lhs,
        const ValueType& rhs
) const {
    return rhs < lhs;
}

template <class ValueType>
bool
Set<ValueType>
::Geq(
        const ValueType& lhs,
        const ValueType& rhs
) const {
    return Greater(lhs, rhs) || Eq(lhs, rhs);
}

template <class ValueType>
bool
Set<ValueType>
::Eq(
        const ValueType& lhs,
        const ValueType& rhs
) const {
    return !(lhs < rhs) && !(rhs < lhs);
}

template <class ValueType>
bool
Set<ValueType>
::Neq(
        const ValueType& lhs,
        const ValueType& rhs
) const {
    return !Eq(lhs, rhs);
}

template <class ValueType>
bool
Set<ValueType>
::IsNil(
        Node* node
) const {
    return node == nil_;
}

template <class ValueType>
bool
Set<ValueType>
::IsRoot(
        Node* node
) const {
    return node == root_;
}

template <class ValueType>
bool
Set<ValueType>
::IsRed(
        Node* node
) const {
    return node->color == Color::kRed;
}

template <class ValueType>
bool
Set<ValueType>
::IsBlack(
        Node* node
) const {
    return node->color == Color::kBlack;
}

template <class ValueType>
void
Set<ValueType>
::SetRed(
        Node* node
) {
    node->color = Color::kRed;
}

template <class ValueType>
void
Set<ValueType>
::SetBlack(
        Node* node
) {
    node->color = Color::kBlack;
}

template <class ValueType>
void
Set<ValueType>
::DeleteSubtree(
        Node*& root
) {
    if (!IsNil(root->left)) {
        DeleteSubtree(root->left);
    }

    if (!IsNil(root->right)) {
        DeleteSubtree(root->right);
    }

    delete root;
}

template <class ValueType>
void
Set<ValueType>
::LeftRotate(
        Node* x_node
) {
    if (!x_node->right) {
        throw std::logic_error(
                "Trying to rotate left node without right child!");
    }

    Node* y_node = x_node->right;
    x_node->right = y_node->left;
    if (!IsNil(y_node->left)) {
        y_node->left->parent = x_node;
    }

    y_node->parent = x_node->parent;
    if (IsNil(x_node->parent)) {
        root_ = y_node;

    } else if (x_node == x_node->parent->left) {
        x_node->parent->left = y_node;

    } else {
        x_node->parent->right = y_node;
    }

    y_node->left = x_node;
    x_node->parent = y_node;
}

template <class ValueType>
void
Set<ValueType>
::RightRotate(
        Node* x_node
) {
    if (!x_node->left) {
        throw std::logic_error(
                "Trying to rotate right node without left child!");
    }

    auto y_node = x_node->left;
    x_node->left = y_node->right;
    if (!IsNil(y_node->right)) {
        y_node->right->parent = x_node;
    }

    y_node->parent = x_node->parent;

    if (IsNil(x_node->parent)) {
        root_ = y_node;

    } else if (x_node == x_node->parent->left) {
        x_node->parent->left = y_node;

    } else {
        x_node->parent->right = y_node;
    }

    y_node->right = x_node;
    x_node->parent = y_node;
}

template <class ValueType>
void
Set<ValueType>
::RBInsert(
        Node*& z_node
) {
    auto y_node = nil_;
    auto x_node = root_;
    while (!IsNil(x_node)) {
        y_node = x_node;
        if (z_node->value < x_node->value) {
            x_node = x_node->left;

        } else {
            x_node = x_node->right;
        }
    }

    z_node->parent = y_node;
    if (IsNil(y_node)) {
        root_ = z_node;

    } else if (z_node->value < y_node->value) {
        y_node->left = z_node;

    } else {
        y_node->right = z_node;
    }

    z_node->left = nil_;
    z_node->right = nil_;
    SetRed(z_node);
    RBInsertFixup(z_node);
    ++size_;
}

template <class ValueType>
void
Set<ValueType>
::RBInsertFixup(
        Node*& z_node
) {
    while (IsRed(z_node->parent)) {
        if (z_node->parent == z_node->parent->parent->left) {
            auto y_node = z_node->parent->parent->right;
            if (IsRed(y_node)) {
                SetBlack(z_node->parent);
                SetBlack(y_node);
                SetRed(z_node->parent->parent);
                z_node = z_node->parent->parent;

            } else {
                if (z_node == z_node->parent->right) {
                    z_node = z_node->parent;
                    LeftRotate(z_node);
                }

                SetBlack(z_node->parent);
                SetRed(z_node->parent->parent);
                RightRotate(z_node->parent->parent);
            }

        } else {
            auto y_node = z_node->parent->parent->left;
            if (IsRed(y_node)) {
                SetBlack(z_node->parent);
                SetBlack(y_node);
                SetRed(z_node->parent->parent);
                z_node = z_node->parent->parent;

            } else {
                if (z_node == z_node->parent->left) {
                    z_node = z_node->parent;
                    RightRotate(z_node);
                }

                SetBlack(z_node->parent);
                SetRed(z_node->parent->parent);
                LeftRotate(z_node->parent->parent);
            }
        }
    }

    SetBlack(root_);
}

template <class ValueType>
void
Set<ValueType>
::RBTransplant(
        Node*& u_node,
        Node*& v_node
) {
    if (IsNil(u_node->parent)) {
        root_ = v_node;

    } else if (u_node == u_node->parent->left) {
        u_node->parent->left = v_node;

    } else {
        u_node->parent->right = v_node;
    }

    v_node->parent = u_node->parent;
}

template <class ValueType>
void
Set<ValueType>
::RBDelete(
        Node*& z_node
) {
    auto x_node = nil_;
    auto y_node = z_node;
    auto y_original_color = y_node->color;

    if (IsNil(z_node->left)) {
        x_node = z_node->right;
        RBTransplant(z_node, z_node->right);

    } else if (IsNil(z_node->right)) {
        x_node = z_node->left;
        RBTransplant(z_node, z_node->left);

    } else {
        y_node = TreeMinimum(z_node->right);
        y_original_color = y_node->color;
        x_node = y_node->right;
        if (y_node->parent == z_node) {
            x_node->parent = y_node;

        } else {
            RBTransplant(y_node, y_node->right);
            y_node->right = z_node->right;
            y_node->right->parent = y_node;
        }

        RBTransplant(z_node, y_node);
        y_node->left = z_node->left;
        y_node->left->parent = y_node;
        y_node->color = z_node->color;
    }

    if (y_original_color == Color::kBlack) {
        RBDeleteFixup(x_node);
    }

    delete z_node;
    --size_;
}

template <class ValueType>
void
Set<ValueType>
::RBDeleteFixup(
        Node*& x_node
) {
    while (!IsRoot(x_node) && IsBlack(x_node)) {
        if (x_node == x_node->parent->left) {
            auto w_node = x_node->parent->right;
            if (IsRed(w_node)) {
                SetBlack(w_node);
                SetRed(x_node->parent);
                LeftRotate(x_node->parent);
                w_node = x_node->parent->right;
            }

            if (IsBlack(w_node->left) &&
                IsBlack(w_node->right)) {

                SetRed(w_node);
                x_node = x_node->parent;

            } else {
                if (IsBlack(w_node->right)) {
                    SetBlack(w_node->left);
                    SetRed(w_node);
                    RightRotate(w_node);
                    w_node = x_node->parent->right;
                }

                w_node->color = x_node->parent->color;
                SetBlack(x_node->parent);
                SetBlack(w_node->right);
                LeftRotate(x_node->parent);
                x_node = root_;
            }

        } else {
            auto w_node = x_node->parent->left;
            if (IsRed(w_node)) {
                SetBlack(w_node);
                SetRed(x_node->parent);
                RightRotate(x_node->parent);
                w_node = x_node->parent->left;
            }

            if (IsBlack(w_node->left) &&
                IsBlack(w_node->right)) {

                SetRed(w_node);
                x_node = x_node->parent;

            } else {
                if (IsBlack(w_node->left)) {
                    SetBlack(w_node->right);
                    SetRed(w_node);
                    LeftRotate(w_node);
                    w_node = x_node->parent->left;
                }

                w_node->color = x_node->parent->color;
                SetBlack(x_node->parent);
                SetBlack(w_node->left);
                RightRotate(x_node->parent);
                x_node = root_;
            }
        }
    }

    SetBlack(x_node);
}

/*
 *
 *      Iterator implementation
 *
 */

template <class ValueType>
typename Set<ValueType>::iterator
Set<ValueType>
::begin() const {
    return iterator(TreeMinimum(root_), this);
}

template <class ValueType>
typename Set<ValueType>::iterator
Set<ValueType>
::end() const {
    return iterator(nil_, this);
}

template <class ValueType>
Set<ValueType>::iterator
::iterator(
        Node* node,
        const Set* set
):
        cur_node_{node},
        set_{set}
{}

template <class ValueType>
const ValueType&
Set<ValueType>::iterator
::operator*() {
    return cur_node_->value;
}

template <class ValueType>
const ValueType*
Set<ValueType>::iterator
::operator->() {
    return &(cur_node_->value);
}

template <class ValueType>
typename Set<ValueType>::iterator&
Set<ValueType>::iterator
::operator++() {
    cur_node_ = set_->TreeSuccessor(cur_node_);
    return *this;
}

template <class ValueType>
typename Set<ValueType>::iterator
Set<ValueType>::iterator
::operator++(int dummy) {
    iterator cpy(cur_node_, set_);
    this->operator++();
    return cpy;
}

template <class ValueType>
typename Set<ValueType>::iterator&
Set<ValueType>::iterator
::operator--() {
    if (set_->IsNil(cur_node_)) {
        cur_node_ = set_->TreeMaximum(set_->root_);

    } else {
        cur_node_ = set_->TreePredecessor(cur_node_);
    }

    return *this;
}

template <class ValueType>
typename Set<ValueType>::iterator
Set<ValueType>::iterator
::operator--(int dummy) {
    iterator cpy(cur_node_, set_);
    this->operator--();
    return cpy;
}

template <class ValueType>
bool
Set<ValueType>::iterator
::operator==(
        const iterator& rhs
) const {
    return cur_node_ == rhs.cur_node_;
}

template <class ValueType>
bool
Set<ValueType>::iterator
::operator!=(
        const iterator& rhs
) const {
    return cur_node_ != rhs.cur_node_;
}

/*
 *
 *      Node implementation
 *
 */

template <class ValueType>
Set<ValueType>::Node
::Node(
        ValueType value
):
        Node(value, Color::kRed)
{}

template <class ValueType>
Set<ValueType>::Node
::Node(
        const ValueType& value,
        Color color
):
        Node(value, color, nullptr, nullptr, nullptr)
{}

template <class ValueType>
Set<ValueType>::Node
::Node(
        const ValueType& value,
        Color color,
        Node *left,
        Node *right,
        Node *parent
):
        value{value},
        color{color},
        left{left},
        right{right},
        parent{parent}
{}


#endif //DATA_STRUCTURES_RBTREE_H
