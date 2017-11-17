#include "gene_basics.h"
#include <string>
#include <vector>

namespace gene_internal {

// int types
template <> struct serializer<int8_t> {
  void operator()(const int8_t &v, container &c) {}
};

template <> struct serializer<int16_t> {
  void operator()(const int16_t &v, container &c) {}
};

template <> struct serializer<int32_t> {
  void operator()(const int32_t &v, container &c) {}
};

template <> struct serializer<int64_t> {
  void operator()(const int64_t &v, container &c) {}
};

// unsigned int types
template <> struct serializer<uint8_t> {
  void operator()(const uint8_t &v, container &c) {}
};

template <> struct serializer<uint16_t> {
  void operator()(const uint16_t &v, container &c) {}
};

template <> struct serializer<uint32_t> {
  void operator()(const uint32_t &v, container &c) {}
};

template <> struct serializer<uint64_t> {
  void operator()(const uint64_t &v, container &c) {}
};

// floating types
template <> struct serializer<float> {
  void operator()(const float &v, container &c) {}
};

template <> struct serializer<double> {
  void operator()(const double &v, container &c) {}
};

// string types
template <> struct serializer<std::string> {
  void operator()(const std::string &v, container &c) {}
};

// container types
template <typename T> struct serializer<std::vector<T>> {
  void operator()(const std::vector<T> &v, container &c) {
    for (const auto &item : v) {
      serialize(item, c);
    }
  }
};

}  // namespace gene_internal
