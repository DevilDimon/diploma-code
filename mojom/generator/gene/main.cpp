#include <iostream>
#include "test.mojom.h"
#include "test.mojom.server.h"
#include "gene_runtime.h"

using namespace my_module;

int main() {
    MyStruct str = MyStruct{
          .myInt32 = 0x1488,
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

    CoreClient c;
    if (c.Init(str, MySecondStruct{
            .myArrayOfArrays = std::vector<std::vector<std::string>>{
                    std::vector<std::string>{"1"},
                    std::vector<std::string>{"2"}
            },
            .mySecondStr = "jorje",
            .myThirdStruct = MyThirdStruct{
                    .arr = std::vector<std::string>{"1", "2", "3"}
            }
    })) {
        std::cout << "Success sending msg" << std::endl;
    }

    GeneRuntime r{};
    r.RegisterCoreHandler(new CoreServer());
    gene_internal::container container;
    gene_internal::receive_message_internal(&container);
    if (r.ProcessIncomingMessage(container)) {
        std::cout << "Success receiving and processing msg" << std::endl;
    }


  return 0;
}
