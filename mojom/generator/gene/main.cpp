#include <iostream>
#include <cstdlib>
#include <cstring>

#include <windows.h>
#include <strsafe.h>

#include "test.mojom.h"
#include "test.mojom.server.h"
#include "gene_runtime.h"

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

    CoreClient c;
    if (c.Init(str, {
			"jorje",
			{ std::vector<std::string>{"1", "2", "3"} },
			std::vector<std::vector<std::string>>{
				std::vector<std::string>{"1"},
				std::vector<std::string>{"2"}
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
	*/

	HANDLE hDevice = CreateFile("\\\\.\\TEST",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		std::cout << "Error: CreateFile Failed : " << GetLastError() << std::endl;
		return 7;
	}

	char OutputBuffer[100];
	char InputBuffer[100];

	if (FAILED(StringCchCopy(InputBuffer, sizeof(InputBuffer),
		"this String is from User Application; using METHOD_BUFFERED"))) {
		return 7;
	}

	std::cout << "Calling DeviceIoControl..." << std::endl;
	memset(OutputBuffer, 0, sizeof(OutputBuffer));

	ULONG bytesReturned;

	BOOL bRc = DeviceIoControl(hDevice,
		(DWORD)CTL_CODE(40001, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS),
		InputBuffer,
		(DWORD)strlen(InputBuffer) + 1,
		OutputBuffer,
		sizeof(OutputBuffer),
		&bytesReturned,
		NULL);

	if (!bRc)
	{
		std::cout << "Error in DeviceIoControl : " << GetLastError() << std::endl;
		return 7;

	}
	std::cout << "    OutBuffer (" << bytesReturned << "): " << OutputBuffer << std::endl;

	CloseHandle(hDevice);


  return 0;
}
