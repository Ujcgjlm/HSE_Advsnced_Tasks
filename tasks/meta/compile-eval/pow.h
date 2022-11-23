#pragma once

template<unsigned a, unsigned b>
struct Pow {
    static const unsigned value = a * Pow<a, b - 1>::value;
};

template<unsigned a>
struct Pow<a, 0> {
    static const unsigned value = 1;
};
