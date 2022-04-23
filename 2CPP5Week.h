#pragma once

#include <future>
#include <utility>
#include <vector>
#include <iterator>
#include <numeric>
// заголовок <future> уже подключён.
// заново подключать не нужно

// реализация функции mapreduce

template<typename Iter, typename Func1, typename Func2>
auto F(Iter p, Iter q, Func1 f1, Func2 f2)->decltype(f2(f1(*p), f1(*p))) {
    using TypeR = decltype(f2(f1(*p), f1(*p)));
    //TypeR Res;
    //while (p != q) {
    //    Res = f1(*p);
    //    p++;
    //}
    TypeR Res = f1(*p);
    while (++p != q)
       Res = f2(Res, f1(*p)); //В ОСНОВНОМ ПОТОКЕ??
    return Res;
}
template <typename Iter, typename Func1 , typename Func2>
auto map_reduce1(Iter p, Iter q, Func1 f1, Func2 f2, size_t threads) -> decltype(f2(f1(*p), f1(*p))) {

    using TypeR = decltype(f2(f1(*p), f1(*p)));

    std::vector<std::future<TypeR>> Res(threads);
    size_t Size = std::distance(p, q);
   //20/6 = 3 =>
	for (int i = 0; i != threads; i++) { //нужно правильно разбить на подстроки!!!
            //auto Lmbd = [=p+, =q](int p_, int q_) { return; }; //Захватываем нужную подстроку по значению
			//auto Lmbd = []() { return; }
        while (p != q) { q--; } //p=q
        if (i == (threads - 1)) {
            for (int j = 0; j != ((Size / threads) +(Size % threads)) ; ++j) {
                q++;//std::advance(q, (Size / threads) + (Size % threads)); 
            }
        }
        else {
            for (int j = 0; j != (Size / threads); ++j) {
                q++;//std::advance(q, (Size / threads));
            }
        }
        Res[i] = std::async(std::launch::async, F<Iter, Func1, Func2>, p, q, f1, f2);
        p = q;
	}
    //auto Lambda = [&f1, &f2](Iter p, Iter q)-> TypeR {  }
    //std::vector<std::future<TypeR>> TheResult(threads);
    int i = 0; //достаем результат обратно КАК ПРАВИЛЬНО??
    //    //TheResult[i] = 
//  //      return Res[i].get();
    //     Res[i].get();
    TypeR RetRes = Res[i].get();
    while (++i != threads)
    RetRes = f2(RetRes, Res[i].get()); //В ОСНОВНОМ ПОТОКЕ??
    return RetRes;
    //RetRes = std::accumulate(Res[0].get(), Res[threads-1].get(), 0, f2);  
    //S = std::accumulate(Res[0], Res[threads])
}
//using TypeR = decltype(f(*p)); будем определять тип возвращаемого значения
//auto Lambda = [&f1, &f2](Iterator p, Iterator q)-> TypeR { ... }
//не передавать итераторы по ссылкам, что бы потоки работали с разными итераторами
//использовать std::launch::async для асинхронной работы 
//???какие данные надо копировать в лямбду, а к каким можно обращаться по ссылке??

//разбить на порции(определение размера, расчёт каждой порции и работа с итераторами)(каждый поток должен знать счем ему работать)
//для каждой порции создать свою лямбду с захватом нужных полей
//создать все future и засунунть в контейнер
//пройтись по контейнеру достать результат каждой пачки и собрать в итоговый результат с помощью f2
//==============================================================================================
/*#include <thread>// уже подключен 
#include <vector>
#include <iterator>
#include <numeric>

template<typename Iter, typename Func1, typename Func2>
auto FF(Iter p, Iter q, Func1 f1, Func2 f2) -> decltype(f2(f1(*p), f1(*p))) {
    using TypeR = decltype(f2(f1(*p), f1(*p)));
    TypeR Res = f1(*p);
    while (++p != q)
        Res = f2(Res, f1(*p)); 
    return Res;
}

template <typename Iter, typename Func1, typename Func2>
auto map_reduce(Iter p, Iter q, Func1 f1, Func2 f2, size_t threads) -> decltype(f2(f1(*p), f1(*p))) {

    using TypeR = decltype(f2(f1(*p), f1(*p)));

    //std::vector<std::future<TypeR>> Res(threads);
    std::vector<std::thread> Strms(threads);
    //std::thread Streams[threads];

    size_t Size = std::distance(p, q);

    for (int i = 0; i != threads; i++) {
        while (p != q) { q--; }
        if (i == (threads - 1)) {
            for (int j = 0; j != ((Size / threads) + (Size % threads)); ++j) {
                q++;
            }
        }
        else {
            for (int j = 0; j != (Size / threads); ++j) {
                q++;
            }
        }
        //Res[i] = std::async(std::launch::async, FF<Iter, Func1, Func2>, p, q, f1, f2);
        Strms[i] = std::move(std::thread([&Strms]() { Strms[i] = FF(p, q, f1, f2); }));
        p = q;
    }
    
    for (int i = 0; i < threads; i++) {
        Strms[i].join();
    }

    int i = 0; 
    TypeR RetStrms = Strms[i];
    while (++i != threads)
        RetStrms = f2(RetStrms, Strms[i]); //В ОСНОВНОМ ПОТОКЕ??
    return RetStrms;
}*/
//std::vector<std::thread> StreamVector;
//for (int i = 0; i!= threads; i++)
//StreamVector.push_back(std::thread(&foo, i));
//for (auto & th: StreamVctor)
//th.join();
//======================================================================================

// определите метафункцию Fib<N>
template <int N>
struct Fib {
    static int const
        value = Fib<N - 1>::value + Fib<N - 2>::value;
};

template<>
struct Fib<0> {
    static int const value = 0;
};

template<>
struct Fib<1> {
    static int const value = 1;
};
//======================================================================================

// Определение списка целых чисел времени компиляции IntList
template <int ... Numb>
struct IntList;

template <int H, int ... T>
struct IntList<H, T...> {
    static int const Head = H;
    using Tail = IntList<T...>;
};

template<>
struct IntList<> { };
//=====================================================================================

// определите метафункцию Length для вычисления длины списка
template <typename IL = IntList<>>
struct Length {
    static int const value = 1 + Length<typename IL::Tail>::value;
};

template <>
struct Length<IntList<> > {
    static int const value = 0;
};
//=====================================================================================

// тип IntList уже определён
// реализация метафункции IntCons
template <int Val = 0, typename IL = IntList<>>
struct IntCons;

template <int Val, int... Pr>
struct IntCons<Val, IntList<Pr...>> {
    using type = IntList<Val, Pr...>;
};

// реализация метафункции Generate
template <typename IL = IntList<>, int Val = 0>
struct MyIntCons;
template <int... Pr, int Val>
struct MyIntCons<IntList<Pr...>, Val> {
    using type = IntList<Pr..., Val>;
};

template <int N = 0, typename IL = IntList<>>
struct Generate {
    static const int Length = N;
    using type = typename MyIntCons<typename Generate<N - 1>::type, Generate<N - 1>::Length>::type;
};

template<>
struct Generate<0> {
    static const int Length = 0;
    using type = IntList<>;
};
//==============================================================================

#include <tuple>
// тип IntList и метафункция Generate уже определены


//использовать std::forward для передачи r-value ссылок
//принимать кортеж по значению
//Функция apply() с двумя параметрами и шаблонным типом по умолчанию должна рекурсивно вызвать перегруженную apply() с третьим шаблонным параметром int
//sizeof ... (Args) вернет количество аргументов
//не забывать typename перед Generate
// 
// 
//
template<typename Func, typename... Args, int... Val>
auto HelpApply(Func F, std::tuple<Args...> Tpl, IntList<Val...>)
    -> decltype(F(std::forward<Args>(std::get<Val>(Tpl))...)) {

    return F(std::forward<Args>(std::get<Val>(Tpl))...);
}

template<typename Func, typename ...Args>
auto apply(Func F, std::tuple<Args...> Tpl) 
-> decltype(HelpApply(F, Tpl, typename Generate<sizeof...(Args)>::type{ })) {

    auto Ret = HelpApply(F, Tpl, typename Generate<sizeof ...(Args)>::type{ });
    return Ret;
}
//==================================================================================

// Определение метафункции Zip

template <typename IL1, typename IL2, template <int, int> class BinF>
struct Zip {
    using type = typename IntCons< BinF< IL1::Head, IL2::Head>::value, typename Zip<typename IL1::Tail, typename IL2::Tail, BinF>::type >::type;
};

template <template <int, int> class BinF>
struct Zip<IntList<>, IntList<>, BinF> {
    using type = IntList<>;
};

//==================================================================================
#include <string>
#include <algorithm>

template <template <class...> class Container, class T, class... Args>
Container<std::string> to_strings(const Container<T, Args...>& container) {
    std::size_t size = container.size();
    Container<std::string> strings(size);
    std::transform(container.begin(), container.end(), strings.begin(),
        [](const T& value) { return std::to_string(value); });
    return strings;
}
//======================================================================
#include <type_traits>
template<>
struct has_mtd {
    typedef char YES;
    struct NO { YES m[2]; };
    template <class M, size_t(M::*)() const = &M::size>
    struct HasMtdWrapper { };

    template <class C>
    static YES check(HasMtdWrapper<C>* mtd);
    template <class C>
    static NO check(...);

    static bool const value = sizeof(YES) == sizeof(check<T>(0));
};

template <typename T>
struct has_fld {
    typedef char YES;
    struct NO { YES m[2]; };

    template <class F, size_t(F::*) = &F::size>
    struct HasFltWrapper { };

    template <class C>
    static YES check(HasFltWrapper<C>* mtd);
    template <class C>
    static NO check(...);

    static bool const value = sizeof(YES) == sizeof(check<T>(0));
};

template <class T>
typename std::enable_if<has_mtd<T>::value, size_t>::type get_size(T &t) {
    size_t(T::*mtd)() const = &T::size;
    return (t.*mtd)();
}

template <class T>
typename std::enable_if<has_fld<T>::value, size_t>::type get_size(T &t) {
    size_t(T::*fld) = &T::size;
    return (t.*fld);
}



//is_foo_defind понять
// 
// strust U{
//     unsigned size() const;
//     unsigned size;
// }
// size_t(T::*)() const = &T::size; //указатель на метод класса 
// size_t (T::*)        = &T::size; //указатель на поле класса 
//
