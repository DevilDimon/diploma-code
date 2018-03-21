#pragma once
#include <iostream>
#include <windows.h>
#include "gene_basics.h"
#include "TestDriver\Public.h"

namespace gene_internal {

bool send_message_internal(const container &c) {
	HANDLE hDevice = CreateFile("\\\\.\\TEST",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Error: CreateFile failed: " << GetLastError() << std::endl;
		return false;
	}

	BOOL bRc = DeviceIoControl(hDevice,
		IOCTL_TEST_METHOD_BUFFERED,
		reinterpret_cast<LPVOID>(const_cast<uint8_t *>(c.data())),
		c.size(),
		nullptr,
		0,
		nullptr,
		nullptr);

	if (!bRc)
	{
		std::cout << "Error in DeviceIoControl: " << GetLastError() << std::endl;
	}

	CloseHandle(hDevice);

	return bRc;
}

bool receive_message_internal(container *c) {
	// TODO: user-mode reception
    return true;
}

} // namespace gene_internal
