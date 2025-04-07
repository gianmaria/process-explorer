#define NOMINMAX 1
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#include <iostream>
#include <string>
#include <vector>
#include <format>
#include <algorithm>

using std::cerr;
using std::cout;
using std::endl;
using std::format;
using std::string;
using std::vector;

vector<DWORD> enum_all_pids()
{
    constexpr DWORD PIDS_COUNT = 1024;
    auto pids = vector<DWORD>(PIDS_COUNT, 0);

    DWORD bytes_needed = 0;
    constexpr DWORD PIDS_SIZE = PIDS_COUNT * sizeof(DWORD);

    BOOL res = EnumProcesses(
        pids.data(),  // [out] DWORD   *lpidProcess,
        PIDS_SIZE,    // [in]  DWORD   cb,
        &bytes_needed // [out] LPDWORD lpcbNeeded
    );

    if (res == 0)
    {
        cerr << format("ERROR: EnumProcesses failed ({})", GetLastError()) << endl;
        return {};
    }

    auto pids_enumerated = bytes_needed / sizeof(DWORD);
    if (pids_enumerated == PIDS_COUNT)
    {
        cerr << format("ERROR: Too much processes for array capacity") << endl;
        return {};
    }

    return pids;
}

string collect_process_info(DWORD pid)
{
    HANDLE handle = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, // [in] DWORD dwDesiredAccess,
        FALSE,                                       // [in] BOOL  bInheritHandle,
        pid                                          // [in] DWORD dwProcessId
    );

    if (handle == NULL) return {};

    DWORD buffer_size = 1024;
    auto buffer = string(buffer_size, 0);

    DWORD size_needed = buffer_size;
    BOOL res = QueryFullProcessImageNameA(
        handle,        // [in]      HANDLE hProcess,
        0,             // [in]      DWORD  dwFlags,
        buffer.data(), // [out]     LPWSTR lpExeName,
        &size_needed   // [in, out] PDWORD lpdwSize
    );

    if (res == 0 or
        size_needed >= buffer_size)
    {
        cerr << format("ERROR: QueryFullProcessImageNameW failes ({})", GetLastError()) << endl;
        CloseHandle(handle);
        return {};
    }

    CloseHandle(handle);
    return buffer;
}

int main()
{
    auto pids = enum_all_pids();

    if (pids.size() == 0)
        return -1;

    // std::sort(pids.begin(), pids.end());

    for (auto &pid : pids)
    {
        if (pid == 0 or pid == 4)
            continue;

        auto info = collect_process_info(pid);

        cout << format("pid: {:<5} - {}", pid, info) << endl;

    }

    return 0;
}
