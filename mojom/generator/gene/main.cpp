#include <iostream>

#include "test.mojom.h"
#include "test.mojom.client.h"
#include "gene_runtime.h"

using namespace my_module;

int main() {
	
    MyStruct str = {
		"hello worlc",
         0x1488,
		{
			"oasis",
			{ 5553535 },
			std::vector<std::vector<std::string>>{
				std::vector<std::string>{"definitely", "maybe"},
				std::vector<std::string>{"what's", "the", "story", "morning", "glory"}
			}
        }
    };

	MySecondStruct str2 = {
		"kernel test",
		{ 0x228 },
		std::vector<std::vector<std::string>>{
			std::vector<std::string>{"be", "here", "now"},
			std::vector<std::string>{"standing", "on", "the", "shoulder", "of", "giants"}
		}
	};


	CoreClient c;
	auto res = c.Init(str, str2);
	if (res) {
		std::cout << "Success sending msg" << std::endl;
		std::cout << *res << std::endl;
	} else {
		std::cerr << "Could not send msg to driver" << std::endl;
		return 1;
	}

  return 0;
}
