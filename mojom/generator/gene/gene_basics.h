#include <cstdint>
#include <vector>

namespace gene_internal {

using container = std::vector<uint8_t>;

bool send_message_internal(const container &c);
bool receive_message_internal(container *c);

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

// value constraints

template <typename T> struct value_equals_constraint : constraint<T> {
    T value;
    value_equals_constraint(T value) : value(value) {}
    bool check(const T &v) const {
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
    value_lesser_constraint(T value) : value(value) {}
    bool check(const T &v) const {
        return v < value;
    }
};

template <typename T> struct value_greater_constraint : constraint<T> {
    T value;
    value_greater_constraint(T value) : value(value) {}
    bool check(const T &v) const {
        return v> value;
    }
};

// size constraints

template <typename T> struct size_equals_constraint : constraint<T> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const {
        return sizeof(T) == value;
    }
};

template <typename T> struct size_not_equals_constraint : constraint<T> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const {
        return sizeof(T) != value;
    }
};

template <typename T> struct size_lesser_constraint : constraint<T> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const {
        return sizeof(T) < value;
    }
};

template <typename T> struct size_greater_constraint : constraint<T> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const T &v) const {
        return sizeof(T) > value;
    }
};

template <typename T> struct compound_constraint : constraint<T> {
    const constraint<T> &constraint_one;
    const constraint<T> &constraint_two;

    compound_constraint(const constraint<T> &constraint_one, const constraint<T> &constraint_two) : constraint_one(constraint_one),
        constraint_two(constraint_two) {}

    bool check(const T &v) const {
        return constraint_one.check(v) || constraint_two.check(v);
    }
};

// message sending & receiving

template <typename T> bool send_message(const T &v) {
    container c;
    bool res = serialize(v, c);
    if (!res)
        return false;

    for (int i = 0; i < 8; i++) {
        int pos_to_move = 8 * (7 - i);
        c.push_back((T::__type_id & (0b11111111 << pos_to_move)) >> pos_to_move);
    }

    return send_message_internal(c);
}

template <typename T> bool receive_message(T *v) {
    container c;
    if (!receive_message_internal(&c)) {
        return false;
    }

    uint64_t received_type_id = 0;

    for (int i = 0; i < 8; i++) {
        uint64_t part = c.back();
        c.pop_back();
        received_type_id |= part << (8 * i);
    }

    if (received_type_id != T::__type_id) {
        return false;
    } else {
        return deserialize(c, v);
    }
}

}  // namespace gene_internal
