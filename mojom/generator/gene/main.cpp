#include <iostream>
#include <cstdlib>
#include <cstring>

#include <windows.h>
#include <strsafe.h>

#include "test2.mojom.h"
#include "test2.mojom.server.h"
#include "gene_runtime.h"

#include "TestDriver\Public.h"

using namespace my_module;

int main() {
	/*
    MyStruct str = {
		"hello worlc",
         0x1488,
		{
			"villalobos",
			{std::vector<std::string>{"lol", "kek", "cheburek"}},
			std::vector<std::vector<std::string>>{
				std::vector<std::string>{"hello", "world"},
				std::vector<std::string>{"test", "transport", "rigorously"}
			}
        }
    };

    MyStruct str1{};
	*/

	MyThirdStruct str = { 0x228 };
	MyThirdStruct str1{};

	TestClient c;
    if (c.Init(str)) {
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
