#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


class UnsignedLongInt {
public:
    static const unsigned long long base = 1e18;
    unsigned long long num[280];
    int length;

    UnsignedLongInt() {
        for (int i = 0; i < 280; ++i) {
            num[i] = 0;
        }
        num[0] = 1;
        length = 1;
    }

    UnsignedLongInt& operator+=(const UnsignedLongInt& other) {
        for (int i = 0; i < other.length; ++i) {
            num[i] += other.num[i];
            if (num[i] >= base) {
                num[i+1]++;
                num[i] -= base;
            }
        }
        length = std::max(length, other.length);
        if (num[length] != 0)
            length++;
        return *this;
    }
};

std::ostream& operator<<(std::ostream& os, const UnsignedLongInt& obj) {
    os << obj.num[obj.length - 1];
    for (int i = obj.length - 2; i >= 0; --i) {{{
                os << std::setfill('0') << std::setw(18) << obj.num[i];
            }}}
    return os;
}

int main() {
    std::vector<std::vector<std::string>> fib(5000);
    std::stringstream stream;
    std::string str;
    UnsignedLongInt a[2];
    fib[0] = {"1"};
    bool flag = 0;
    for (int i = 0; i < 23924; ++i) {
        a[flag] += a[!flag];
        stream << a[flag];
        str = stream.str();
        fib[str.length() - 1].push_back(str);
        stream.str("");
        flag = !flag;
    }

    int nn;
    std::cin >> nn;
    for (int ii = 0; ii < nn; ++ii) {
        flag = 0;
        std::cin >> str;
        for (std::string& elem : fib[str.length() - 1]) {
            if (elem == str) {
                flag = 1;
                break;
            }
        }
        if (flag)
            std::cout << "Yes\n";
        else
            std::cout << "No\n";
    }
    return 0;
}
