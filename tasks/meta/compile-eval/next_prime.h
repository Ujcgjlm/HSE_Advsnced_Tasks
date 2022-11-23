#pragma once

constexpr bool IsPrime(int x) {
    if (x == 1) {
        return 0;
    }

    for (int i = 2; i*i <= x; ++i) {
        if (!(x % i)) {
            return 0;
        }
    }

    return 1;
}

constexpr int next_prime(int x) {
    while (!IsPrime(x)) {
        ++x;
    }
    return x;
}
