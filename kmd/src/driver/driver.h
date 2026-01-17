#pragma once
#include <ntifs.h>

namespace driver {
    NTSTATUS initialize(PDRIVER_OBJECT driver_object);
}