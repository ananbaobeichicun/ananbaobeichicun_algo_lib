#ifndef UNTITLED_VEC2D_H
#define UNTITLED_VEC2D_H
#include "../math.h"

struct vec2d;

constexpr vec2d operator*(double lambda, vec2d v);

struct vec2d {
    double x, y;

    [[nodiscard]] constexpr double abs() const {
        return std::hypot(x, y);
    }

    [[nodiscard]] constexpr vec2d unitize() const {
        return 1 / abs() * *this;
    }
};

constexpr vec2d operator+(const vec2d v) {
    return v;
}

constexpr vec2d operator-(const vec2d v) {
    return (-1.) * v;
}

constexpr vec2d operator+(const vec2d a, const vec2d b) {
    return {a.x + b.x, a.y + b.y};
}

constexpr vec2d operator-(const vec2d a, const vec2d b) {
    return {a.x - b.x, a.y - b.y};
}

constexpr vec2d operator*(const double lambda, const vec2d v) {
    return {lambda * v.x, lambda * v.y};
}

constexpr double cross_product(const vec2d a, const vec2d b) {
    return a.x * b.y - a.y * b.x;
}

constexpr double dot_product(const vec2d a, const vec2d b) {
    return a.x * b.x + a.y * b.y;
}

constexpr double cos_angle_between(const vec2d a, const vec2d b) {
    return dot_product(a, b) / (a.abs() * b.abs());
}

constexpr double angle_between(const vec2d a, const vec2d b) {
    return clamped_acos(cos_angle_between(a, b));
}

constexpr double project_length(const vec2d a, const vec2d b) {
    return dot_product(a, b) / dot_product(b, b);
}

#endif //UNTITLED_VEC2D_H