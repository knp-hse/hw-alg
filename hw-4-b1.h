#include <algorithm>
#include <iostream>
#include <vector>

class Dsu {
private:
    std::vector<size_t> ranks;
    mutable std::vector<size_t> parents;
    size_t unities;

    size_t find(size_t elem) const {
        if (parents[elem] == elem)
            return elem;
        return elem = find(parents[elem]);
    }

public:
    Dsu(size_t n):
    parents(n), ranks(n) {
        for (size_t i = 0; i < n; ++i) {
            parents[i] = i;
            ranks[i] = 0;
        }
        unities = n;
    }

    bool are_joined(size_t first, size_t second) const {
        return find(first) == find(second);
    }

    bool join(size_t first, size_t second) {
        first = find(first);
        second = find(second);
        if (first == second)
            return false;
        if (ranks[first] == ranks[second])
            ranks[second]++;
        if (ranks[first] > ranks[second])
            parents[second] = first;
        else
            parents[first] = second;
        unities--;
        return true;
    }

    size_t size() const {
        return parents.size();
    }

    size_t disjoint_sets_count() const {
        return unities;
    }
};

struct Edge {
    int from, to, time;
};

bool cmp_edge (const Edge& first, const Edge& second) {
    return first.time < second.time;
}

size_t calculate_answer() {
    size_t ans = 0;
    int nn, mm, foo;
    bool bar;
    std::cin >> nn >> mm;
    std::vector<Edge> edges;
    edges.reserve(nn*mm);
    Dsu uni(nn*mm);

    for (int i = 0; i < nn; ++i)
        for (int j = 0; j < mm; ++j) {
            if (j != mm - 1) {
                std::cin >> bar >> foo;
                if (bar) {
                    uni.join(i * mm + j, i * mm + j + 1);
                    ans += foo;
                } else {
                    edges.push_back({i * mm + j, i * mm + j + 1, -1 * foo});
                }
            }
            if (i != nn - 1) {
                std::cin >> bar >> foo;
                if (bar) {
                    uni.join(i * mm + j, (i + 1) * mm + j);
                    ans += foo;
                } else {
                    edges.push_back({i * mm + j, (i + 1) * mm + j, -1 * foo});
                }
            }
        }
    std::sort(edges.begin(), edges.end(), cmp_edge);
    auto iter = edges.begin();

    while (uni.disjoint_sets_count() != 1) {
        if (!uni.are_joined(iter->from, iter->to)) {
            uni.join(iter->from, iter->to);
            ans -= iter->time;
        }
        iter++;
    }

    return ans;
}
