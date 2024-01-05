#pragma once

template <class T> class SinglyLingedList {
  public:
    struct Node {
        T data;
        Node *next;
    };

    Node *head = nullptr;

  public:
    SinglyLingedList();

    void insert(Node *previousNode, Node *newNode);
    void remove(Node *previousNode, Node *deleteNode);
};

#include "SinglyLinkedListImpl.h"
