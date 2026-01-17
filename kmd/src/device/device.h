#pragma once
#include <ntifs.h>

namespace device {
    NTSTATUS create(PDRIVER_OBJECT driver_object);
    void destroy(PDRIVER_OBJECT driver_object);
}