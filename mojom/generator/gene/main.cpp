#include <iostream>
#include "test2.mojom.h"

using namespace my_module;

int main() {
  MyThirdStruct str{.floatVal = 22.0f}, str1{};

  if (gene_internal::send_message(str) && gene_internal::receive_message(&str1)) {
    std::cout << str1.floatVal << std::endl;
  }

  return 0;
}
