#include <iostream>
#include "test.mojom.h"

using namespace my_module;

int main() {
  MyStruct str, str1;
  gene_internal::container c;

  std::cout << gene_internal::serialize(str, c) << std::endl;
  std::cout << gene_internal::deserialize(c, &str1) << std::endl;

  return 0;
}
