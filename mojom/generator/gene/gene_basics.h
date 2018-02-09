#include <cstdint>
#include <vector>

namespace gene_internal {

using container = std::vector<uint8_t>;

template <typename T> struct serializer {
  bool operator()(const T &v, container &c);
  bool operator()(const container &c, T *v);
};

template <typename T> bool serialize(const T &v, container &c) {
  serializer<T> serializer;
  return serializer(v, c);
}

template <typename T> bool deserialize(const container &c, T *v) {
    serializer<T> serializer;
    return serializer(c, v);
}

template <typename T> struct constraint {
    virtual bool check(const T &v) const = 0;
};

struct value_equals_constraint : constraint<uint32_t> {
    uint32_t value;
    value_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint32_t &v) const {
        return v == value;
    }
};

template <typename T> struct value_not_equals_constraint : constraint<T> {
    T value;
    value_not_equals_constraint(T value) : value(value) {}
    bool check(const T &v) const {
        return v != value;
    }
};

template <typename T> struct value_lesser_constraint : constraint<T> {
    T value;
    value_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const {
        return v < value;
    }
};

template <typename T> struct value_greater_constraint : constraint<T> {
    T value;
    value_greater_constraint(T value) : value(value) {}
    bool check(const T &v) const {
        return v > value;
    }
};

template <typename T> struct size_equals_constraint : constraint<T> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
};

template <typename T> struct size_not_equals_constraint : constraint<T> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
};

template <typename T> struct size_lesser_constraint : constraint<T> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
};

template <typename T> struct size_greater_constraint : constraint<T> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
};

template <typename T> struct compound_constraint : constraint<T> {
    constraint<T> constraint_one;
    constraint<T> constraint_two;

    compound_constraint(constraint<T> constraint_one, constraint<T> constraint_two) : constraint_one(constraint_one),
        constraint_two(constraint_two) {}

    bool check(const T &v) const {
        return constraint_one.check(v) || constraint_two.check(v);
    }
};

template <typename T> bool check_constraints(const T &v, std::vector<constraint<T>> &constraints) {
    for (const auto &c : constraints) {
        if (!c.check(v)) {
            return false;
        }
    }
    return true;
}

}  // namespace gene_internal
