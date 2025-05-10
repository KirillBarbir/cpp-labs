#pragma once
#include <vector>
#include <list>
#include <memory>

namespace Scheduler {

class any {
  public:
    struct AnyConcept {
        virtual std::unique_ptr<AnyConcept> clone() const = 0;
        virtual ~AnyConcept() = default;
    };

    template<typename T>
    struct AnyModel : public AnyConcept {
        T inst;

        explicit AnyModel(const T& val) : inst(val) {}

        std::unique_ptr<AnyConcept> clone() const override {
            return std::make_unique<AnyModel>(*this);
        }
    };
    std::unique_ptr<AnyConcept> ptr;

    template<typename T>
    explicit any(T val) : ptr(new AnyModel<T>(val)) {}

    any() = default;

    any(const any& other) : ptr(other.ptr->clone()) {}

    any& operator=(const any& other) {
        ptr = (other.ptr->clone());
        return *this;
    }

};

template<typename T>
T any_cast(const any& in) {
    auto out{static_cast<any::AnyModel<T>*>(in.ptr.get())};
    if (out == nullptr) {
        std::cerr << "bad_any_cast - UB";
    }
    return out->inst;
}

class function {
  public:
    struct FunctionConcept {
        virtual std::unique_ptr<FunctionConcept> clone() const = 0;
        virtual any operator()(any lhs, any rhs) const = 0;
        virtual ~FunctionConcept() = default;
    };

    template<typename T>
    struct FunctionModel : public FunctionConcept {
        T inst;

        explicit FunctionModel(const T& val) : inst(val) {}

        std::unique_ptr<FunctionConcept> clone() const override {
            return std::make_unique<FunctionModel>(*this);
        }

        virtual any operator()(any lhs, any rhs) const override {
            any out(inst(lhs, rhs));
            return out;
        }
    };
    std::unique_ptr<FunctionConcept> ptr;

    template<typename T>
    explicit function(T val) : ptr(new FunctionModel<T>(val)) {}

    function() = default;

    function(const function& other) : ptr(other.ptr->clone()) {}

    function& operator=(const function& other) {
        ptr = (other.ptr->clone());
        return *this;
    }

    any operator()(any lhs, any rhs) {
        auto out = this->ptr.get();
        return (*out)(lhs, rhs);
    }

};

template<typename T, T val>
struct IntegralConstant {

    static constexpr T value = val;

    constexpr operator T() const {
        return value;
    }

    constexpr T operator()() const {
        return value;
    }
};

using true_type = IntegralConstant<bool, true>;
using false_type = IntegralConstant<bool, false>;

template<typename B>
true_type guess(const B*);

template<typename B>
false_type guess(const void*);

template<typename B, typename D>
decltype(guess<B>(static_cast<D*>(nullptr))) is_base_of;

class task {
  public:
    task(function func, const any& lhs, const any& rhs, size_t ID,
         task* dep1 = nullptr, task* dep2 = nullptr) :
        func_(func), lhs_(lhs), rhs_(rhs), ID_(ID), dep1_(dep1), dep2_(dep2) {};

    size_t GetID() {
        return ID_;
    }

    any operator()() {
        if (used_) {
            return res_;
        }
        if (dep1_ != nullptr) {
            lhs_ = (*dep1_)();
        }
        if (dep2_ != nullptr) {
            rhs_ = (*dep2_)();
        }
        auto out = func_(lhs_, rhs_);
        res_ = out;
        used_ = true;
        return out;
    };
  private:
    size_t ID_;
    function func_;
    task* dep1_ = nullptr;
    task* dep2_ = nullptr;
    any lhs_;
    any rhs_;
    any res_ = any(1);
    bool used_ = false;
};

class addReturnClass {
  public:
    addReturnClass(task* ID) : ID_(ID) {};
    task* GetID() {
        return ID_;
    }
  private:
    task* ID_;
};

struct getFutureResultReturnClass_base {};

template<typename T>
class getFutureResultReturnClass : public getFutureResultReturnClass_base {
  public:
    using out_type = T;
    getFutureResultReturnClass(task* ID) : ID_(ID) {};
    task* GetID() {
        return ID_;
    }
  private:
    task* ID_;
};

struct EmptyStruct_base {};
struct EmptyStruct : public EmptyStruct_base {};

class TTaskSheduler {
  public:
    template<typename Func, typename arg1_T = EmptyStruct, typename arg2_T = EmptyStruct>
    addReturnClass add(const Func& func, arg1_T lhs = EmptyStruct(), arg2_T rhs = EmptyStruct()) {
        if constexpr (is_base_of<getFutureResultReturnClass_base, arg1_T>()
            && is_base_of<getFutureResultReturnClass_base, arg2_T>()) {
            function new_func([&func](any any1, any any2) {
                auto ret_arg1 = any_cast<typename arg1_T::out_type>(any1);
                auto ret_arg2 = any_cast<typename arg2_T::out_type>(any2);
                any out(func(ret_arg1, ret_arg2));
                return out;
            });
            task task1(new_func, static_cast<const any>(lhs),
                       static_cast<const any>(rhs), tasks_.size(), lhs.GetID(), rhs.GetID());
            tasks_.push_back(task1);
            addReturnClass RR(&tasks_.back());
            return RR;
        } else if constexpr (is_base_of<getFutureResultReturnClass_base, arg1_T>()) {
            function new_func([&func](any any1, any any2) {
                auto ret_arg1 = any_cast<typename arg1_T::out_type>(any1);
                auto ret_arg2 = any_cast<arg2_T>(any2);
                if constexpr (is_base_of<EmptyStruct_base, arg2_T>()) {
                    any out(func(ret_arg1));
                    return out;
                } else {
                    any out(func(ret_arg1, ret_arg2));
                    return out;
                }
            });
            task task1(new_func, static_cast<const any>(lhs), static_cast<const any>(rhs), tasks_.size(), lhs.GetID());
            tasks_.push_back(task1);
            addReturnClass RR(&tasks_.back());
            return RR;
        } else if constexpr (is_base_of<getFutureResultReturnClass_base, arg2_T>()) {
            function new_func([&func](any any1, any any2) {
                auto ret_arg1 = any_cast<arg1_T>(any1);
                auto ret_arg2 = any_cast<typename arg2_T::out_type>(any2);
                any out(func(ret_arg1, ret_arg2));
                return out;
            });
            task task1(new_func,
                       static_cast<const any>(lhs),
                       static_cast<const any>(rhs),
                       tasks_.size(),
                       nullptr,
                       rhs.GetID());
            tasks_.push_back(task1);
            addReturnClass RR(&tasks_.back());
            return RR;
        } else {
            function new_func([&func](any any1, any any2) {
                auto ret_arg1 = any_cast<arg1_T>(any1);
                auto ret_arg2 = any_cast<arg2_T>(any2);
                if constexpr (is_base_of<EmptyStruct_base, arg1_T>()
                    && is_base_of<EmptyStruct_base, arg2_T>()) {
                    any out(func());
                    return out;
                } else if constexpr (is_base_of<EmptyStruct_base, arg2_T>()) {
                    any out(func(ret_arg1));
                    return out;
                } else {
                    any out(func(ret_arg1, ret_arg2));
                    return out;
                }
            });
            task task1(new_func, static_cast<const any>(lhs), static_cast<const any>(rhs), tasks_.size());
            tasks_.push_back(task1);
            addReturnClass RR(&tasks_.back());
            return RR;
        }

    }

    template<typename T, typename FuncT>
    getFutureResultReturnClass<T> getFutureResult(FuncT cur) {
        getFutureResultReturnClass<T> out(cur.GetID());
        return out;
    }

    template<typename T>
    T getResult(addReturnClass cur) {
        task cur_task = *cur.GetID();
        return any_cast<T>(cur_task());
    }

    void executeAll() {
        for (task cur_task : tasks_) {
            cur_task();
        }
    }

  private:
    std::list<task> tasks_;
};

}