#include <stdint.h>
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

template <typename T> struct size_equals_constraint : constraint {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
};

template <typename T> struct size_not_equals_constraint : constraint {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
};

template <typename T> struct size_lesser_constraint : constraint {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
}

template <typename T> struct size_greater_constraint : constraint {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
}

template <typename T> struct size_lesser_or_equals_constraint : constraint {
    uint32_t value;
    size_lesser_or_equals_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
}

template <typename T> struct size_greater_or_equals_constraint : constraint {
    uint32_t value;
    size_greater_or_equals_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const;
}

template <typename T> bool check_constraints(const T &v, std::vector<constraint> &constraints) {
    for (const auto &c : constraints) {
        if (!c.check(v)) {
            return false;
        }
    }
    return true;
}

}  // namespace gene_internal
