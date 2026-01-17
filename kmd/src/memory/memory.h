#pragma once
#include <ntifs.h>
#include <ioctls.h>

extern "C" {
	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(
		PEPROCESS SourceProcess,
		PVOID SourceAddress,
		PEPROCESS TargetProcess,
		PVOID TargetAddress,
		SIZE_T BufferSize,
		KPROCESSOR_MODE PreviousMode,
		PSIZE_T ReturnSize
	);
}

namespace memory {
    NTSTATUS attach(HANDLE pid);
    NTSTATUS read(kmd::request* req);
    NTSTATUS write(kmd::request* req);
}