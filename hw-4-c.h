#include <iostream>
#include <vector>

typedef enum {BLACK, RED} NodeColor;

template<class ValueType>
class Set {
public:
    struct Node {
        Node *left, *right, *parent;
        ValueType value;
        NodeColor color;
    };

    Node* root;
    size_t elems;

    class iterator {
    public:
        const Set* parent;
        const Node* data;

        iterator() {
            parent = NULL;
            data = NULL;
        }

        iterator(const Set* st) {
            data = NULL;
            parent = st;
        }

        friend bool operator==(const iterator& left, const iterator& right) {
            return left.data == right.data && left.parent == right.parent;
        }

        friend bool operator!=(const iterator& left, const iterator& right) {
            return left.data != right.data || left.parent != right.parent;
        }

        const ValueType* operator->() {
            return &(data->value);
        }

        const ValueType& operator*() {
            return data->value;
        }

        iterator& operator++() {
            ValueType old = data->value;
            if (data->right != NULL) {
                data = data->right;
                while (data->left != NULL)
                    data = data->left;
            } else {
                while (data != NULL && !(old < data->value)) {
                    data = data->parent;
                }
            }
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator& operator--() {
            if (data == NULL) {
                data = parent->root;
                if (data == NULL)
                    return *this;
                while (data->right != NULL)
                    data = data->right;
                return *this;
            }
            ValueType old = data->value;
            if (data->left != NULL) {
                data = data->left;
                while (data->right != NULL)
                    data = data->right;
            } else {
                while (data != NULL && !(data->value < old)) {
                    data = data->parent;
                }
            }
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
    };

    Set() {
        root = NULL;
        elems = 0;
    }

    template<class Iterator>
    Set(Iterator begin, Iterator end) {
        root = NULL;
        elems = 0;
        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }

    Set(const std::initializer_list<ValueType>& ls) {
        root = NULL;
        elems = 0;
        for (auto elem : ls) {
            insert(elem);
        }
    }

    Set(const Set& other) {
        root = NULL;
        *this = other;
    }

    Set& operator=(const Set& other) {
        if (this == &other)
            return *this;
        elems = other.elems;
        clear(root);
        if (other.root == NULL) {
            root = NULL;
            return *this;
        }
        root = new Node();
        root->parent = NULL;
        notStdCopy(other.root, root);
        return *this;
    }


    // PCF is not very clever
    void notStdCopy(Node* from, Node* to) {
        to->value = from->value;
        to->color = from->color;

        if (from->left != NULL) {
            to->left = new Node();
            to->left->parent = to;
            notStdCopy(from->left, to->left);
        } else {
            to->left = NULL;
        }
        if (from->right != NULL) {
            to->right = new Node();
            to->right->parent = to;
            notStdCopy(from->right, to->right);
        } else {
            to->right = NULL;
        }
    }

    size_t size() const {
        return elems;
    }

    bool empty() const {
        return elems == 0;
    }

    iterator begin() const {
        Node* foo = root;
        if (root == NULL)
            return iterator(this);

        while (foo->left != NULL)
            foo = foo->left;
        iterator it;
        it.parent = this;
        it.data = foo;
        return it;
    }

    iterator end() const {
        return iterator(this);
    }

    void rotateLeft(Node* no) {
        Node* tmp = no->right;

        tmp->parent = no->parent;
        if (tmp->parent == NULL)
            root = tmp;

        if (no->parent != NULL) {
            if (no->parent->left == no)
                no->parent->left = tmp;
            else
                no->parent->right = tmp;
        }

        no->right = tmp->left;
        if (tmp->left != NULL)
            tmp->left->parent = no;

        no->parent = tmp;
        tmp->left = no;
    }

    void rotateRight(Node* no) {
        Node* tmp = no->left;

        tmp->parent = no->parent;
        if (tmp->parent == NULL)
            root = tmp;

        if (no->parent != NULL) {
            if (no->parent->left == no)
                no->parent->left = tmp;
            else
                no->parent->right = tmp;
        }

        no->left = tmp->right;
        if (tmp->right != NULL)
            tmp->right->parent = no;

        no->parent = tmp;
        tmp->right = no;
    }

    void fixInsert(Node* no) {
        Node* bar;
        while (no != root && no->parent->color == RED) {
            if (no->parent->parent == NULL)
                break;
            if (no->parent->parent->left == no->parent) {
                bar = no->parent->parent->right;
                if (bar == NULL || bar->color == BLACK) {
                    if (no == no->parent->right) {
                        rotateLeft(no->parent);
                        no = no->left;
                    }

                    no->parent->color = BLACK;
                    no->parent->parent->color = RED;
                    rotateRight(no->parent->parent);
                } else {
                    bar->color = BLACK;
                    no->parent->color = BLACK;
                    no->parent->parent->color = RED;
                    no = no->parent->parent;
                }
            } else {
                bar = no->parent->parent->left;
                if (bar == NULL || bar->color == BLACK) {
                    if (no == no->parent->left) {
                        rotateRight(no->parent);
                        no = no->right;
                    }

                    no->parent->color = BLACK;
                    no->parent->parent->color = RED;
                    rotateLeft(no->parent->parent);
                } else {
                    bar->color = BLACK;
                    no->parent->color = BLACK;
                    no->parent->parent->color = RED;
                    no = no->parent->parent;
                }
            }
        }
        root->color = BLACK;
    }

    void insert(ValueType data) {
        Node *foo = root, *parent = NULL;

        while (foo != NULL) {
            parent = foo;
            if (!(data < foo->value || foo->value < data))
                return;
            if (data < foo->value)
                foo = foo->left;
            else
                foo = foo->right;
        }
        elems++;

        Node* tmp = new Node();
        tmp->parent = parent;
        tmp->right = NULL;
        tmp->left = NULL;
        tmp->color = RED;
        tmp->value = data;

       if (root == NULL) {
           root = tmp;
       } else {
           if (data < parent->value)
               parent->left = tmp;
           else
               parent->right = tmp;
       }
       fixInsert(tmp);
    }

    iterator find(ValueType value) const {
        Node* foo = root;
        while (foo != NULL && (foo->value < value || value < foo->value)) {
            if (value < foo->value)
                foo = foo->left;
            else
                foo = foo->right;
        }
        iterator ans;
        ans.parent = this;
        ans.data = foo;
        return ans;
    }

    iterator lower_bound(ValueType value) const {
        Node* foo = root;
        Node* best = NULL;
        while (foo != NULL && (foo->value < value || value < foo->value)) {
            if (foo->value < value) {
                foo = foo->right;
            } else {
                if (best == NULL) {
                    best = foo;
                } else {
                    if (foo->value < best->value)
                        best = foo;
                }
                foo = foo->left;
            }
        }
        if (foo != NULL && !(foo->value < value || value < foo->value))
            best = foo;
        iterator ans;
        ans.parent = this;
        ans.data = best;
        return ans;
    }

    void clear(Node* no) {
        if (no == NULL)
            return;
        clear(no->right);
        clear(no->left);
        // std::cout << "deleting " << no->value << '\n';
        delete no;
    }

    ~Set() {
        clear(root);
        // std::cout << "deleted\n\n";
    }
};
