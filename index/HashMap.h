#pragma once

template <typename K, typename V>
class HashMap {
private:
    struct Node {
        K key;
        V* value;
        Node* next;

        Node(const K& k, V* v, Node* n)
            : key(k), value(v), next(n) {}
    };

    Node** buckets;
    int bucketCapacity;
    int entryCount;

    static int bucketIndex(const K& key, int cap) {
        return static_cast<int>(key % cap);
    }

    static bool isPrime(int number) {
        if (number < 2) return false;

        for (int d = 2; d <= number / d; ++d) {
            if (number % d == 0) return false;
        }
        return true;
    }

    static int nextPrime(int minimum) {
        while (!isPrime(minimum))
            ++minimum;

        return minimum;
    }

    void resize() {
        int newCapacity = nextPrime(bucketCapacity * 2);
        Node** newBuckets = new Node*[newCapacity]();

        for (int i = 0; i < bucketCapacity; ++i) {
            Node* current = buckets[i];

            while (current != nullptr) {
                Node* next = current->next;
                int index = bucketIndex(
                    current->key, newCapacity);

                current->next = newBuckets[index];
                newBuckets[index] = current;
                current = next;
            }
        }

        delete[] buckets;
        buckets = newBuckets;
        bucketCapacity = newCapacity;
    }

public:
    HashMap()
        : buckets(new Node*[11]()),
          bucketCapacity(11),
          entryCount(0) {}

    ~HashMap() {
        for (int i = 0; i < bucketCapacity; ++i) {
            Node* current = buckets[i];

            while (current != nullptr) {
                Node* next = current->next;
                delete current;
                current = next;
            }
        }

        delete[] buckets;
    }

    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;

    bool insert(const K& key, V* value) {
        int index = bucketIndex(key, bucketCapacity);
        Node* current = buckets[index];

        while (current != nullptr) {
            if (current->key == key)
                return false;

            current = current->next;
        }

        buckets[index] = new Node(
            key, value, buckets[index]);

        ++entryCount;

        // Resize only when load factor exceeds 0.75.
        if (entryCount * 4 > bucketCapacity * 3)
            resize();

        return true;
    }

    V* find(const K& key) const {
        int index = bucketIndex(key, bucketCapacity);
        Node* current = buckets[index];

        while (current != nullptr) {
            if (current->key == key)
                return current->value;

            current = current->next;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        int index = bucketIndex(key, bucketCapacity);
        Node* current = buckets[index];
        Node* previous = nullptr;

        while (current != nullptr) {
            if (current->key == key) {
                if (previous == nullptr)
                    buckets[index] = current->next;
                else
                    previous->next = current->next;

                // Delete the hash node, not the Incident.
                delete current;
                --entryCount;
                return true;
            }

            previous = current;
            current = current->next;
        }

        return false;
    }

    int capacity() const {
        return bucketCapacity;
    }

    int size() const {
        return entryCount;
    }
};
