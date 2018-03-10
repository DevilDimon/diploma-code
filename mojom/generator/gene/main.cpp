#include <iostream>
#include "test2.mojom.h"

using namespace my_module;

int main() {
  MyThirdStruct str{.arr = std::vector<std::string>({"hello", "world", "1"})}, str1{};

  if (gene_internal::send_message(str) && gene_internal::receive_message(&str1)) {
      for (const auto &item : str1.arr) {
          std::cout << item << std::endl;
      }
  }

  return 0;
}
