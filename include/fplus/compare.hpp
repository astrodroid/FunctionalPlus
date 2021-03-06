// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/function_traits.hpp>

namespace fplus
{

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

namespace internal
{
    // Checks if a type has a non-template call operator.
    // source: http://stackoverflow.com/a/8907461/1866775
    template <typename F, typename... Args>
    struct check_callable{
        static int tester[1];
        typedef char yes;
        typedef yes (&no)[2];

        template <typename G, typename... Brgs, typename C>
        static typename std::enable_if<!std::is_same<G,C>::value, char>::type
            sfinae(decltype(std::declval<G>()(std::declval<Brgs>()...)) (C::*pfn)(Brgs...));

        template <typename G, typename... Brgs, typename C>
        static typename std::enable_if<!std::is_same<G,C>::value, char>::type
            sfinae(decltype(std::declval<G>()(std::declval<Brgs>()...)) (C::*pfn)(Brgs...) const);

        template <typename G, typename... Brgs>
        static char sfinae(decltype(std::declval<G>()(std::declval<Brgs>()...)) (G::*pfn)(Brgs...));

        template <typename G, typename... Brgs>
        static char sfinae(decltype(std::declval<G>()(std::declval<Brgs>()...)) (G::*pfn)(Brgs...) const);

        template <typename G, typename... Brgs>
        static yes test(int (&a)[sizeof(sfinae<G,Brgs...>(&G::operator()))]);

        template <typename G, typename... Brgs>
        static no test(...);
    public:
        static bool const value = sizeof(test<F, Args...>(tester)) == sizeof(yes);
    };

    template<class R, class... Args>
    struct check_callable_helper{ R operator()(Args...); };

    template<typename R, typename... FArgs, typename... Args>
    struct check_callable<R(*)(FArgs...), Args...>
      : public check_callable<check_callable_helper<R, FArgs...>, Args...>
    {};

    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif

    template<int TargetArity, typename F>
    void check_arity_helper(std::true_type)
    {
        static_assert(utils::function_traits<F>::arity == TargetArity,
            "Wrong arity.");
    }

    template<int TargetArity, typename F>
    void check_arity_helper(std::false_type)
    {
    }

    template<int TargetArity, typename F>
    void check_arity()
    {
        internal::check_arity_helper<TargetArity, F>(
            std::integral_constant<bool, check_callable<F>::value>());
    }

    template <typename UnaryPredicate, typename T>
    void check_unary_predicate_for_type_helper(std::true_type)
    {
        static_assert(check_callable<UnaryPredicate>::value && utils::function_traits<UnaryPredicate>::arity == 1,
            "Wrong arity.");
        static_assert(check_callable<UnaryPredicate>::value && std::is_convertible<T,
            typename utils::function_traits<UnaryPredicate>::template arg<0>::type>::value,
            "Unary predicate can not take these values.");
        static_assert(check_callable<UnaryPredicate>::value && std::is_convertible<
            typename std::result_of<UnaryPredicate(T)>::type, bool>::value,
            "Predicate must return bool.");
    }

    template <typename UnaryPredicate, typename T>
    void check_unary_predicate_for_type_helper(std::false_type)
    {
    }

    template <typename UnaryPredicate, typename T>
    void check_unary_predicate_for_type()
    {
        internal::check_unary_predicate_for_type_helper<UnaryPredicate, T>(
            std::integral_constant<bool, check_callable<UnaryPredicate>::value>());
    }

    template <typename F, typename T>
    void check_index_with_type_predicate_for_type_helper(std::true_type)
    {
        static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
        typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
        typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
        static_assert(std::is_same<FIn0, std::size_t>::value,
            "First parameter of function must be std::size_t.");
        static_assert(std::is_convertible<
            typename std::result_of<F(std::size_t, T)>::type, bool>::value,
            "Function must return bool.");
        static_assert(std::is_convertible<T, FIn1>::value,
            "Function does not work with elements of Container.");
    }

    template <typename F, typename T>
    void check_index_with_type_predicate_for_type_helper(std::false_type)
    {
    }

    template <typename F, typename T>
    void check_index_with_type_predicate_for_type()
    {
        internal::check_index_with_type_predicate_for_type_helper<F, T>(
            std::integral_constant<bool, check_callable<F>::value>());
    }

    template <typename BinaryPredicate, typename T>
    void check_binary_predicate_for_type_helper(std::true_type)
    {
        static_assert(utils::function_traits<BinaryPredicate>::arity == 2,
            "Wrong arity.");
        typedef typename utils::function_traits<BinaryPredicate>::template arg<0>::type FIn;
        typedef typename utils::function_traits<BinaryPredicate>::template arg<1>::type FIn1;
        static_assert(std::is_same<FIn, FIn1>::value,
            "BinaryPredicate must take two similar types");
        static_assert(std::is_convertible<
            typename std::result_of<BinaryPredicate(T, T)>::type, bool>::value,
            "BinaryPredicate must return bool.");
        static_assert(std::is_convertible<T, FIn>::value,
            "BinaryPredicate does not work with elements of Container.");
    }

    template <typename BinaryPredicate, typename T>
    void check_binary_predicate_for_type_helper(std::false_type)
    {
    }

    template <typename BinaryPredicate, typename T>
    void check_binary_predicate_for_type()
    {
        internal::check_binary_predicate_for_type_helper<BinaryPredicate, T>(
            std::integral_constant<bool, check_callable<BinaryPredicate>::value>());
    }

    template <typename Compare, typename T>
    void check_compare_for_type_helper(std::true_type)
    {
        static_assert(utils::function_traits<Compare>::arity == 2, "Wrong arity.");
        typedef typename utils::function_traits<Compare>::template arg<0>::type FIn;
        typedef typename utils::function_traits<Compare>::template arg<1>::type FIn1;
        static_assert(std::is_same<FIn, FIn1>::value,
            "Compare must take two similar types");
        static_assert(std::is_convertible<
            typename std::result_of<Compare(T, T)>::type, bool>::value,
            "Compare must return bool.");
        static_assert(std::is_convertible<T, FIn>::value,
            "Compare does not work with elements of Container.");
    }

    template <typename Compare, typename T>
    void check_compare_for_type_helper(std::false_type)
    {
    }

    template <typename Compare, typename T>
    void check_compare_for_type()
    {
        internal::check_compare_for_type_helper<Compare, T>(
            std::integral_constant<bool, check_callable<Compare>::value>());
    }

    template <typename F, typename G, typename X, typename Y>
    void check_compare_preprocessors_for_types_helper(
        std::true_type, std::true_type)
    {
        static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
        static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
        static_assert(std::is_convertible<X,
            typename utils::function_traits<F>::template arg<0>::type>::value,
            "Function can note take elements of this type.");
        static_assert(std::is_convertible<Y,
            typename utils::function_traits<G>::template arg<0>::type>::value,
            "Function can note take elements of this type.");
        static_assert(std::is_same<typename std::result_of<F(X)>::type,
            typename std::result_of<G(Y)>::type>::value,
            "Both functions must return same type.");
    }

    template <typename F, typename G, typename X, typename Y,
        typename FC, typename GC>
    void check_compare_preprocessors_for_types_helper(FC, GC)
    {
    }

    template <typename F, typename G, typename X, typename Y>
    void check_compare_preprocessors_for_types()
    {
        internal::check_compare_preprocessors_for_types_helper<F, G, X, Y>(
            std::integral_constant<bool, check_callable<F>::value>(),
            std::integral_constant<bool, check_callable<G>::value>());
    }
} // namespace internal

// API search type: identity : a -> a
// fwd bind count: 0
// identity(x) == x
template <typename T>
T identity(const T& x)
{
    return x;
}

// API search type: is_equal : (a, a) -> Bool
// fwd bind count: 1
// x == y
template <typename T>
bool is_equal(const T& x, const T& y)
{
    return x == y;
}

// API search type: always : a -> (b -> a)
// always(x)(y) == x
template <typename Y, typename X>
std::function<X(const Y&)> always(const X& x)
{
    return [x](const Y&) { return x; };
}

// API search type: is_equal_by_and_by : ((a -> b), (c -> b)) -> ((a, c) -> Bool)
// f(x) == g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GOut = typename std::result_of<G(GIn)>::type>
std::function<bool(const FIn& x, const GIn& y)>
        is_equal_by_and_by(F f, G g)
{
    internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_equal(f(x), g(y));
    };
}

// API search type: is_equal_by : (a -> b) -> (a -> Bool)
// f(x) == f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type>
std::function<bool(const FIn& x, const FIn& y)>
        is_equal_by(F f)
{
    return is_equal_by_and_by(f, f);
}

// API search type: is_equal_by_to : ((b -> a), a) -> (b -> Bool)
// f(y) == x
template <typename F, typename X,
    typename Y = typename utils::function_traits<F>::template arg<0>::type>
std::function<bool(const Y&)> is_equal_by_to(F f, const X& x)
{
    return [f, x](const Y& y)
    {
        return is_equal(f(y), x);
    };
}

// API search type: is_equal_to : a -> (a -> Bool)
// x == y
// curried version of is_equal
template <typename X>
std::function<bool(const X&)> is_equal_to(const X& x)
{
    return is_equal_by_to(identity<X>, x);
}

// API search type: is_not_equal : (a, a) -> Bool
// fwd bind count: 1
// x != y
template <typename T>
bool is_not_equal(const T& x, const T& y)
{
    return x != y;
}

// API search type: is_not_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) != g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GOut = typename std::result_of<G(GIn)>::type>
std::function<bool(const FIn& x, const GIn& y)>
        is_not_equal_by_and_by(F f, G g)
{
    internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_not_equal(f(x), g(y));
    };
}

// API search type: is_not_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) != f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type>
std::function<bool(const FIn& x, const FIn& y)>
        is_not_equal_by(F f)
{
    return is_not_equal_by_and_by(f, f);
}

// API search type: is_not_equal_by_to : ((a -> b), b) -> (a -> Bool)
// f(y) != x
template <typename F, typename X,
    typename Y = typename utils::function_traits<F>::template arg<0>::type>
std::function<bool(const Y&)> is_not_equal_by_to(F f, const X& x)
{
    return [f, x](const Y& y)
    {
        return is_not_equal(f(y), x);
    };
}

// API search type: is_not_equal_to : a -> (a -> Bool)
// y != x
template <typename X>
std::function<bool(const X&)> is_not_equal_to(const X& x)
{
    return is_not_equal_by_to(identity<X>, x);
}

// API search type: is_less : (a, a) -> Bool
// fwd bind count: 1
// x < y
template <typename T>
bool is_less(const T& x, const T& y)
{
    return x < y;
}

// API search type: is_less_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) < g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GOut = typename std::result_of<G(GIn)>::type>
std::function<bool(const FIn& x, const GIn& y)>
        is_less_by_and_by(F f, G g)
{
    internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_less(f(x), g(y));
    };
}

// API search type: is_less_by : (a -> b) -> ((a, a) -> Bool)
// f(x) < f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type>
std::function<bool(const FIn& x, const FIn& y)>
        is_less_by(F f)
{
    return is_less_by_and_by(f, f);
}

// API search type: is_less_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) < x
template <typename F, typename X,
    typename Y = typename utils::function_traits<F>::template arg<0>::type>
std::function<bool(const Y&)> is_less_by_than(F f, const X& x)
{
    return [f, x](const Y& y)
    {
        return is_less(f(y), x);
    };
}

// API search type: is_less_than : a -> (a -> Bool)
// y < x
template <typename X>
std::function<bool(const X&)> is_less_than(const X& x)
{
    return is_less_by_than(identity<X>, x);
}

// API search type: is_less_or_equal : (a, a) -> Bool
// fwd bind count: 1
// x <= y
template <typename T>
bool is_less_or_equal(const T& x, const T& y)
{
    return x <= y;
}

// API search type: is_less_or_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) <= g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GOut = typename std::result_of<G(GIn)>::type>
std::function<bool(const FIn& x, const GIn& y)>
        is_less_or_equal_by_and_by(F f, G g)
{
    internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_less_or_equal(f(x), g(y));
    };
}

// API search type: is_less_or_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) <= f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type>
std::function<bool(const FIn& x, const FIn& y)>
        is_less_or_equal_by(F f)
{
    return is_less_or_equal_by_and_by(f, f);
}

// API search type: is_less_or_equal_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) <= x
template <typename F, typename X,
    typename Y = typename utils::function_traits<F>::template arg<0>::type>
std::function<bool(const Y&)> is_less_or_equal_by_than(F f, const X& x)
{
    return [f, x](const Y& y)
    {
        return is_less_or_equal(f(y), x);
    };
}

// API search type: is_less_or_equal_than : a -> (a -> Bool)
// y <= x
template <typename X>
std::function<bool(const X&)> is_less_or_equal_than(const X& x)
{
    return is_less_or_equal_by_than(identity<X>, x);
}

// API search type: is_greater : a -> a -> Bool
// fwd bind count: 1
// x > y
template <typename T>
bool is_greater(const T& x, const T& y)
{
    return x > y;
}

// API search type: is_greater_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) > g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GOut = typename std::result_of<G(GIn)>::type>
std::function<bool(const FIn& x, const GIn& y)>
        is_greater_by_and_by(F f, G g)
{
    internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_greater(f(x), g(y));
    };
}

// API search type: is_greater_by : (a -> b) -> ((a, a) -> Bool)
// f(x) > f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type>
std::function<bool(const FIn& x, const FIn& y)>
        is_greater_by(F f)
{
    return is_greater_by_and_by(f, f);
}

// API search type: is_greater_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) > x
template <typename F, typename X,
    typename Y = typename utils::function_traits<F>::template arg<0>::type>
std::function<bool(const Y&)> is_greater_by_than(F f, const X& x)
{
    return [f, x](const Y& y)
    {
        return is_greater(f(y), x);
    };
}

// API search type: is_greater_than : a -> (a -> Bool)
// y > x
template <typename X>
std::function<bool(const X&)> is_greater_than(const X& x)
{
    return is_greater_by_than(identity<X>, x);
}

// API search type: is_greater_or_equal : (a, a) -> Bool
// fwd bind count: 1
// x >= y
template <typename T>
bool is_greater_or_equal(const T& x, const T& y)
{
    return x >= y;
}

// API search type: is_greater_or_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) >= g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GOut = typename std::result_of<G(GIn)>::type>
std::function<bool(const FIn& x, const GIn& y)>
        is_greater_or_equal_by_and_by(F f, G g)
{
    internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_greater_or_equal(f(x), g(y));
    };
}

// API search type: is_greater_or_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) >= f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type>
std::function<bool(const FIn& x, const FIn& y)>
        is_greater_or_equal_by(F f)
{
    return is_greater_or_equal_by_and_by(f, f);
}

// API search type: is_greater_or_equal_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) >= x
template <typename F, typename X,
    typename Y = typename utils::function_traits<F>::template arg<0>::type>
std::function<bool(const Y&)> is_greater_or_equal_by_than(F f, const X& x)
{
    return [f, x](const Y& y)
    {
        return is_greater_or_equal(f(y), x);
    };
}

// API search type: is_greater_or_equal_than : a -> (a -> Bool)
// y >= x
template <typename X>
std::function<bool(const X&)> is_greater_or_equal_than(const X& x)
{
    return is_greater_or_equal_by_than(identity<X>, x);
}

// API search type: xor_bools : (Bool, Bool) -> Bool
// fwd bind count: 1
// Exclusive or.
template <typename T>
bool xor_bools(const T& x, const T& y)
{
    static_assert(std::is_convertible<T, bool>::value,
        "Type must be convertible to bool.");
    return (x && !y) || (!x && y);
}

// API search type: ord_to_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_to_eq((<)) == (==)
template <typename Compare,
    typename FIn0 = typename utils::function_traits<Compare>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<Compare>::template arg<1>::type,
    typename FOut = typename std::result_of<Compare(FIn0, FIn1)>::type>
std::function<FOut(FIn0, FIn1)> ord_to_eq(Compare comp)
{
    internal::check_arity<2, Compare>();
    static_assert(std::is_same<FOut, bool>::value, "Function must return bool.");
    static_assert(std::is_same<FIn0, FIn1>::value,
        "Function must take two equal types.");
    return [comp]
           (FIn0 x, FIn1 y)
           { return !comp(x, y) && !comp(y, x); };
}

// API search type: ord_to_not_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_to_not_eq((<)) == (!=)
template <typename Compare,
    typename FIn0 = typename utils::function_traits<Compare>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<Compare>::template arg<1>::type,
    typename FOut = typename std::result_of<Compare(FIn0, FIn1)>::type>
std::function<FOut(FIn0, FIn1)> ord_to_not_eq(Compare comp)
{
    internal::check_arity<2, Compare>();
    static_assert(std::is_same<FOut, bool>::value, "Function must return bool.");
    static_assert(std::is_same<FIn0, FIn1>::value,
        "Function must take two equal types.");
    return [comp]
           (FIn0 x, FIn1 y)
           { return comp(x, y) || comp(y, x); };
}

// API search type: ord_eq_to_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_eq_to_eq((<=)) == (==)
template <typename Compare,
    typename FIn0 = typename utils::function_traits<Compare>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<Compare>::template arg<1>::type,
    typename FOut = typename std::result_of<Compare(FIn0, FIn1)>::type>
std::function<FOut(FIn0, FIn1)> ord_eq_to_eq(Compare comp)
{
    internal::check_arity<2, Compare>();
    static_assert(std::is_same<FOut, bool>::value, "Function must return bool.");
    static_assert(std::is_same<FIn0, FIn1>::value,
        "Function must take two equal types.");
    return [comp]
           (FIn0 x, FIn1 y)
           { return comp(x, y) && comp(y, x); };
}

// API search type: ord_eq_to_not_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_eq_to_not_eq((<=)) == (!=)
template <typename Compare,
    typename FIn0 = typename utils::function_traits<Compare>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<Compare>::template arg<1>::type,
    typename FOut = typename std::result_of<Compare(FIn0, FIn1)>::type>
std::function<FOut(FIn0, FIn1)> ord_eq_to_not_eq(Compare comp)
{
    internal::check_arity<2, Compare>();
    static_assert(std::is_same<FOut, bool>::value, "Function must return bool.");
    static_assert(std::is_same<FIn0, FIn1>::value,
        "Function must take two equal types.");
    return [comp]
           (FIn0 x, FIn1 y)
           {
                bool a = comp(x, y);
                bool b = comp(y, x);
                return (a && !b) || (!a && b);
           };
}

} // namespace fplus
