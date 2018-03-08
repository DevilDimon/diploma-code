#include <string>
#include <vector>

#include "gene_constraints.h"

namespace gene_internal {

template <typename T> static bool serialize_n_bits(int n, const T &v, container &c) {
    for (int i = 0; i < n; i++) {
        int pos_to_move = 8 * (n - 1 - i);
        uint8_t part = static_cast<uint8_t>((v & (0b11111111 << pos_to_move)) >> pos_to_move);
        c.push_back(part);
    }
    return true;
}

template <typename T> static bool deserialize_n_bits(int n, container &c, T *v) {
    if (c.size() < n) {
        return false;
    }
    T res = 0;
    for (int i = 0; i < n; i++) {
        res |= c[i] << 8 * (n - 1 - i);
    }
    *v = res;
    return true;
}

// int types
template <> struct serializer<int8_t> {
  bool operator()(const int8_t &v, container &c) {
      return serialize_n_bits(8, v, c);
  }
  bool operator()(container &c, int8_t *v) const {
      return deserialize_n_bits(8, c, v);
  }
};

template <> struct serializer<int16_t> {
  bool operator()(const int16_t &v, container &c) {
      return serialize_n_bits(16, v, c);
  }
  bool operator()(container &c, int16_t *v) const {
      return deserialize_n_bits(16, c, v);
  }
};

template <> struct serializer<int32_t> {
  bool operator()(const int32_t &v, container &c) {
      return serialize_n_bits(32, v, c);
  }
  bool operator()(container &c, int32_t *v) const {
      return deserialize_n_bits(32, c, v);
  }
};

template <> struct serializer<int64_t> {
  bool operator()(const int64_t &v, container &c) {
      return serialize_n_bits(64, v, c);
  }
  bool operator()(container &c, int64_t *v) const {
      return deserialize_n_bits(64, c, v);
  }
};

// unsigned int types
template <> struct serializer<uint8_t> {
  bool operator()(const uint8_t &v, container &c) {
      return serialize_n_bits(8, v, c);
  }
  bool operator()(container &c, uint8_t *v) const {
      return deserialize_n_bits(8, c, v);
  }
};

template <> struct serializer<uint16_t> {
  bool operator()(const uint16_t &v, container &c) {
      return serialize_n_bits(16, v, c);
  }
  bool operator()(container &c, uint16_t *v) const {
      return deserialize_n_bits(16, c, v);
  }
};

template <> struct serializer<uint32_t> {
  bool operator()(const uint32_t &v, container &c) {
      return serialize_n_bits(32, v, c);
  }
  bool operator()(container &c, uint32_t *v) const {
      return deserialize_n_bits(32, c, v);
  }
};

template <> struct serializer<uint64_t> {
  bool operator()(const uint64_t &v, container &c) {
      return serialize_n_bits(64, v, c);
  }
  bool operator()(container &c, uint64_t *v) const {
      return deserialize_n_bits(64, c, v);
  }
};

// floating types
template <> struct serializer<float> {
  bool operator()(const float &v, container &c) {
      float temp = v;
      uint8_t *arr = reinterpret_cast<uint8_t *>(&temp);
      for (int i = 0; i < sizeof(temp); ++i) {
          c.push_back(arr[i]);
      }
      return true;
  }
  bool operator()(container &c, float *v) const {
      uint8_t arr[sizeof(float)];
      for (int i = 0; i < sizeof(float); ++i) {
          arr[i] = c[i];
      }
      float *res = reinterpret_cast<float *>(arr);
      *v = *res;
      return true;
  }
};

template <> struct serializer<double> {
  bool operator()(const double &v, container &c) {
      double temp = v;
      uint8_t *arr = reinterpret_cast<uint8_t *>(&temp);
      for (int i = 0; i < sizeof(temp); ++i) {
          c.push_back(arr[i]);
      }
      return true;
  }
  bool operator()(container &c, double *v) const {
      uint8_t arr[sizeof(double)];
      for (int i = 0; i < sizeof(double); ++i) {
          arr[i] = c[i];
      }
      double *res = reinterpret_cast<double *>(arr);
      *v = *res;
      return true;
  }
};

// string types
template <> struct serializer<std::string> {
  bool operator()(const std::string &v, container &c) { return true; }
  bool operator()(container &c, std::string *v) const { return true; }
};

// container types
template <typename T> struct serializer<std::vector<T>> {
  bool operator()(const std::vector<T> &v, container &c) {
    for (const auto &item : v) {
      serialize(item, c);
    }

    return true;
  }
  bool operator()(container &c, std::vector<T> *v) const { return true; }
};

}  // namespace gene_internal
