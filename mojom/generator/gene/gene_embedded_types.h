#include <string>
#include <vector>

#include "gene_constraints.h"

namespace gene_internal {

// int types
template <> struct serializer<int8_t> {
  bool operator()(const int8_t &v, container &c) { return true; }
  bool operator()(const container &c, int8_t *v) const { return true; }
};

template <> struct serializer<int16_t> {
  bool operator()(const int16_t &v, container &c) { return true; }
  bool operator()(const container &c, int16_t *v) const { return true; }
};

template <> struct serializer<int32_t> {
  bool operator()(const int32_t &v, container &c) { return true; }
  bool operator()(const container &c, int32_t *v) const { return true; }
};

template <> struct serializer<int64_t> {
  bool operator()(const int64_t &v, container &c) { return true; }
  bool operator()(const container &c, int64_t *v) const { return true; }
};

// unsigned int types
template <> struct serializer<uint8_t> {
  bool operator()(const uint8_t &v, container &c) { return true; }
  bool operator()(const container &c, uint8_t *v) const { return true; }
};

template <> struct serializer<uint16_t> {
  bool operator()(const uint16_t &v, container &c) { return true; }
  bool operator()(const container &c, uint16_t *v) const { return true; }
};

template <> struct serializer<uint32_t> {
  bool operator()(const uint32_t &v, container &c) { return true; }
  bool operator()(const container &c, uint32_t *v) const { return true; }
};

template <> struct serializer<uint64_t> {
  bool operator()(const uint64_t &v, container &c) { return true; }
  bool operator()(const container &c, uint64_t *v) const { return true; }
};

// floating types
template <> struct serializer<float> {
  bool operator()(const float &v, container &c) { return true; }
  bool operator()(const container &c, float *v) const { return true; }
};

template <> struct serializer<double> {
  bool operator()(const double &v, container &c) { return true; }
  bool operator()(const container &c, double *v) const { return true; }
};

// string types
template <> struct serializer<std::string> {
  bool operator()(const std::string &v, container &c) { return true; }
  bool operator()(const container &c, std::string *v) const { return true; }
};

// container types
template <typename T> struct serializer<std::vector<T>> {
  bool operator()(const std::vector<T> &v, container &c) {
    for (const auto &item : v) {
      serialize(item, c);
    }

    return true;
  }
  bool operator()(const container &c, std::vector<T> *v) const { return true; }
};

}  // namespace gene_internal
