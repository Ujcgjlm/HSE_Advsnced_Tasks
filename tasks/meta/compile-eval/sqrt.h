template<bool B, class T, class F>
struct Conditional {
    typedef T Type;
};

template<class T, class F>
struct Conditional<false, T, F> {
    typedef F Type;
};

template<bool B, int T, int F>
struct IntConditional {
    static const unsigned value = T;
};

template<int T, int F>
struct IntConditional<false, T, F> {
    static const unsigned value = F;
};


template<int N, unsigned MaxValue = 32768, unsigned Step = MaxValue / 2>
struct Sqrt {
    static const unsigned value = Conditional<MaxValue * MaxValue >= N, Sqrt<N, MaxValue - Step, Step / 2>, Sqrt<N, MaxValue + Step, Step / 2> >::Type::value;
};

template<int N, unsigned MaxValue>
struct Sqrt<N, MaxValue, 0> {
    static const unsigned value = IntConditional<MaxValue * MaxValue >= N, MaxValue, MaxValue + 1>::value;
};
