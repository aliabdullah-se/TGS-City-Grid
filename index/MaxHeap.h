#pragma once
#include "Incident.h"

template <typename T>
class MaxHeap {
private:
    T* items;
    int itemCount;
    int heapCapacity;

    static bool higher(T first, T second) {
        return higherPriority(first, second);
    }

    void exchange(int a, int b) {
        T temp = items[a];
        items[a] = items[b];
        items[b] = temp;
    }

    void grow() {
        int newCapacity = heapCapacity * 2;
        T* replacement = new T[newCapacity];

        for (int i = 0; i < itemCount; ++i)
            replacement[i] = items[i];

        delete[] items;
        items = replacement;
        heapCapacity = newCapacity;
    }

    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (!higher(items[index], items[parent]))
                break;

            exchange(index, parent);
            index = parent;
        }
    }

    void siftDown(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int best = index;

            if (left < itemCount &&
                higher(items[left], items[best]))
                best = left;

            if (right < itemCount &&
                higher(items[right], items[best]))
                best = right;

            if (best == index)
                break;

            exchange(index, best);
            index = best;
        }
    }

public:
    MaxHeap()
        : items(new T[16]),
          itemCount(0),
          heapCapacity(16) {}

    ~MaxHeap() {
        // Incident objects belong to the BST.
        delete[] items;
    }

    MaxHeap(const MaxHeap&) = delete;
    MaxHeap& operator=(const MaxHeap&) = delete;

    void insert(T value) {
        if (itemCount == heapCapacity)
            grow();

        items[itemCount] = value;
        ++itemCount;

        siftUp(itemCount - 1);
    }

    T peek() const {
        if (itemCount == 0)
            return nullptr;

        return items[0];
    }

    T extractMax() {
        if (itemCount == 0)
            return nullptr;

        T result = items[0];
        --itemCount;

        if (itemCount > 0) {
            items[0] = items[itemCount];
            siftDown(0);
        }

        items[itemCount] = nullptr;
        return result;
    }

    bool removeById(int id) {
        int index = -1;

        // Assignment requires linear search.
        for (int i = 0; i < itemCount; ++i) {
            if (items[i]->id == id) {
                index = i;
                break;
            }
        }

        if (index == -1)
            return false;

        --itemCount;

        if (index != itemCount) {
            items[index] = items[itemCount];
            items[itemCount] = nullptr;

            if (index > 0 &&
                higher(items[index],
                       items[(index - 1) / 2])) {
                siftUp(index);
            } else {
                siftDown(index);
            }
        } else {
            items[itemCount] = nullptr;
        }

        return true;
    }

    int size() const {
        return itemCount;
    }

    int capacity() const {
        return heapCapacity;
    }
};
