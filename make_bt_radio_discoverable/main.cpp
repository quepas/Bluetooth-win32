#include <Windows.h>
#include <bluetoothapis.h>
#include <chrono>
#include <iostream>
#include <format>
#include <vector>
#include <thread>

using namespace std::chrono_literals;

bool list_bt_radios(std::vector<HANDLE>& radio_handles) {
    BLUETOOTH_FIND_RADIO_PARAMS find_radio_params = {sizeof(find_radio_params)};
    HANDLE radio_handle = nullptr;
    DWORD last_error = 0;
    HBLUETOOTH_RADIO_FIND finding_handle = BluetoothFindFirstRadio(&find_radio_params, &radio_handle);
    if (finding_handle) {
        do {
            radio_handles.push_back(radio_handle);
        } while (BluetoothFindNextRadio(&find_radio_params, &radio_handle));
        // Check if all items were correctly returned
        last_error = GetLastError();
    }
    BluetoothFindRadioClose(finding_handle);
    return last_error == ERROR_NO_MORE_ITEMS;
}

int main() {
    std::vector<HANDLE> radio_handles;
    if (!list_bt_radios(radio_handles)) {
        std::cout << "Couldn't list all existing BT radios" << std::endl;
    }

    for (int idx = 0; idx < radio_handles.size(); ++idx) {
        std::cout << "--- Displaying information about radio #" << idx + 1 << " ---" << std::endl;
        auto radio_handle = radio_handles[idx];
        BLUETOOTH_RADIO_INFO radio_info = {0};
        radio_info.dwSize = sizeof(BLUETOOTH_RADIO_INFO);
        BluetoothGetRadioInfo(radio_handle, &radio_info);
        std::wcout << "\tRadio name: " << radio_info.szName << std::endl;
        std::cout << "\tIs connectable?: " << BluetoothIsConnectable(radio_handle) << std::endl;
        std::cout << "\tIs discoverable?: " << BluetoothIsDiscoverable(radio_handle) << std::endl;
        std::cout << "\tEnable incoming connections: " << BluetoothEnableIncomingConnections(radio_handle, TRUE) << std::endl;
        std::cout << "\tEnable discovery" << BluetoothEnableDiscovery(radio_handle, TRUE) << std::endl;
        // Checking again what has changed
        std::cout << "\tIs connectable?: " << BluetoothIsConnectable(radio_handle) << std::endl;
        std::cout << "\tIs discoverable?: " << BluetoothIsDiscoverable(radio_handle) << std::endl;
        std::thread t1([]() {
            auto sleep_time = 30s;
            std::cout << "Allowing for the BT radio to be found by sleeping for: " << sleep_time << std::endl;;
            std::this_thread::sleep_for(sleep_time);
        });
        t1.join();
        CloseHandle(radio_handle);
    }
    radio_handles.clear();
}
