#pragma once

template <typename K, typename V>
class AVL {
private:
    struct Node {
        K key;
        V value;
        int height;
        Node* left;
        Node* right;

        Node(const K& k, const V& v)
            : key(k), value(v), height(1),
              left(nullptr), right(nullptr) {}
    };

    Node* root;
    int nodeCount;

    static int nodeHeight(const Node* node) {
        return node == nullptr ? 0 : node->height;
    }

    static void updateHeight(Node* node) {
        int leftHeight = nodeHeight(node->left);
        int rightHeight = nodeHeight(node->right);

        node->height = 1 + (leftHeight > rightHeight
                            ? leftHeight : rightHeight);
    }

    static int balanceFactor(const Node* node) {
        return nodeHeight(node->left) - nodeHeight(node->right);
    }

    static Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* middle = x->right;

        x->right = y;
        y->left = middle;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    static Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* middle = y->left;

        y->left = x;
        x->right = middle;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    Node* insertNode(Node* node, const K& key,
                     const V& value, bool& inserted) {
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
        else
            return node;

        updateHeight(node);
        int balance = balanceFactor(node);

        if (balance > 1) {
            if (key > node->left->key)
                node->left = rotateLeft(node->left);

            return rotateRight(node);
        }

        if (balance < -1) {
            if (key < node->right->key)
                node->right = rotateRight(node->right);

            return rotateLeft(node);
        }

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
        delete node;
    }

public:
    AVL() : root(nullptr), nodeCount(0) {}

    ~AVL() {
        destroy(root);
    }

    AVL(const AVL&) = delete;
    AVL& operator=(const AVL&) = delete;

    bool insert(const K& key, const V& value) {
        bool inserted = false;
        root = insertNode(root, key, value, inserted);
        return inserted;
    }

    const V* find(const K& key) const {
        const Node* current = root;

        while (current != nullptr) {
            if (key == current->key)
                return &current->value;

            current = key < current->key
                ? current->left : current->right;
        }

        return nullptr;
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
