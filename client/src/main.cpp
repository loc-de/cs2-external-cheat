#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <ioctls.h>

static DWORD get_process_id(const wchar_t* process_name) {
    DWORD process_id = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (snapshot == INVALID_HANDLE_VALUE)
        return process_id;

    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(decltype(entry));

    if (Process32FirstW(snapshot, &entry) == TRUE) {
        if (_wcsicmp(process_name, entry.szExeFile) == 0)
            process_id = entry.th32ProcessID;
        else {
            while (Process32NextW(snapshot, &entry) == TRUE) {
                if (_wcsicmp(process_name, entry.szExeFile) == 0) {
                    process_id = entry.th32ProcessID;
                    break;
                }
            }
        }
    }

    CloseHandle(snapshot);

    return process_id;
}

bool attach_to_process(HANDLE driver, DWORD pid) {
    kmd::request req{};
    req.process_id = reinterpret_cast<HANDLE>(pid);

    return DeviceIoControl(
        driver,
        kmd::ioctl::attach,
        &req,
        sizeof(req),
        &req,
        sizeof(req),
        nullptr,
        nullptr
    );
}

int main() {
    std::wcout << L"[+] starting test\n";

    DWORD pid = get_process_id(L"notepad.exe");
    if (!pid) {
        std::cout << "[-] notepad.exe not found\n";
        return 1;
    }

    std::cout << "[+] notepad pid: " << pid << "\n";

    HANDLE driver = CreateFileW(
        KMD_DEVICE_NAME,
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (driver == INVALID_HANDLE_VALUE) {
        std::cout << "[-] failed to open driver: " << GetLastError() << "\n";
        std::cin.get();
        return 1;
    }

    std::cout << "[+] driver handle OK\n";

    if (!attach_to_process(driver, pid)) {
        std::cout << "[-] attach failed\n";
        CloseHandle(driver);
        std::cin.get();
        return 1;
    }

    CloseHandle(driver);

    std::cin.get();

    return 0;
}
