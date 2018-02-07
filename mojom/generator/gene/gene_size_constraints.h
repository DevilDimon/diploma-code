#include "gene_basics.h"
#include <string>
#include <vector>

namespace gene_internal {

// size_equals_constraint

template <> struct size_equals_constraint<int8_t> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const int8_t &v) {
        return value == 8;
    }
};

template <> struct size_equals_constraint<int16_t> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const int16_t &v) {
        return value == 16;
    }
};

template <> struct size_equals_constraint<int32_t> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const int32_t &v) {
        return value == 32;
    }
};

template <> struct size_equals_constraint<int64_t> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const int64_t &v) {
        return value == 64;
    }
};

template <> struct size_equals_constraint<uint8_t> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint8_t &v) {
        return value == 8;
    }
};

template <> struct size_equals_constraint<uint16_t> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint16_t &v) {
        return value == 16;
    }
};

template <> struct size_equals_constraint<uint32_t> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint32_t &v) {
        return value == 32;
    }
};

template <> struct size_equals_constraint<uint64_t> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint64_t &v) {
        return value == 64;
    }
};

template <> struct size_equals_constraint<float> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const float &v) {
        return value == 32;
    }
};

template <> struct size_equals_constraint<double> {
    uint32_t value;
    size_equals_constraint(uint32_t value) : value(value) {}
    bool check(const double &v) {
        return value == 64;
    }
};

// size_not_equals_constraint

template <> struct size_not_equals_constraint<int8_t> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const int8_t &v) {
        return value != 8;
    }
};

template <> struct size_not_equals_constraint<int16_t> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const int16_t &v) {
        return value != 16;
    }
};

template <> struct size_not_equals_constraint<int32_t> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const int32_t &v) {
        return value != 32;
    }
};

template <> struct size_not_equals_constraint<int64_t> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const int64_t &v) {
        return value != 64;
    }
};

template <> struct size_not_equals_constraint<uint8_t> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint8_t &v) {
        return value != 8;
    }
};

template <> struct size_not_equals_constraint<uint16_t> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint16_t &v) {
        return value != 16;
    }
};

template <> struct size_not_equals_constraint<uint32_t> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint32_t &v) {
        return value != 32;
    }
};

template <> struct size_not_equals_constraint<uint64_t> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const uint64_t &v) {
        return value != 64;
    }
};

template <> struct size_not_equals_constraint<float> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const float &v) {
        return value != 32;
    }
};

template <> struct size_not_equals_constraint<double> {
    uint32_t value;
    size_not_equals_constraint(uint32_t value) : value(value) {}
    bool check(const double &v) {
        return value != 64;
    }
};

// size_lesser_constraint

template <> struct size_lesser_constraint<int8_t> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const int8_t &v) {
        return value > 8;
    }
};

template <> struct size_lesser_constraint<int16_t> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const int16_t &v) {
        return value > 16;
    }
};

template <> struct size_lesser_constraint<int32_t> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const int32_t &v) {
        return value > 32;
    }
};

template <> struct size_lesser_constraint<int64_t> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const int64_t &v) {
        return value > 64;
    }
};

template <> struct size_lesser_constraint<uint8_t> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const uint8_t &v) {
        return value > 8;
    }
};

template <> struct size_lesser_constraint<uint16_t> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const uint16_t &v) {
        return value > 16;
    }
};

template <> struct size_lesser_constraint<uint32_t> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const uint32_t &v) {
        return value > 32;
    }
};

template <> struct size_lesser_constraint<uint64_t> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const uint64_t &v) {
        return value > 64;
    }
};

template <> struct size_lesser_constraint<float> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const float &v) {
        return value > 32;
    }
};

template <> struct size_lesser_constraint<double> {
    uint32_t value;
    size_lesser_constraint(uint32_t value) : value(value) {}
    bool check(const double &v) {
        return value > 64;
    }
};

// size_greater_constraint

template <> struct size_greater_constraint<int8_t> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const int8_t &v) {
        return value < 8;
    }
};

template <> struct size_greater_constraint<int16_t> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const int16_t &v) {
        return value < 16;
    }
};

template <> struct size_greater_constraint<int32_t> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const int32_t &v) {
        return value < 32;
    }
};

template <> struct size_greater_constraint<int64_t> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const int64_t &v) {
        return value < 64;
    }
};

template <> struct size_greater_constraint<uint8_t> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const uint8_t &v) {
        return value < 8;
    }
};

template <> struct size_greater_constraint<uint16_t> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const uint16_t &v) {
        return value < 16;
    }
};

template <> struct size_greater_constraint<uint32_t> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const uint32_t &v) {
        return value < 32;
    }
};

template <> struct size_greater_constraint<uint64_t> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const uint64_t &v) {
        return value < 64;
    }
};

template <> struct size_greater_constraint<float> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const float &v) {
        return value < 32;
    }
};

template <> struct size_greater_constraint<double> {
    uint32_t value;
    size_greater_constraint(uint32_t value) : value(value) {}
    bool check(const double &v) {
        return value < 64;
    }
};

}  // namespace gene_internal