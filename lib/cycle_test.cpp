#include <iostream>
#include <cstdint>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    uint64_t x = 1;
    constexpr size_t BATCH = 1e9;
    for (size_t i = 0; i < BATCH; ++i) {
        x ^= x << 7;
        x ^= x >> 9;
        x *= 0x9e3779b97f4a7c15ULL;
    }

    std::cout << x;
}
