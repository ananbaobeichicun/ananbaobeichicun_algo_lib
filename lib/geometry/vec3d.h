#ifndef VEC3D_H
#define VEC3D_H
#include "../math.h"

struct vec3d;

constexpr vec3d operator*(double lambda, vec3d v);

struct vec3d {
    double x, y, z;

    [[nodiscard]] constexpr double abs() const {
        return std::hypot(x, y, z);
    }

    [[nodiscard]] constexpr vec3d unitize() const {
        return 1 / abs() * *this;
    }
};

constexpr vec3d operator+(const vec3d v) {
    return v;
}

constexpr vec3d operator-(const vec3d v) {
    return (-1.) * v;
}

constexpr vec3d operator+(const vec3d a, const vec3d b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

constexpr vec3d operator-(const vec3d a, const vec3d b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

constexpr vec3d operator*(const double lambda, const vec3d v) {
    return {lambda * v.x, lambda * v.y, lambda * v.z};
}

constexpr vec3d cross_product(const vec3d a, const vec3d b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

constexpr double dot_product(const vec3d a, const vec3d b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr double cos_angle_between(const vec3d a, const vec3d b) {
    return dot_product(a, b) / (a.abs() * b.abs());
}

constexpr double angle_between(const vec3d a, const vec3d b) {
    return clamped_acos(cos_angle_between(a, b));
}

constexpr double project_length(const vec3d a, const vec3d b) {
    return dot_product(a, b) / dot_product(b, b);
}

#endif //VEC3D_H
