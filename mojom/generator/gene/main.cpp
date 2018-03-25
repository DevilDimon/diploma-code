#include <iostream>
#include <cstdlib>
#include <cstring>

#include <windows.h>
#include <strsafe.h>

#include "test.mojom.h"
#include "test.mojom.server.h"
#include "gene_runtime.h"

#include "TestDriver\Public.h"

using namespace my_module;

int main() {
	
    MyStruct str = {
		"hello worlc",
         0x1488,
		{
			"villalobos",
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
    if (c.Init(str, str2)) {
        std::cout << "Success sending msg" << std::endl;
    }
	else {
		std::cerr << "Could not send msg to driver" << std::endl;
		return 1;
	}

	/*

    GeneRuntime r{};
    r.RegisterCoreHandler(new CoreServer());
    gene_internal::container container;
    gene_internal::receive_message_internal(&container);
    if (r.ProcessIncomingMessage(container)) {
        std::cout << "Success receiving and processing msg" << std::endl;
    }
	*/


  return 0;
}
