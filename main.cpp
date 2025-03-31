#define NOMINMAX 1
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>


#include <iostream>
#include <string>
#include <vector>
#include <format>


using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::format;

int main()
{
    constexpr DWORD PIDS_COUNT = 1024;
    constexpr DWORD PIDS_SIZE = PIDS_COUNT * sizeof(DWORD);
    DWORD pids[PIDS_COUNT] {};

    DWORD bytes_needed = 0;

    BOOL res = EnumProcesses(
        pids, // [out] DWORD   *lpidProcess,
        PIDS_SIZE, // [in]  DWORD   cb,
        &bytes_needed // [out] LPDWORD lpcbNeeded
    );

    auto pids_enumerated = bytes_needed / sizeof(DWORD);

    if (res == 0)
    {
        cerr << format("ERROR: EnumProcesses failed ({})", GetLastError()) << endl;
        return 1;
    }

    if (pids_enumerated == PIDS_COUNT)
    {
        cerr << format("ERROR: Too much processes for array capacity") << endl;
        return -1;
    }

    for (DWORD i = 0;
         i < pids_enumerated;
         ++i)
    {
        auto& pid = pids[i];
        if (pid == 0 or pid == 4) continue;

        HANDLE handle = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, // [in] DWORD dwDesiredAccess,
            FALSE, // [in] BOOL  bInheritHandle,
            pid    // [in] DWORD dwProcessId
        );

        if (handle == NULL)
        {
            cout << format("pid: {} -", pid) << endl;
            continue;
        }

        constexpr DWORD buffer_count = 1024;
        char buffer[buffer_count] = {};

        DWORD size = buffer_count;
        res = QueryFullProcessImageNameA(
            handle, // [in]      HANDLE hProcess,
            0,      // [in]      DWORD  dwFlags,
            buffer, // [out]     LPWSTR lpExeName,
            &size   // [in, out] PDWORD lpdwSize
        );

        if (res == 0)
        {
            cerr << format("ERROR: QueryFullProcessImageNameW failes ({})", GetLastError()) << endl;
            return 1;
        }

        cout << format("pid: {} - {}", pid, buffer) << endl;

        CloseHandle(handle);
    }



    return 0;
}
