# FROM[memory-allocators](https://github.com/mtrebi/memory-allocators/tree/master)

## CAllocator

普通内存分配器，使用malloc和free， 它的问题是使用malloc时需要从用户态进入内核态，效率很低。

## LinearAllocator

没有针对每次分配的单独FREE方法，FREE时必须全部一同FREE。

## StackAllocator

栈内存分配器，只能先分配的后释放。

## PoolAllocator

内存池，内存被分成一个个大小固定的小块，使用链表将他们组织起来，每次分配的内存大小是固定的提前设置好的chuncksize。

## FreeListAllocator

### 链表实现

空闲的地址快有序地被链表组织起来。
