#include <iostream>
#include "test.mojom.h"

using namespace my_module;

int main() {
    MyStruct str = MyStruct{
          .myInt32 = 1488,
          .myStruct = MySecondStruct{
                  .myArrayOfArrays = std::vector<std::vector<std::string>>{
                          std::vector<std::string>{"hello", "world"},
                          std::vector<std::string>{"test", "transport", "rigorously"}
                  },
                  .mySecondStr = "villalobos",
                  .myThirdStruct = MyThirdStruct{
                          .arr = std::vector<std::string>{"lol", "kek", "cheburek"}
                  }
          }
    };

    MyStruct str1{};

  if (gene_internal::send_message(str) && gene_internal::receive_message(&str1)) {
      std::cout << "Success" << std::endl;
  }

  return 0;
}
