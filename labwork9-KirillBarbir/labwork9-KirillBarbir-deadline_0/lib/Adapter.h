#pragma once

struct IsView {};
struct IsWrapper {};

template<typename T>
concept Range = requires(T a){
    a.begin();
    a.end();
    typename T::const_iterator;
};

template<typename T>
concept Iteratable = Range<T> && requires(T::const_iterator lhs, T::const_iterator rhs){
    ++lhs;
    *lhs;
    lhs != rhs;
};

template<typename T>
concept BidirIteratable = Iteratable<T> && requires(T::const_iterator lhs){
    --lhs;
};

template<typename T>
concept Wrap = std::derived_from<T, IsWrapper>;

template<typename T>
concept Associative = requires(T a){
    typename T::value_type;
    typename T::key_type;
    typename T::mapped_type;
    (*a.begin()).first;
    (*a.begin()).second;
}  && Iteratable<T> || std::is_same_v<typename T::Associative, std::true_type>;



// TRANSFORM ****************************************************************************************************

template<typename cur_view, typename lambda> requires Iteratable<cur_view>
class TransformView : public IsView {
  public:
    using range_type = std::conditional<std::derived_from<cur_view, IsView>, cur_view, const cur_view&>::type;
    using Associative = std::conditional<Associative<cur_view>, std::true_type, std::false_type>::type;
    class const_iterator {
      public:
        using difference_type = cur_view;
        using value_type = cur_view;
        using pointer = cur_view*;
        using reference = cur_view&;
        using iterator_category = std::bidirectional_iterator_tag;
        const_iterator(cur_view::const_iterator it, lambda func) : cur_it(it), _func(func) {};
        const_iterator& operator++() {
            ++cur_it;
            return *this;
        }
        const_iterator& operator--() requires BidirIteratable<cur_view> {
            --cur_it;
            return *this;
        }
        auto operator*() {
            auto out = *(cur_it);
            out = _func(out);
            return out;
        }
        bool operator!=(const_iterator rhs) {
            return this->cur_it != rhs.cur_it;
        }
      private:
        cur_view::const_iterator cur_it;
        lambda _func;
    };

    TransformView(range_type v, lambda func) : v_(v), func_(func) {};

    const_iterator begin() const {
        const_iterator beg(v_.begin(), func_);
        return beg;
    }

    const_iterator end() const {
        const_iterator en(v_.end(), func_);
        return en;
    }

  private:
    range_type v_;
    lambda func_;
};

template<typename lambda>
class FunctionaryTransformView : public IsWrapper {
  public:
    using function_type = lambda;

    FunctionaryTransformView(lambda func) : func_(func) {};

    lambda GetFunction() {
        return func_;
    }

    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    TransformView<cur_view, lambda> operator()(cur_view v) {
        TransformView<cur_view, lambda> out(v, func_);
        return (out);
    }

    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    TransformView<cur_view, lambda> operator()(cur_view& v) {
        TransformView<cur_view, lambda> out(v, func_);
        return (out);
    }

  private:
    lambda func_;
};

class Transform_ : public IsWrapper {
  public:
    template<typename lambda>
    FunctionaryTransformView<lambda> operator()(lambda func) {
        FunctionaryTransformView<lambda> out(func);
        return (out);
    }

    template<typename cur_view, typename lambda, typename = std::enable_if_t<
        std::derived_from<cur_view, IsView> && !std::derived_from<lambda, IsWrapper>>>
    requires Iteratable<cur_view>
    TransformView<cur_view, lambda> operator()(cur_view v, lambda func) {
        TransformView<cur_view, lambda> out(v, func);
        return (out);
    }

    template<typename cur_view, typename lambda, typename = std::enable_if_t<
        !std::derived_from<cur_view, IsView> && !std::derived_from<lambda, IsWrapper>>>
    requires Iteratable<cur_view>
    TransformView<cur_view, lambda> operator()(const cur_view& v, lambda func) {
        TransformView<cur_view, lambda> out(v, func);
        return (out);
    }

    template<typename cur_view_lhs, typename cur_view_rhs, typename = std::enable_if_t<
        std::derived_from<cur_view_lhs, IsView> && std::derived_from<cur_view_rhs, IsWrapper>>>
    requires Iteratable<cur_view_lhs>
    TransformView<cur_view_lhs, typename cur_view_rhs::function_type> operator()(cur_view_lhs lhs, cur_view_rhs rhs) {
        using lambda = cur_view_rhs::function_type;
        TransformView<cur_view_lhs, lambda> out(lhs, rhs.GetFunction());
        return (out);
    }

    template<typename cur_view_lhs, typename cur_view_rhs, typename = std::enable_if_t<
        !std::derived_from<cur_view_lhs, IsView> && std::derived_from<cur_view_rhs, IsWrapper>>>
    requires Iteratable<cur_view_lhs>
    TransformView<cur_view_lhs, typename cur_view_rhs::function_type> operator()(const cur_view_lhs& lhs,
                                                                                 cur_view_rhs rhs) {
        using lambda = cur_view_rhs::function_type;
        TransformView<cur_view_lhs, lambda> out(lhs, rhs.GetFunction());
        return (out);
    }
};

inline Transform_ transform;

// FILTER ****************************************************************************************************

template<typename cur_view, typename lambda> requires Iteratable<cur_view>
class FilterView : public IsView {
  public:
    using range_type = std::conditional<std::derived_from<cur_view, IsView>, cur_view, const cur_view&>::type;
    using Associative = std::conditional<Associative<cur_view>, std::true_type, std::false_type>::type;

    class const_iterator {
      public:
        using difference_type = cur_view;
        using value_type = cur_view;
        using pointer = cur_view*;
        using reference = cur_view&;
        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator(cur_view::const_iterator it, lambda func, cur_view::const_iterator it2)
            : cur_it(it), _func(func), end_it(it2) {};
        const_iterator& operator++() {
            ++cur_it;
            while (!_func((*cur_it)) && end_it != cur_it) {
                ++cur_it;
            }
            return *this;
        }
        const_iterator& operator--() requires BidirIteratable<cur_view> {
            --cur_it;
            while (!_func((*cur_it)) && end_it != cur_it) {
                --cur_it;
            }
            return *this;
        }
        auto operator*() {
            return *(cur_it);
        }
        bool operator!=(const_iterator rhs) {
            return this->cur_it != rhs.cur_it;
        }
      private:
        cur_view::const_iterator cur_it;
        cur_view::const_iterator end_it;
        lambda _func;
    };

    FilterView(range_type v, lambda func) : v_(v), func_(func) {};

    const_iterator begin() const {
        const_iterator it(v_.begin(), func_, v_.end());
        const_iterator en(v_.end(), func_, v_.end());
        while (it != en && func_(*it)) {
            ++it;
        }
        return it;
    }

    const_iterator end() const {
        const_iterator en(v_.end(), func_, v_.end());
        return en;
    }

  private:
    range_type v_;
    lambda func_;
};

template<typename lambda>
class FunctionaryFilterView : public IsWrapper {
  public:
    using function_type = lambda;
    FunctionaryFilterView(lambda func) : func_(func) {};
    lambda GetFunction() {
        return func_;
    }

    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    FilterView<cur_view, lambda> operator()(cur_view v) {
        FilterView<cur_view, lambda> out(v, func_);
        return (out);
    }

    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    FilterView<cur_view, lambda> operator()(cur_view& v) {
        FilterView<cur_view, lambda> out(v, func_);
        return (out);
    }
  private:
    lambda func_;
};

class Filter_ : public IsWrapper {
  public:
    template<typename lambda>
    FunctionaryFilterView<lambda> operator()(lambda func) {
        FunctionaryFilterView<lambda> out(func);
        return (out);
    }
    template<typename cur_view, typename lambda, typename = std::enable_if_t<
        std::derived_from<cur_view, IsView> && !std::derived_from<lambda, IsWrapper>>>
    requires Iteratable<cur_view>
    FilterView<cur_view, lambda> operator()(cur_view v, lambda func) {
        FilterView<cur_view, lambda> out(v, func);
        return (out);
    }
    template<typename cur_view, typename lambda, typename = std::enable_if_t<
        !std::derived_from<cur_view, IsView> && !std::derived_from<lambda, IsWrapper>>>
    requires Iteratable<cur_view>
    FilterView<cur_view, lambda> operator()(const cur_view& v, lambda func) {
        FilterView<cur_view, lambda> out(v, func);
        return (out);
    }
    template<typename cur_view_lhs, typename cur_view_rhs, typename = std::enable_if_t<
        std::derived_from<cur_view_lhs, IsView> && std::derived_from<cur_view_rhs, IsWrapper>>>
    requires Iteratable<cur_view_lhs>
    FilterView<cur_view_lhs, typename cur_view_rhs::function_type> operator()(cur_view_lhs lhs, cur_view_rhs rhs) {
        using lambda = cur_view_rhs::function_type;
        FilterView<cur_view_lhs, lambda> out(lhs, rhs.GetFunction());
        return (out);
    }
    template<typename cur_view_lhs, typename cur_view_rhs, typename = std::enable_if_t<
        !std::derived_from<cur_view_lhs, IsView> && std::derived_from<cur_view_rhs, IsWrapper>>>
    requires Iteratable<cur_view_lhs>
    FilterView<cur_view_lhs, typename cur_view_rhs::function_type> operator()(const cur_view_lhs& lhs,
                                                                              cur_view_rhs rhs) {
        using lambda = cur_view_rhs::function_type;
        FilterView<cur_view_lhs, lambda> out(lhs, rhs.GetFunction());
        return (out);
    }
};

inline Filter_ filter;

// REVERSE ****************************************************************************************************

template<typename cur_view> requires BidirIteratable<cur_view>
class ReverseView : public IsView {
  public:
    using range_type = std::conditional<std::derived_from<cur_view, IsView>, cur_view, const cur_view&>::type;
    using const_iterator = std::reverse_iterator<typename cur_view::const_iterator>;
    using Associative = std::conditional<Associative<cur_view>, std::true_type, std::false_type>::type;

    ReverseView(range_type v) : v_(v) {};

    const_iterator begin() const {
        return std::make_reverse_iterator(v_.end());
    }

    const_iterator end() const {
        return std::make_reverse_iterator(v_.begin());
    }

  private:
    range_type v_;
};

class Reverse_ : public IsWrapper {
  public:
    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    requires BidirIteratable<cur_view>
    ReverseView<cur_view> operator()(cur_view v) {
        ReverseView<cur_view> out(v);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    requires BidirIteratable<cur_view>
    ReverseView<cur_view> operator()(cur_view& v) {
        ReverseView<cur_view> out(v);
        return (out);
    }
};

inline Reverse_ reverse;

// DROP ****************************************************************************************************
template<typename cur_view> requires Iteratable<cur_view>
class DropView : public IsView {
  public:
    using range_type = std::conditional<std::derived_from<cur_view, IsView>, cur_view, const cur_view&>::type;
    using const_iterator = cur_view::const_iterator;
    using Associative = std::conditional<Associative<cur_view>, std::true_type, std::false_type>::type;

    DropView(range_type v, size_t n) : v_(v), n_(n) {};

    const_iterator begin() const {
        const_iterator it = v_.begin();
        size_t temp = n_;
        while (temp > 0 && it != v_.end()) {
            ++it;
            --temp;
        }
        return it;
    }

    const_iterator end() const {
        return v_.end();
    }

  private:
    range_type v_;
    size_t n_;
};

class Drop_ : public IsWrapper {
  public:
    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    requires Iteratable<cur_view>
    DropView<cur_view> operator()(cur_view v, size_t n) {
        DropView<cur_view> out(v, n);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    requires Iteratable<cur_view>
    DropView<cur_view> operator()(cur_view& v, size_t n) {
        DropView<cur_view> out(v, n);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    requires Iteratable<cur_view>
    DropView<cur_view> operator()(cur_view v) {
        DropView<cur_view> out(v, n_);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    requires Iteratable<cur_view>
    DropView<cur_view> operator()(cur_view& v) {
        DropView<cur_view> out(v, n_);
        return (out);
    }
    Drop_ operator()(size_t n) {
        n_ = n;
        return *this;
    }

  private:
    size_t n_;
};

inline Drop_ drop;

// TAKE ****************************************************************************************************
template<typename cur_view> requires Iteratable<cur_view>
class TakeView : public IsView {
  public:
    using range_type = std::conditional<std::derived_from<cur_view, IsView>, cur_view, const cur_view&>::type;
    using const_iterator = cur_view::const_iterator;
    using Associative = std::conditional<Associative<cur_view>, std::true_type, std::false_type>::type;

    TakeView(range_type v, size_t n) : v_(v), n_(n) {};

    const_iterator begin() const {
        return v_.begin();
    }

    const_iterator end() const {
        const_iterator it = v_.begin();
        size_t temp = n_;
        while (it != v_.end() && temp > 0) {
            ++it;
            --temp;
        }
        return it;
    }

  private:
    range_type v_;
    size_t n_;
};

class Take_ : public IsWrapper {
  public:
    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    requires Iteratable<cur_view>
    TakeView<cur_view> operator()(cur_view v, size_t n) {
        TakeView<cur_view> out(v, n);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    requires Iteratable<cur_view>
    TakeView<cur_view> operator()(cur_view& v, size_t n) {
        TakeView<cur_view> out(v, n);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    requires BidirIteratable<cur_view>
    TakeView<cur_view> operator()(cur_view v) {
        TakeView<cur_view> out(v, n_);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    requires BidirIteratable<cur_view>
    TakeView<cur_view> operator()(cur_view& v) {
        TakeView<cur_view> out(v, n_);
        return (out);
    }
    Take_ operator()(size_t n) {
        n_ = n;
        return *this;
    }

  private:
    size_t n_;
};

inline Take_ take;

// KEYS ****************************************************************************************************
template<typename cur_view> requires Associative<cur_view>
class KeysView : public IsView {
  public:
    using range_type = std::conditional<std::derived_from<cur_view, IsView>, cur_view, const cur_view&>::type;

    class const_iterator {
      public:
        using difference_type = cur_view;
        using value_type = cur_view;
        using pointer = cur_view*;
        using reference = cur_view&;
        using iterator_category = std::bidirectional_iterator_tag;
        const_iterator(cur_view::const_iterator it) : cur_it(it) {};
        const_iterator& operator++() {
            ++cur_it;
            return *this;
        }
        const_iterator& operator--() requires BidirIteratable<cur_view> {
            --cur_it;
            return *this;
        }
        auto operator*() {
            auto out = *(cur_it);
            return out.first;
        }
        bool operator!=(const_iterator rhs) {
            return this->cur_it != rhs.cur_it;
        }
      private:
        cur_view::const_iterator cur_it;
    };

    KeysView(range_type v) : v_(v) {};

    const_iterator begin() const {
        const_iterator it(v_.begin());
        return it;
    }

    const_iterator end() const {
        const_iterator it(v_.end());
        return it;
    }

  private:
    range_type v_;
};

class Keys_ : public IsWrapper {
  public:
    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    requires Associative<cur_view>
    KeysView<cur_view> operator()(cur_view v) {
        KeysView<cur_view> out(v);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    requires Associative<cur_view>
    KeysView<cur_view> operator()(cur_view& v) {
        KeysView<cur_view> out(v);
        return (out);
    }
};

inline Keys_ keys;

// VALUES ****************************************************************************************************
template<typename cur_view> requires Associative<cur_view>
class ValuesView : public IsView {
  public:
    using range_type = std::conditional<std::derived_from<cur_view, IsView>, cur_view, const cur_view&>::type;

    class const_iterator {
      public:
        using difference_type = cur_view;
        using value_type = cur_view;
        using pointer = cur_view*;
        using reference = cur_view&;
        using iterator_category = std::bidirectional_iterator_tag;
        const_iterator(cur_view::const_iterator it) : cur_it(it) {};
        const_iterator& operator++() {
            ++cur_it;
            return *this;
        }
        const_iterator& operator--() requires BidirIteratable<cur_view> {
            --cur_it;
            return *this;
        }
        auto operator*() {
            auto out = *(cur_it);
            return out.second;
        }
        bool operator!=(const_iterator rhs) {
            return this->cur_it != rhs.cur_it;
        }
      private:
        cur_view::const_iterator cur_it;
    };

    ValuesView(range_type v) : v_(v) {};

    const_iterator begin() const {
        const_iterator it(v_.begin());
        return it;
    }

    const_iterator end() const {
        const_iterator it(v_.end());
        return it;
    }

  private:
    range_type v_;
};

class Values_ : public IsWrapper {
  public:
    template<typename cur_view, typename = std::enable_if_t<std::derived_from<cur_view, IsView>>>
    requires Associative<cur_view>
    ValuesView<cur_view> operator()(cur_view v) {
        ValuesView<cur_view> out(v);
        return (out);
    }
    template<typename cur_view, typename = std::enable_if_t<!std::derived_from<cur_view, IsView>>>
    requires Associative<cur_view>
    ValuesView<cur_view> operator()(cur_view& v) {
        ValuesView<cur_view> out(v);
        return (out);
    }
};

inline Values_ values;

// Pipes
template<typename cur_view_lhs, typename cur_view_rhs, typename = std::enable_if_t<
    std::derived_from<cur_view_lhs, IsView>>>
requires (Iteratable<cur_view_lhs> && Wrap<cur_view_rhs>)
auto operator|(cur_view_lhs lhs, cur_view_rhs rhs) {
    return rhs(lhs);
}

template<typename cur_view_lhs, typename cur_view_rhs, typename = std::enable_if_t<
    !std::derived_from<cur_view_lhs, IsView>>>
requires (Iteratable<cur_view_lhs> && Wrap<cur_view_rhs>)
auto operator|(const cur_view_lhs& lhs, cur_view_rhs rhs) {
    return rhs(lhs);
}