#include <string>
#include <vector>
#include <array>

#include "gene_basics.h"

namespace gene_internal {

// vector constraints

template <typename T> struct size_equals_constraint<std::vector<T>> : constraint<std::vector<T>> {
    uint32_t value;
    explicit size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const std::vector<T> &v) const {
        return v.size() == value;
    }
};

template <typename T> struct size_not_equals_constraint<std::vector<T>> : constraint<std::vector<T>> {
    uint32_t value;
    explicit size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const std::vector<T> &v) const {
        return v.size() != value;
    }
};

template <typename T> struct size_lesser_constraint<std::vector<T>> : constraint<std::vector<T>> {
    uint32_t value;
    explicit size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const std::vector<T> &v) const {
        return v.size() < value;
    }
};

template <typename T> struct size_greater_constraint<std::vector<T>> : constraint<std::vector<T>> {
    uint32_t value;
    explicit size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const std::vector<T> &v) const {
        return v.size() > value;
    }
};

// string constraints

template <> struct size_equals_constraint<std::string> : constraint<std::string> {
    uint32_t value;
    explicit size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const std::string &v) const override {
        return v.size() == value;
    }
};

template <> struct size_not_equals_constraint<std::string> : constraint<std::string> {
    uint32_t value;
    explicit size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const std::string &v) const override {
        return v.size() != value;
    }
};

template <> struct size_lesser_constraint<std::string> : constraint<std::string> {
    uint32_t value;
    explicit size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const std::string &v) const override {
        return v.size() < value;
    }
};

template <> struct size_greater_constraint<std::string> : constraint<std::string> {
    uint32_t value;
    explicit size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const std::string &v) const override {
        return v.size() > value;
    }
};

// array constraints

//template <typename T> struct size_equals_constraint<std::array<T, N>> : constraint<std::array<T, N>> {
//    uint32_t value;
//    explicit size_equals_constraint(uint32_t value) : value(value) {}
//    bool check(const std::array<T, N> &v) const {
//        return v.size() == value;
//    }
//};


}  // namespace gene_internal