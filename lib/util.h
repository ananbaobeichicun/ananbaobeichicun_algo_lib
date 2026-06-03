#ifndef UTIL_H
#define UTIL_H

template<typename T>
bool cmin(T &x, const T &y) {
    if (x > y) {
        x = y;
        return true;
    }
    return false;
}

template<typename T>
bool cmax(T &x, const T &y) {
    if (x < y) {
        x = y;
        return true;
    }
    return false;
}

#endif //UTIL_H
