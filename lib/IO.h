// ReSharper disable CppNonInlineVariableDefinitionInHeaderFile
// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
#ifndef IO_H
#define IO_H
#include <iomanip>
#include <utility>

namespace IO {
    constexpr size_t SIZE = 1ull << 24;
    static char read_buf[SIZE], write_buf[SIZE];
    char *rp = read_buf, *rq = read_buf, *wp = write_buf;

    char read_char() {
        [[unlikely]]
        if (rp == rq) {
            rp = read_buf;
            rq = rp + fread(read_buf, 1, SIZE, stdin);
            if (rp == rq)
                return EOF;
        }
        return *rp++;
    }

    template<std::integral T>
    T read_integral() = delete;

    template<std::unsigned_integral T>
    T read_integral() {
    retry:
        char c;
        T ans = 0;
        bool success = false;
        while ((c = read_char()) != '\n' && c != ' ' && c != EOF) {
            success = true;
            ans = ans * 10 + (c ^ 48);
        }
        if (!success)
            goto retry;
        return ans;
    }

    template<std::signed_integral T>
    T read_integral() {
    retry:
        char c;
        T ans = 0, flag = 1;
        bool success = false;
        while ((c = read_char()) != '\n' && c != ' ' && c != EOF) {
            if (c == '-')
                flag = -1;
            else
                ans = ans * 10 + (c ^ 48);
        }
        if (!success)
            goto retry;
        return ans * flag;
    }

    void write_char(const char c) {
        if (wp == write_buf + SIZE) {
            fwrite(write_buf, 1, SIZE, stdout);
            wp = write_buf;
        }
        *wp++ = c;
    }

    size_t flush() {
        fwrite(write_buf, 1, wp - write_buf, stdout);
        return std::exchange(wp, write_buf) - write_buf;
    }

    template<std::integral T>
    void write_integral(T x) = delete;

    template<std::unsigned_integral T>
    void write_integral(T x) {
        T l = 1;
        while (l * 10 <= x)
            l *= 10;
        for (; l; x %= l, l /= 10)
            write_char((x / l) ^ 48);
    }

    template<std::signed_integral T>
    void write_integral(T x) {
        if (x < 0) {
            write_char('-');
            x = -x;
        }
        write_integral<std::make_unsigned_t<T> >(x);
    }
}

#endif // IO_H
