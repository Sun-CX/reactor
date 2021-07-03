//
// Created by 孙诚雄 on 2021/7/1.
//

#ifndef REACTOR_URLENCODER_H
#define REACTOR_URLENCODER_H

#include "Buffer.h"
#include <vector>

namespace reactor::net {
    using std::vector;

    class URLEncoder final {
    private:
        static void hex_char(byte c, byte &hex1, byte &hex2) {
            hex1 = c / 16;
            hex2 = c % 16;
            hex1 += hex1 <= 9 ? '0' : 'a' - 10;
            hex2 += hex2 <= 9 ? '0' : 'a' - 10;
        }

    public:
        static string encode(const string &s) {
            const char *str = s.c_str();
            vector<char> v(s.size());
            v.clear();
            for (size_t i = 0, l = s.size(); i < l; ++i) {
                char c = str[i];
                if ((c >= '0' && c <= '9') ||
                    (c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') ||
                    c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
                    c == '*' || c == '\'' || c == '(' || c == ')') {
                    v.push_back(c);
                } else if (c == ' ') {
                    v.push_back('+');
                } else {
                    v.push_back('%');
                    byte d1, d2;
                    hex_char(c, d1, d2);
                    v.push_back(d1);
                    v.push_back(d2);
                }
            }
            return string(v.cbegin(), v.cend());
        }
    };
}

#endif //REACTOR_URLENCODER_H
