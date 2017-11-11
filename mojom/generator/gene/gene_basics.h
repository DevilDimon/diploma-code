#include <stdint.h>
#include <vector>

namespace gene_internal {

using container = std::vector<uint8_t>;

template <typename T> struct serializer {
  void operator()(const T &v, container &c);
};

template <typename T> void serialize(const T &v, container &c) {
  serializer<T> serializer;
  serializer(v, c);
}

template <typename T> struct size_checker {
    bool check_equal_size(const T &v, uint32_t size);
    bool check_greater_size(const T &v, uint32_t size);
    bool check_lesser_size(const T &v, uint32_t size);
};

template <typename T> bool check_equal_size(const T &v, uint32_t size) {
    size_checker<T> checker;
    return checker.check_equal_size(v, size);
}

template <typename T> bool check_greater_size(const T &v, uint32_t size) {
    size_checker<T> checker;
    return checker.check_greater_size(v, size);
}

template <typename T> bool check_lesser_size(const T &v, uint32_t size) {
    size_checker<T> checker;
    return checker.check_lesser_size(v, size);
}

}  // namespace gene_internal
