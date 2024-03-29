#pragma once

template <class T> class StackLinkedList {
  public:
    struct Node {
        T data;
        Node *next;
    };
    Node *head = nullptr;

  public:
    StackLinkedList() = default;
    StackLinkedList(StackLinkedList &) = delete;
    void push(Node *newNode);
    Node *pop();
};

#include "StackLinkedListImpl.h"
