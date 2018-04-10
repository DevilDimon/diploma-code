#include <iostream>
#include <windows.h>
#include "gene_basics.h"
#include "TestDriver\Public.h"



namespace gene_internal {

const DWORD OUTPUT_BUFSIZE = 64 * 1024;

bool exchange_messages_internal(const container &in, container *out) {
	HANDLE hDevice = CreateFile("\\\\.\\TEST",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		std::cerr << "Error: CreateFile failed: " << GetLastError() << std::endl;
		return false;
	}

	char OutputBuf[OUTPUT_BUFSIZE];
	DWORD bytesReturned = 0;

	BOOL bRc = DeviceIoControl(hDevice,
		IOCTL_TEST_METHOD_BUFFERED,
		reinterpret_cast<LPVOID>(const_cast<uint8_t *>(in.data())),
		in.size(),
		OutputBuf,
		OUTPUT_BUFSIZE,
		&bytesReturned,
		nullptr);

	if (!bRc) {
		std::cout << "Error in DeviceIoControl: " << GetLastError() << std::endl;
	}

	if (bytesReturned && out) {
		for (int i = 0; i < bytesReturned; ++i) {
			out->push_back(OutputBuf[i]);
		}
	}

	CloseHandle(hDevice);

	return bRc;
}

} // namespace gene_internal
