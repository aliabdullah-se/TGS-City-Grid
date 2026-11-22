#pragma once

template <typename K, typename V>
class BST {
private:
    struct Node {
        K key;
        V* value;
        Node* left;
        Node* right;

        Node(const K& k, V* v)
            : key(k), value(v),
              left(nullptr), right(nullptr) {}
    };

    Node* root;
    int nodeCount;

    Node* insertNode(Node* node, const K& key,
                     V* value, bool& inserted) {
        if (node == nullptr) {
            inserted = true;
            ++nodeCount;
            return new Node(key, value);
        }

        if (key < node->key)
            node->left = insertNode(
                node->left, key, value, inserted);
        else if (key > node->key)
            node->right = insertNode(
                node->right, key, value, inserted);

        return node;
    }

    static Node* detachMin(Node* node, Node*& smallest) {
        if (node->left == nullptr) {
            smallest = node;
            Node* remaining = node->right;
            node->right = nullptr;
            return remaining;
        }

        node->left = detachMin(node->left, smallest);
        return node;
    }

    Node* removeNode(Node* node, const K& key,
                     bool& removed) {
        if (node == nullptr)
            return nullptr;

        if (key < node->key) {
            node->left = removeNode(
                node->left, key, removed);
            return node;
        }

        if (key > node->key) {
            node->right = removeNode(
                node->right, key, removed);
            return node;
        }

        removed = true;
        --nodeCount;

        // Case 1 and 2: zero or one child.
        if (node->left == nullptr ||
            node->right == nullptr) {

            Node* child = node->left != nullptr
                        ? node->left : node->right;

            delete node->value;
            delete node;
            return child;
        }

        // Case 3: two children.
        Node* successor = nullptr;
        node->right = detachMin(node->right, successor);

        delete node->value;

        // Move the successor pointer, not the Incident.
        node->key = successor->key;
        node->value = successor->value;

        delete successor;
        return node;
    }

    template <typename Visitor>
    static void traverseInOrder(const Node* node,
                                Visitor& visit) {
        if (node == nullptr) return;

        traverseInOrder(node->left, visit);
        visit(node->key, node->value);
        traverseInOrder(node->right, visit);
    }

    template <typename Visitor>
    static void traversePreOrder(const Node* node,
                                 Visitor& visit) {
        if (node == nullptr) return;

        visit(node->key, node->value);
        traversePreOrder(node->left, visit);
        traversePreOrder(node->right, visit);
    }

    static void destroy(Node* node) {
        if (node == nullptr) return;

        destroy(node->left);
        destroy(node->right);

        delete node->value;
        delete node;
    }

public:
    BST() : root(nullptr), nodeCount(0) {}

    ~BST() {
        destroy(root);
    }

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    // Ownership transfers only on successful insertion.
    bool insert(const K& key, V* value) {
        bool inserted = false;
        root = insertNode(root, key, value, inserted);
        return inserted;
    }

    V* find(const K& key) const {
        Node* current = root;

        while (current != nullptr) {
            if (key == current->key)
                return current->value;

            current = key < current->key
                    ? current->left : current->right;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        bool removed = false;
        root = removeNode(root, key, removed);
        return removed;
    }

    int size() const {
        return nodeCount;
    }

    template <typename Visitor>
    void inOrder(Visitor visit) const {
        traverseInOrder(root, visit);
    }

    template <typename Visitor>
    void preOrder(Visitor visit) const {
        traversePreOrder(root, visit);
    }
};
