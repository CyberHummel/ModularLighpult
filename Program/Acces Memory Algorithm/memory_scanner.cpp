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

void printValueEvery100Milliseconds(HANDLE processHandle, uintptr_t address) {
    while (true) {
        int value;
        if (ReadProcessMemory(processHandle, (LPVOID)address, &value, sizeof(value), NULL)) {
            std::cout << "Value at address " << address << ": " << value << std::endl;
        }

        // Sleep for 100 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    const std::string processName = "Daslight4.exe";

    // Get the process handle
    HANDLE hProcess = getProcessHandleByName(processName);

    if (hProcess != NULL) {
        // Create a MemoryScanner object
        MemoryScanner scanner(hProcess);

        // Search for the first integer value
        int firstSearchValue = 42;
        std::vector<uintptr_t> firstSearchAddresses;

        // Measure the time to find the first list of addresses
        auto start = std::chrono::high_resolution_clock::now();
        if (!scanner.scanMemoryForValue(firstSearchValue, firstSearchAddresses)) {
            std::cout << "First value not found in process memory" << std::endl;
            CloseHandle(hProcess);
            return 1;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        // Print the time taken to find the first list of addresses
        std::cout << "Time taken to find the first list of addresses: " << duration.count() << " seconds" << std::endl;

        // Pause for 5 seconds
        std::cout << "Pausing for 5 seconds..." << std::endl;
        Sleep(5000);

        // Search for the second integer value in the addresses from the first search
        int secondSearchValue = 82;
        std::vector<uintptr_t> secondSearchAddresses;
        for (uintptr_t address : firstSearchAddresses) {
            // Read the value at the current address
            int value;
            if (ReadProcessMemory(hProcess, (LPVOID)address, &value, sizeof(value), NULL)) {
                if (value == secondSearchValue) {
                    secondSearchAddresses.push_back(address);
                }
            }
        }

        if (secondSearchAddresses.empty()) {
            std::cout << "Second value not found in process memory after the pause" << std::endl;
            CloseHandle(hProcess);
            return 1;
        }

        std::cout << "Found " << secondSearchAddresses.size() << " occurrences of " << secondSearchValue << " in process memory after the pause:" << std::endl;
        for (uintptr_t address : secondSearchAddresses) {
            std::cout << "  " << address << std::endl;

            // Start a separate thread to print the value every 100 milliseconds
            std::thread(printValueEvery100Milliseconds, hProcess, address).detach();
        }

        // Keep the main thread running to wait for the value updates
        std::this_thread::sleep_for(std::chrono::seconds(30));

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