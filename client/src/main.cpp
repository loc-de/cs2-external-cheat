#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <ioctls.h>

bool service_exists(const wchar_t* name) {
    SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm) return false;

    SC_HANDLE svc = OpenServiceW(scm, name, SERVICE_QUERY_STATUS);
    bool exists = (svc != nullptr);

    if (svc) CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return exists;
}

bool create_service(const wchar_t* name, const wchar_t* path) {
    SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!scm) return false;

    SC_HANDLE svc = CreateServiceW(
        scm,
        name,
        name,
        SERVICE_START | DELETE | SERVICE_STOP,
        SERVICE_KERNEL_DRIVER,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_IGNORE,
        path,
        nullptr, nullptr, nullptr, nullptr, nullptr
    );

    if (!svc && GetLastError() != ERROR_SERVICE_EXISTS) {
        CloseServiceHandle(scm);
        return false;
    }

    if (svc) CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return true;
}

bool start_driver(const wchar_t* name) {
    SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm) return false;

    SC_HANDLE svc = OpenServiceW(scm, name, SERVICE_START);
    if (!svc) {
        CloseServiceHandle(scm);
        return false;
    }

    bool ok = StartServiceW(svc, 0, nullptr) || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;

    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return ok;
}


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

    const wchar_t* name = L"kmd";
    const wchar_t* path = L"D:\\Projects\\cs2-external-cheat\\build\\kmd.sys";

    if (!service_exists(name)) {
        create_service(name, path);
    }

    std::cout << std::boolalpha << start_driver(name) << std::endl;

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
