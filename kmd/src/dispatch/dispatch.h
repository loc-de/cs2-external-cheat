#pragma once
#include <ntifs.h>
#include <ioctls.h>

namespace dispatch {
    NTSTATUS create(PDEVICE_OBJECT device, PIRP irp);
    NTSTATUS close(PDEVICE_OBJECT device, PIRP irp);
    NTSTATUS device_control(PDEVICE_OBJECT device, PIRP irp);
}