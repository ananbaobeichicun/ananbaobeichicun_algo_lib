#ifndef FAST_IO_H
#define FAST_IO_H
#include <iomanip>
#include <utility>

#ifdef LOCAL_DBG
#define fread _fread_nolock
#define fwrite _fwrite_nolock
#else
#define fread fread_unlocked
#define fwrite fwrite_unlocked
#endif

// 读入/输出INT_MIN（对 int 变量）或LONG_LONG_MIN（对 long long 变量）是ub

// ReSharper disable CppNonInlineVariableDefinitionInHeaderFile
// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
namespace fast_IO {
    constexpr size_t size = 1ull << 24;
    static char read_buf[size], write_buf[size];
    static char *rp = read_buf, *rq = read_buf, *wp = write_buf;

    static char peek_char() {
        if (rp == rq) [[unlikely]] {
            rp = read_buf;
            rq = rp + fread(read_buf, 1, size, stdin);
            if (rp == rq)
                return EOF;
        }
        return *rp;
    }

    static char read_char() {
        const char res = peek_char();
        ++rp;
        return res;
    }

    template<std::unsigned_integral T>
    static T read_integral() {
        char c;
        T ans = 0;
        while (std::isdigit(c = read_char())) {
            [[assume('0' <= c && c <= '9')]];
            ans = ans * 10 + (c ^ 48);
        }
        return ans;
    }

    template<std::signed_integral T>
    static T read_integral() {
        char c = read_char();
        T ans = 0, flag = -1;
        if (c != '-') {
            flag = 1;
            ans = ans * 10 + (c ^ 48);
        }

        while (std::isdigit(c = read_char())) {
            [[assume('0' <= c && c <= '9')]];
            ans = ans * 10 + (c ^ 48);
        }
        return ans * flag;
    }

    static void write_char(const char c) {
        if (wp == write_buf + size) [[unlikely]] {
            fwrite(write_buf, 1, size, stdout);
            wp = write_buf;
        }
        *wp++ = c;
    }

    static size_t flush() {
        fwrite(write_buf, 1, size_t(wp - write_buf), stdout);
        return size_t(std::exchange(wp, write_buf) - write_buf);
    }

    template<std::unsigned_integral T>
    static void write_integral(T x) {
        T l = 1;
        while (l <= x / 10)
            l *= 10;
        for (; l; x %= l, l /= 10)
            write_char((x / l) ^ 48);
    }

    template<std::signed_integral T>
    static void write_integral(T x) {
        if (x < 0) {
            write_char('-');
            x = -x;
        }
        write_integral(std::make_unsigned_t<T>(x));
    }
}

// ReSharper restore CppNonInlineVariableDefinitionInHeaderFile
// ReSharper restore CppNonInlineFunctionDefinitionInHeaderFile
#endif // FAST_IO_H
