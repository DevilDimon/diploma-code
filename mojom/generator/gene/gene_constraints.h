#include <string>
#include <vector>

#include "gene_basics.h"

namespace gene_internal {

// vector constraints

template <typename T> struct size_equals_constraint<std::vector<T>> : constraint<std::vector<T>> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const std::vector<T> &v) const {
        return v.size() == value;
    }
};

template <typename T> struct size_not_equals_constraint<std::vector<T>> : constraint<std::vector<T>> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const std::vector<T> &v) const {
        return v.size() != value;
    }
};

template <typename T> struct size_lesser_constraint<std::vector<T>> : constraint<std::vector<T>> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const std::vector<T> &v) const {
        return v.size() < value;
    }
};

template <typename T> struct size_greater_constraint<std::vector<T>> : constraint<std::vector<T>> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const std::vector<T> &v) const {
        return v.size() > value;
    }
};


}  // namespace gene_internal