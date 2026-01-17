#include "dispatch.h"
#include "../memory/memory.h"

namespace dispatch {

    NTSTATUS create(PDEVICE_OBJECT, PIRP irp) {
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }

    NTSTATUS close(PDEVICE_OBJECT, PIRP irp) {
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }

    NTSTATUS device_control(PDEVICE_OBJECT, PIRP irp) {
        auto stack = IoGetCurrentIrpStackLocation(irp);
        auto req = reinterpret_cast<kmd::request*>(irp->AssociatedIrp.SystemBuffer);

        NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;

        if (!stack || !req) {
            IoCompleteRequest(irp, IO_NO_INCREMENT);
            return status;
        }

        switch ((kmd::ioctl)stack->Parameters.DeviceIoControl.IoControlCode) {
        case kmd::ioctl::attach:
            status = memory::attach(req->process_id);
            break;

        case kmd::ioctl::read:
            status = memory::read(req);
            break;

        case kmd::ioctl::write:
            status = memory::write(req);
            break;

        default:
            break;
        }

        irp->IoStatus.Status = status;
        irp->IoStatus.Information = sizeof(kmd::request);
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return status;
    }
}
