#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

template <size_t N>
constexpr int& GetElement(std::array<std::array<int, N>, N>& a, size_t i, size_t j) {
    return *(&std::get<0>(*(&std::get<0>(a) + i)) + j);
}

template <size_t N>
constexpr int determinant(const std::array<std::array<int, N>, N>& a) {
    int det = 0;
    for (size_t i = 0; i < N; ++i) {
        std::array<std::array<int, N-1>, N-1> matrix{};
        for (size_t j = 0; j < N; ++j) {
            if (i != j) {
                for (size_t g = 1; g < N; ++g) {
                    ((j < i) ? GetElement<N-1>(matrix, j, g-1) : GetElement<N-1>(matrix, j-1, g-1)) = a[j][g];
                }
            }
        }
        det += ((i&1) ? -1 : 1) * a[i][0] * determinant<N - 1>(matrix);
    }
    return det;
}

template <>
constexpr int determinant<2>(const std::array<std::array<int, 2>, 2>& a) {
    return a[0][0] * a[1][1] - a[1][0] * a[0][1];
}

template <>
constexpr int determinant<1>(const std::array<std::array<int, 1>, 1>& a) {
    return a[0][0];
}
