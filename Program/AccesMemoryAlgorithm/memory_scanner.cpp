#include <Windows.h>
#include <Tlhelp32.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

class MemoryScanner {
public:
    MemoryScanner(void* processHandle) : processHandle(processHandle) {}

    bool scanMemoryForValue(int value, std::vector<uintptr_t>& foundAddresses) {
        foundAddresses.clear();

        uintptr_t memoryBase = 0;
        MEMORY_BASIC_INFORMATION memInfo;

        while (VirtualQueryEx(processHandle, (LPCVOID)memoryBase, &memInfo, sizeof(memInfo))) {
            DWORD memorySize = memInfo.RegionSize;

            if (memorySize == 0) {
                break;
            }

            // Allocate memory to store the region content
            std::vector<char> buffer(memorySize);

            // Read the memory region
            SIZE_T bytesRead;
            if (ReadProcessMemory(processHandle, memInfo.BaseAddress, buffer.data(), memorySize, &bytesRead) && bytesRead == memorySize) {
                // Search for the specified integer value in the buffer
                for (size_t i = 0; i < memorySize - sizeof(int); i += sizeof(int)) {
                    if (*(int*)(buffer.data() + i) == value) {
                        foundAddresses.push_back((uintptr_t)memInfo.BaseAddress + i);
                    }
                }
            }

            memoryBase = (uintptr_t)memInfo.BaseAddress + memorySize;
        }

        return !foundAddresses.empty();
    }

private:
    void* processHandle;
};

// Forward declaration
HANDLE getProcessHandleByName(const std::string& processName);

int main() {
    const std::string processName = "Daslight4.exe";

    // Get the process handle
    HANDLE hProcess = getProcessHandleByName(processName);

    if (hProcess != NULL) {
        // Create a MemoryScanner object
        MemoryScanner scanner(hProcess);

        // Search for the first integer value
        int firstSearchValue = 246;
        std::vector<uintptr_t> firstSearchAddresses;

        if (!scanner.scanMemoryForValue(firstSearchValue, firstSearchAddresses)) {
            std::cout << "First value not found in process memory" << std::endl;
            CloseHandle(hProcess);
            return 1;
        }
        for (uintptr_t address : firstSearchAddresses) {
            std::cout << "  " << address << std::endl;
        }

        CloseHandle(hProcess);
    } else {
        // The process could not be found or opened
        std::cout << "Could not find or open the process" << std::endl;
        return 1;
    }

    return 0;
}

// Implementation of getProcessHandleByName goes here
HANDLE getProcessHandleByName(const std::string& processName) {
    DWORD processId = 0;
    HANDLE hProcess = NULL;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (processName.compare(pe32.szExeFile) == 0) {
                processId = pe32.th32ProcessID;
                hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return hProcess;
}