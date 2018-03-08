#include <iostream>
#include "test2.mojom.h"

using namespace my_module;

int main() {
  MyThirdStruct str{.smallVal = 16}, str1{};

  if (gene_internal::send_message(str) && gene_internal::receive_message(&str1)) {
    std::cout << str1.smallVal << std::endl;
  }

  return 0;
}
