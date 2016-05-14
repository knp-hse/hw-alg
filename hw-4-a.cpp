#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

int main() {
    std::vector<std::pair<double, size_t>> merc;
    size_t nn, ww, foo, bar;
    double ans = 0;
    std::cin >> nn >> ww;
    for (size_t i = 0; i < nn; ++i) {
        std::cin >> foo >> bar;
        if (bar != 0) {
            if (foo != 0) {
                merc.push_back(std::make_pair(static_cast<double>(foo)/bar, bar));
            }
        } else {
            ans += foo;
        }
    }
    std::sort(merc.begin(), merc.end());
    std::reverse(merc.begin(), merc.end());
    foo = 0;
    auto it = merc.begin();
    while (foo < ww && it != merc.end()) {
        foo += it->second;
        ans += it->first * it-> second;
        it++;
    }
    if (foo > ww) {
        it--;
        ans -= it->first * it->second;
        foo -= it->second;
        ans += it->first * (ww - foo);
    }
    std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(6) << ans;
    return 0;
}
