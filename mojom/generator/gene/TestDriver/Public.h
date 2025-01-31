#pragma once

//
// Device type           -- in the "User Defined" range."
//
#define FILEIO_TYPE 40001
//
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
//

#define IOCTL_TEST_METHOD_BUFFERED \
    CTL_CODE( FILEIO_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS  )

#define NTDEVICE_NAME_STRING      L"\\Device\\TEST"
#define SYMBOLIC_NAME_STRING     L"\\DosDevices\\TEST"