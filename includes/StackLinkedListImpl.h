#include "StackLinkedList.h"

// 因为这里实现的是内存块先入后出（先被free的作为头节点，在分配时也先被分出去），所以叫StackLinkedList
template <class T> void StackLinkedList<T>::push(Node *newNode) {
    newNode->next = head;
    head = newNode;
}

template <class T>
typename StackLinkedList<T>::Node *StackLinkedList<T>::pop() {
    Node *top = head;
    head = head->next;
    return top;
}
