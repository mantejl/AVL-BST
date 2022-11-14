#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError
{
};

/**
 * A special kind of node for an AVL tree, which adds the balance as a data member, plus
 * other additional helper functions. You do NOT need to implement any functionality or
 * add additional data members or helper functions.
 */
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value> *getParent() const override;
    virtual AVLNode<Key, Value> *getLeft() const override;
    virtual AVLNode<Key, Value> *getRight() const override;

protected:
    int8_t balance_; // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class constructor and setting
 * the color to red since every new node will be red when it is first inserted.
 */
template <class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent) : Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
 * A destructor which does nothing.
 */
template <class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

/**
 * A getter for the balance of a AVLNode.
 */
template <class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
 * A setter for the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
 * Adds diff to the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
 * An overridden function for getting the parent since a static_cast is necessary to make sure
 * that our node is a AVLNode.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value> *>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value> *>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value> *>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key &key);                              // TODO
protected:
    virtual void nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2);

    // Add helper functions here
    void removeFix(AVLNode<Key, Value> *p, int8_t diff);
    void insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n);
    void rotateLeft(AVLNode<Key, Value> *p);
    void rotateRight(AVLNode<Key, Value> *p);
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    if (this->root_ == NULL)
    {
        AVLNode<Key, Value> *new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        this->root_ = new_node;
        return;
    }
    else
    {
        AVLNode<Key, Value> *root_temp = static_cast<AVLNode<Key, Value> *>(this->root_);
        while (1)
        {
            if (new_item.first > root_temp->getKey())
            {
                // there is no right child
                if (root_temp->getRight() == NULL)
                {
                    // then we can make a new node
                    AVLNode<Key, Value> *new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, root_temp);
                    root_temp->setRight(new_node);
                    break; // done with inserting
                }
                root_temp = root_temp->getRight();
            }
            else if (new_item.first < root_temp->getKey())
            {
                if (root_temp->getLeft() == NULL)
                {
                    AVLNode<Key, Value> *new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, root_temp);
                    root_temp->setLeft(new_node);
                    break;
                }
                root_temp = root_temp->getLeft();
            }
            else
            {
                // overwrite the current value with the updated value
                root_temp->setValue(new_item.second);
                break;
            }
        }
    }
    AVLNode<Key, Value> *new_node = static_cast<AVLNode<Key, Value> *>(BinarySearchTree<Key, Value>::internalFind(new_item.first));
    AVLNode<Key, Value> *parent = new_node->getParent();
    new_node->setBalance(0);
    if (parent == NULL)
    {
        return;
    }
    if (parent->getBalance() == -1 || parent->getBalance() == 1)
    {
        parent->setBalance(0);
    }
    else if (parent->getBalance() == 0)
    {
        if (new_node->getKey() < parent->getKey())
        {
            parent->updateBalance(-1);
            if (parent->getParent() == NULL)
            {
                return;
            }
            insertFix(parent, new_node);
        }
        else if (new_node->getKey() > parent->getKey())
        {
            parent->updateBalance(1);
            if (parent->getParent() == NULL)
            {
                return;
            }
            parent->getParent()->updateBalance(1);
            insertFix(parent, new_node);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key &key)
{
    // root check
    if (this->root_ == nullptr)
    {
        return;
    }

    // finding the node
    AVLNode<Key, Value> *n = static_cast<AVLNode<Key, Value> *>(this->internalFind(key));

    // if n is nullptr then we return
    if (n == nullptr)
    {
        return;
    }

    // case for when there are 2 children
    // having this first bc after swap we will either be in a 0-child or 1-child case
    if (n->getLeft() != nullptr && n->getRight() != nullptr)
    {
        nodeSwap(n, static_cast<AVLNode<Key, Value> *>(this->predecessor(n)));
    }

    AVLNode<Key, Value> *p = n->getParent();

    int8_t diff;
    if (p != nullptr)
    {
        if (p->getLeft() == n)
        {
            diff = 1;
            p->updateBalance(diff);
        }
        else if (p->getRight() == n)
        {
            diff = 1;
            p->updateBalance(diff);
        }
    }

    // case for when there is one child and 0 children
    // creating temp node to store it
    AVLNode<Key, Value> *one;

    // 0 children
    if (n->getRight() == nullptr && n->getLeft() == nullptr)
    {
        one = nullptr;
    }

    // one child
    if (n->getRight() == nullptr && n->getLeft() != nullptr)
    {
        one = n->getLeft();
    }
    else if (n->getRight() != nullptr && n->getLeft() == nullptr)
    {
        one = n->getRight();
    }

    // updating the parent node to complete the promote functionality
    AVLNode<Key, Value> *two = n->getParent();
    if (two == nullptr)
    {
        this->root_ = one;
    }
    else
    {
        if (n->getParent()->getLeft() == n)
        {
            two->setLeft(one);
        }
        else if (n->getParent()->getRight() == n)
        {
            two->setRight(one);
        }
    }

    // check to set the parent of one to two
    if (one != nullptr)
    {
        one->setParent(two);
    }
    // deleting c
    delete n;

    removeFix(p, diff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template <class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n)
{
    if ((p->getBalance() == 1 || p->getBalance() == -1 || p->getBalance() == 0) && (n->getBalance() == 1 || n->getBalance() == -1 || n->getBalance() == 0))
    {
        if (p == nullptr || p->getParent() == nullptr)
        {
            return;
        }

        AVLNode<Key, Value> *g = p->getParent();
        if (g->getLeft() == p)
        {
            g->updateBalance(-1);
            if (g->getBalance() == 0)
            {
                return;
            }
            else if (g->getBalance() == -1)
            {
                insertFix(g, p);
            }
            else if (g->getBalance() == -2)
            {
                if (g->getLeft() == p && p->getLeft() == n)
                {
                    rotateRight(g);
                    g->setBalance(0);
                    p->setBalance(0);
                }
                else if (g->getLeft() == p && p->getRight() == n)
                {
                    rotateLeft(p);
                    rotateRight(g);
                    if (n->getBalance() == -1)
                    {
                        p->setBalance(0);
                        g->setBalance(1);
                        n->setBalance(0);
                    }
                    else if (n->getBalance() == 0)
                    {
                        p->setBalance(0);
                        g->setBalance(0);
                        n->setBalance(0);
                    }
                    else if (n->getBalance() == 1)
                    {
                        p->setBalance(-1);
                        g->setBalance(0);
                        n->setBalance(0);
                    }
                }
            }
        }
        else if (g->getRight() == p)
        {
            g->updateBalance(1);
            if (g->getBalance() == 0)
            {
                return;
            }
            else if (g->getBalance() == 1)
            {
                insertFix(g, p);
            }
            else if (g->getBalance() == 2)
            {
                if (g->getRight() == p && p->getRight() == n)
                {
                    rotateLeft(g);
                    g->setBalance(0);
                    p->setBalance(0);
                }
                else if (g->getRight() == p && p->getLeft() == n)
                {
                    rotateRight(p);
                    rotateLeft(g);
                    if (n->getBalance() == 1)
                    {
                        p->setBalance(0);
                        g->setBalance(-1);
                        n->setBalance(0);
                    }
                    else if (n->getBalance() == 0)
                    {
                        p->setBalance(0);
                        g->setBalance(0);
                        n->setBalance(0);
                    }
                    else if (n->getBalance() == -1)
                    {
                        p->setBalance(1);
                        g->setBalance(0);
                        n->setBalance(0);
                    }
                }
            }
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> *n, int8_t diff)
{
    if (n == nullptr)
    {
        return;
    }
    AVLNode<Key, Value> *p = n->getParent();

    int8_t ndiff;
    if (p == nullptr)
    {
        return;
    }
    else if (p->getLeft() == n)
    {
        ndiff = 1;
    }
    else if (p->getRight() == n)
    {
        ndiff = -1;
    }

    if (diff == -1)
    {
        if (n->getBalance() + ndiff == -2)
        {
            AVLNode<Key, Value> *c = n->getLeft();
            if (c->getBalance() == -1)
            {
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (c->getBalance() == 0)
            {
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            else if (c->getBalance() == 1)
            {
                AVLNode<Key, Value> *g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if (g->getBalance() == 1)
                {
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == -1)
                {
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == -1)
        {
            n->setBalance(-1);
        }
        else if (n->getBalance() + diff == 0)
        {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if (diff == 1)
    {
        if (n->getBalance() + diff == 2)
        {
            AVLNode<Key, Value> *c = n->getRight();
            if (c->getBalance() == 1)
            {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
            }
            else if (c->getBalance() == 0)
            {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else if (c->getBalance() == -1)
            {
                AVLNode<Key, Value> *g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if (g->getBalance() == -1)
                {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 1)
                {
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == 1)
        {
            n->setBalance(1);
        }
        else if (n->getBalance() + diff == 0)
        {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *cur)
{
    // rotateLeft in zig zig case at grandparent
    if (cur == NULL)
    {
        return;
    }
    AVLNode<Key, Value> *temp = cur->getRight();
    AVLNode<Key, Value> *p = cur->getParent();
    if (temp == NULL)
    {
        return;
    }
    cur->setRight(temp->getLeft()); // zig zag case
    if (p == NULL)
    {
        BinarySearchTree<Key, Value>::root_ = temp;
    }
    else if (p->getLeft() == cur)
    {
        p->setLeft(temp);
    }
    else if (p->getRight() == cur)
    {
        p->setRight(temp);
    }
    temp->setParent(p);
    cur->setParent(temp);
    temp->setLeft(cur);
    return;
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *cur)
{
    // rotateRight in zig zig case at grandparent and zig zag at
    if (cur == NULL)
    {
        return;
    }
    AVLNode<Key, Value> *temp = cur->getLeft();
    AVLNode<Key, Value> *p = cur->getParent();
    if (temp == NULL)
    {
        return;
    }
    cur->setLeft(temp->getRight());
    if (p == NULL)
    { // zig zig
        BinarySearchTree<Key, Value>::root_ = temp;
    }
    else if (p->getLeft() == cur)
    {
        p->setLeft(temp);
    }
    else if (p->getRight() == cur)
    {
        p->setRight(temp);
    }
    temp->setParent(p);
    temp->setRight(cur);
    cur->setParent(temp);
    return;
}

#endif
