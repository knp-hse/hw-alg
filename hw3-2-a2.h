#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <utility>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    Hash hasher;
    std::pair<const KeyType, ValueType>** content;
    std::vector<bool> filled, erased;
    size_t elems;
    size_t max;
    constexpr static double multiplier = 2;
    constexpr static double maxLoad = 0.7;

    class iterator {
    public:
        HashMap* data;
        std::pair<const KeyType, ValueType>* value;
        size_t number;

        iterator() {
            value = nullptr;
            number = 0;
        }

        iterator(const iterator& other) {
            data = other.data;
            value = other.value;
            number = other.number;
        }

        friend bool operator==(const iterator& left, const iterator& right) {
            return left.value == right.value && left.number == right.number &&
                    right.data == left.data;
        }

        friend bool operator!=(const iterator& left, const iterator& right) {
            return left.value != right.value || left.number != right.number ||
                    right.data != left.data;
        }

        std::pair<const KeyType, ValueType>* operator->() {
            return value;
        }

        std::pair<const KeyType, ValueType>& operator*() {
            return *value;
        }

        iterator& operator++() {
            while (++number < data->max && !data->filled[number]) {}
            if (number != data->max)
                value = data->content[number];
            else
                value = nullptr;
            return *this;
        }
    };

    class const_iterator {
    public:
        const HashMap* data;
        const std::pair<const KeyType, ValueType>* value;
        size_t number;

        const_iterator() {
            value = nullptr;
            number = 0;
        }

        const_iterator(const const_iterator& other) {
            data = other.data;
            value = other.value;
            number = other.number;
        }

        friend bool operator==(const const_iterator& left, const const_iterator& right) {
            return left.value == right.value && left.number == right.number &&
                    right.data == left.data;
        }

        friend bool operator!=(const const_iterator& left, const const_iterator& right) {
            return left.value != right.value || left.number != right.number ||
                    right.data != left.data;
        }

        const std::pair<const KeyType, ValueType>& operator*() const {
            return *value;
        }

        const std::pair<const KeyType, ValueType>* operator->() const {
            return value;
        }

        const_iterator& operator++() {
            while (++number < data->max && !data->filled[number]) {}
            if (number != data->max)
                value = data->content[number];
            else
                value = nullptr;
            return *this;
        }
    };

    HashMap(const HashMap& other) :
    erased(other.erased), elems(other.elems), hasher(other.hasher),
    max(other.max), filled(other.filled) {
        content = new std::pair<const KeyType, ValueType>*[max];
        for (int i = 0; i < max; ++i) {
            if (filled[i])
                content[i] = new std::pair<const KeyType, ValueType>(*(other.content[i]));
        }
    }

    HashMap(const Hash& hasher = Hash()) :
    hasher(hasher) {
        elems = 0;
        max = 256;
        content = new std::pair<const KeyType, ValueType>*[256];
        filled.resize(256, 0);
        erased.resize(256, 0);
    }

    template<class Iterator>
    HashMap(Iterator begin, Iterator end, const Hash& hasher = Hash()) :
    hasher(hasher) {
        elems = 0;
        max = 256;
        content = new std::pair<const KeyType, ValueType>*[256];
        filled.resize(256, 0);
        erased.resize(256, 0);
        while (begin != end) {
            insert(*begin);
            begin++;
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> ls,
            const Hash& hasher = Hash()) :
    hasher(hasher) {
        elems = 0;
        max = 256;
        content = new std::pair<const KeyType, ValueType>*[256];
        filled.resize(256, 0);
        erased.resize(256, 0);
        auto begin = ls.begin();
        auto end = ls.end();
        while (begin != end) {
            insert(*begin);
            begin++;
        }
    }

    const size_t size() const {
        return elems;
    }

    const bool empty() const {
        return elems == 0;
    }

    const Hash hash_function() const {
        return hasher;
    }

    void rebuild() {
        std::pair<const KeyType, ValueType>** oldContent = content;
        std::vector<bool> oldFilled = filled;
        size_t oldmax = max;
        max *= multiplier;
        content = new std::pair<const KeyType, ValueType>*[max];
        std::fill(filled.begin(), filled.end(), 0);
        filled.resize(max, 0);
        std::fill(erased.begin(), erased.end(), 0);
        erased.resize(max, 0);
        elems = 0;
        for (size_t i = 0; i < oldmax; ++i) {{{
            if (oldFilled[i]) {
                insert(*oldContent[i]);
                delete oldContent[i];
            }
                }}}
        delete[] oldContent;
    }

    void insert(const std::pair<const KeyType, ValueType>& elem) {
        size_t num = hasher(elem.first) % max;
        while (num < 0)
            num += max;
        size_t ii;
        for (ii = 0; filled[(num + ii) % max] && ii != max ; ++ii) {
            if (filled[(num + ii) % max] &&
                    (content[(num + ii) % max]->first == elem.first))
                return;
        }
        if (ii == max) {
            rebuild();
            insert(elem);
            return;
        }
        filled[(num + ii) % max] = 1;
        content[(num + ii) % max] = new std::pair<const KeyType, ValueType>(elem);
        elems++;
        if ((static_cast<double>(elems) / max) > maxLoad)
            rebuild();
    }

    void erase(KeyType key) {
        size_t num = hasher(key) % max;
        while (num < 0)
            num += max;
        for (size_t i = 0; (filled[(num + i) % max] || erased[(num + i) % max])
             && i != max; ++i) {
            if (filled[(num + i) % max] &&
                    key == content[(num + i) % max]->first) {
                filled[(num + i) % max] = 0;
                erased[(num + i) % max] = 1;
                delete content[(num + i) % max];
                elems--;
                break;
            }
        }
    }

    iterator begin() {
        iterator iter;
        iter.data = this;
        iter.value = nullptr;
        iter.number = max;
        for (size_t i = 0; i < max; ++i) {
            if (filled[i]) {
                iter.number = i;
                iter.value = content[i];
                break;
            }
        }
        return iter;
    }

    const_iterator begin() const {
        const_iterator iter;
        iter.data = this;
        iter.value = nullptr;
        iter.number = max;
        for (size_t i = 0; i < max; ++i) {
            if (filled[i]) {
                iter.number = i;
                iter.value = content[i];
                break;
            }
        }
        return iter;
    }

    iterator end() {
        iterator iter;
        iter.data = this;
        iter.number = max;
        iter.value = nullptr;
        return iter;
    }

    const_iterator end() const {
        const_iterator iter;
        iter.data = this;
        iter.number = max;
        iter.value = nullptr;
        return iter;
    }

    iterator find(KeyType key) {
        size_t num = hasher(key) % max;
        while (num < 0)
            num += max;
        iterator ans;
        ans.data = this;
        bool found = 0;
        size_t ii;
        for (ii = 0; (filled[(num + ii) % max] || erased[(num + ii) % max])
             && ii != max; ++ii) {
            if (filled[(num + ii) % max] &&
                    key == content[(num + ii) % max]->first) {
                ans.number = (num + ii) % max;
                ans.value = content[(num + ii) % max];
                found = 1;
                break;
            }
        }
        if (!found) {
            ans.number = max;
            ans.value = nullptr;
        }
        return ans;
    }

    const_iterator find(KeyType key) const {
        size_t num = hasher(key) % max;
        while (num < 0)
            num += max;
        const_iterator ans;
        ans.data = this;
        bool found = 0;
        size_t ii;
        for (ii = 0; (filled[(num + ii) % max] || erased[(num + ii) % max])
             && ii != max; ++ii) {
            if (filled[(num + ii) % max] &&
                    key == content[(num + ii) % max]->first) {
                ans.number = (num + ii) % max;
                ans.value = content[(num + ii) % max];
                found = 1;
                break;
            }
        }
        if (!found) {
            ans.number = max;
            ans.value = nullptr;
        }
        return ans;
    }

    ValueType& operator[](KeyType key) {
        iterator xd = find(key);
        if (xd == end()) {
            insert(std::make_pair(key, ValueType()));
            xd = find(key);
            return xd->second;
        } else {
            return xd->second;
        }
    }

    const ValueType& at(KeyType key) const {
        const_iterator xd = find(key);
        if (xd == end()) {
            throw std::out_of_range("(-_-)");
        } else {
            return xd->second;
        }
    }

    void clear() {
        for (size_t i = 0; i < max; ++i) {
            if (filled[i])
                    delete content[i];
        }
        delete[] content;
        elems = 0;
        max = 256;
        content = new std::pair<const KeyType, ValueType>*[max];
        filled.resize(max);
        std::fill(filled.begin(), filled.end(), 0);
        erased.resize(max);
        std::fill(erased.begin(), erased.end(), 0);
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other)
            return *this;
        for (int i = 0; i < max; ++i) {{{
            if (filled[i])
                delete content[i];
                }}}
        delete[] content;
        max = other.max;
        filled = other.filled;
        elems = other.elems;
        erased = other.erased;
        content = new std::pair<const KeyType, ValueType>*[max];

        // no assignment operator for lambda functions
        hasher.~Hash();
        new (&hasher) Hash(hasher);

        for (int i = 0; i < max; ++i) {
            if (filled[i])
                content[i] = new std::pair<const KeyType, ValueType>(*(other.content[i]));
        }
        return *this;
    }

    ~HashMap() {
        for (size_t i = 0; i < max; ++i) {{{
            if (filled[i])
                delete content[i];
                }}}
        delete[] content;
    }
};
