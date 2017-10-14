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

}  // namespace gene_internal
