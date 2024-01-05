#pragma once

#include "SinglyLinkedList.h"

template <class T> SinglyLingedList<T>::SinglyLingedList() {}

template <class T>
void SinglyLingedList<T>::insert(Node *previousNode, Node *newNode) {
    if (previousNode == nullptr) {
        if (head != nullptr) {
            newNode->next = head;
        } else {
            newNode->next = nullptr;
        }
        head = newNode;
    } else {
        newNode->next = previousNode->next;
        previousNode->next = newNode;
    }
}

template <class T>
void SinglyLingedList<T>::remove(Node *previousNode, Node *deleteNode) {
    if (previousNode == nullptr) {
        head = deleteNode->next;
    } else {
        previousNode->next = deleteNode->next;
    }
}
