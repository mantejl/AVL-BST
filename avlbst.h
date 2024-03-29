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
    if (this->root_ == nullptr)
    {
        // if it is, then we add new node
        AVLNode<Key, Value> *n = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        this->root_ = n;
        n->setBalance(0);
        return;
    }
    else
    {
        // creating temp node
        AVLNode<Key, Value> *c = static_cast<AVLNode<Key, Value> *>(this->root_);

        // iterating through while true
        while (true)
        {
            // checking if c key is greater
            if (new_item.first < c->getKey())
            {
                // if c getLeft is nullptr, then we add a new node
                if (c->getLeft() == nullptr)
                {
                    AVLNode<Key, Value> *ins2 = new AVLNode<Key, Value>(new_item.first, new_item.second, c);
                    c->setLeft(ins2);
                    ins2->setBalance(0);
                    if (c->getBalance() == 1 || c->getBalance() == -1)
                    {
                        c->setBalance(0);
                    }
                    else if (c->getBalance() == 0)
                    {
                        c->updateBalance(-1);
                        insertFix(c, ins2);
                    }
                    break;
                }
                // if not then we iterate to next left
                else
                {
                    c = c->getLeft();
                }
            }
            // checking if c key is less
            else if (new_item.first > c->getKey())
            {
                // if c getRight is nullptr, then we add a new node
                if (c->getRight() == nullptr)
                {
                    AVLNode<Key, Value> *ins = new AVLNode<Key, Value>(new_item.first, new_item.second, c);
                    ins->setBalance(0);
                    c->setRight(ins);
                    if (c->getBalance() == 1 || c->getBalance() == -1)
                    {
                        c->setBalance(0);
                    }
                    else if (c->getBalance() == 0)
                    {
                        c->updateBalance(1);
                        insertFix(c, ins);
                    }
                    break;
                }
                // if not then we iterate to next left
                else
                {
                    c = c->getRight();
                }
            }
            else // both of them are the same
            {
                c->setValue(new_item.second);
                break;
            }
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

    int8_t diff = 0;
    AVLNode<Key, Value> *p = n->getParent();

    if (p != nullptr)
    {
        if (p->getLeft() == n)
        {
            diff = 1;
        }
        else if (p->getRight() == n)
        {
            diff = -1;
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
    // intial check 
    if ((p->getBalance() == 1 || p->getBalance() == 0 || p->getBalance() == -1) && (n->getBalance() == 1 || n->getBalance() == 0 || n->getBalance() == -1))
    {
        if ((p == nullptr) || (p->getParent() == nullptr))
        {
            return;
        }
        AVLNode<Key, Value> *g = p->getParent();
        // check for left 
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
        // check for right 
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
                if (g->getRight() == p && p->getLeft() == n)
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
    int8_t ndiff = 0;

    if (p != nullptr)
    {
        if (p->getLeft() == n)
        {
            ndiff = 1;
        }
        else
        {
            ndiff = -1;
        }
    }


    // condition when diff == -1
    if (diff == -1)
    {
        if (n->getBalance() + diff == -2)
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

    // condition when diff == 1
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
                removeFix(p, ndiff);
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
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *n)
{
    if (n == nullptr)
    {
        return;
    }

    // getting parent and left 
    AVLNode<Key, Value> *l = n->getLeft();
    AVLNode<Key, Value> *p = n->getParent();
    
    if (l == nullptr)
    {
        return;
    }


    // rotation 
    n->setLeft(l->getRight());

    // changing pointers 
    if (p == nullptr)
    {
        this->root_ = l;
    }
    else if (p->getRight() == n)
    {
        p->setRight(l);
    }
    else if (p->getLeft() == n)
    {
        p->setLeft(l);
    }

    l->setParent(p);
    if (l->getRight() != nullptr)
    {
        l->getRight()->setParent(n);
    }
    n->setParent(l);
    l->setRight(n);
    return;
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *n)
{
    if (n == nullptr)
    {
        return;
    }

    AVLNode<Key, Value> *r = n->getRight();
    AVLNode<Key, Value> *p = n->getParent();

    if (r == nullptr)
    {
        return;
    }

    // rotation 
    n->setRight(r->getLeft());

    // changing pointers 
    if (p == nullptr)
    {
        this->root_ = r;
    }
    else if (p->getRight() == n)
    {
        p->setRight(r);
    }
    else if (p->getLeft() == n)
    {
        p->setLeft(r);
    }

    r->setParent(p);

    if (r->getLeft() != nullptr)
    {
        r->getLeft()->setParent(n);
    }

    n->setParent(r);
    r->setLeft(n);
    return;
}

#endif