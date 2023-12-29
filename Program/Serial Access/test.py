import ctypes
import time
import threading
import psutil

def get_process_handle_by_name(process_name):
    for process in psutil.process_iter(['pid', 'name']):
        if process.info['name'] == process_name:
            return process.pid

    return None

def print_value_every_100_milliseconds(h_process, address):
    while True:
        try:
            value = ctypes.c_int.from_address(address)
            print(f"Address {hex(address)}: {value.value}")
            time.sleep(0.1)
        except ValueError:
            break

def main():
    process_name = "Daslight4.exe"

    # Get the process handle
    h_process = get_process_handle_by_name(process_name)

    if h_process:
        # Create a MemoryScanner object
        scanner = MemoryScanner(h_process)

        # Search for the first integer value
        first_search_value = 42
        first_search_addresses = []

        # Measure the time to find the first list of addresses
        start_time = time.time()
        if not scanner.scan_memory_for_value(first_search_value, first_search_addresses):
            print("First value not found in process memory")
            return 1
        end_time = time.time()
        time_taken = end_time - start_time

        # Print the time taken to find the first list of addresses
        print(f"Time taken to find the first list of addresses: {time_taken} seconds")

        # Pause for 5 seconds
        print("Pausing for 5 seconds...")
        time.sleep(5)

        # Search for the second integer value in the addresses from the first search
        second_search_value = 82
        second_search_addresses = []
        for address in first_search_addresses:
            # Read the value at the current address
            value = ctypes.c_int.from_address(address)
            if value.value == second_search_value:
                second_search_addresses.append(address)

        if not second_search_addresses:
            print("Second value not found in process memory after the pause")
            return 1

        print(f"Found {len(second_search_addresses)} occurrences of {second_search_value} in process memory after the pause:")
        for address in second_search_addresses:
            print(f" {hex(address)}")

            # Start a separate thread to print the value every 100 milliseconds
            threading.Thread(target=print_value_every_100_milliseconds, args=(h_process, address)).start()

        # Keep the main thread running to wait for the value updates
        time.sleep()

if __name__ == '__main__':
    main()