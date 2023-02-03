#include <Windows.h>
#include <bluetoothapis.h>
#include <iostream>
#include <format>
#include <vector>

bool enumerate_bt_radios(std::vector<BLUETOOTH_RADIO_INFO>& radio_info_collection) {
    BLUETOOTH_FIND_RADIO_PARAMS find_radio_params = {sizeof(find_radio_params)};
    HANDLE radio_handle = nullptr;
    DWORD last_error = 0;
    HBLUETOOTH_RADIO_FIND finding_handle = BluetoothFindFirstRadio(&find_radio_params, &radio_handle);
    if (finding_handle) {
        do {
            BLUETOOTH_RADIO_INFO radio_info = {0};
            radio_info.dwSize = sizeof(BLUETOOTH_RADIO_INFO);
            BluetoothGetRadioInfo(radio_handle, &radio_info);
            radio_info_collection.push_back(radio_info);
            CloseHandle(radio_handle);
        } while (BluetoothFindNextRadio(&find_radio_params, &radio_handle));
        // Check if all items were correctly returned
        last_error = GetLastError();
    }
    BluetoothFindRadioClose(finding_handle);
    return last_error == ERROR_NO_MORE_ITEMS;
}

int main() {
    std::vector<BLUETOOTH_RADIO_INFO> radios;
    if (!enumerate_bt_radios(radios)) {
        std::cout << "Couldn't enumerate all existing BT radios" << std::endl;
    }

    for (int idx = 0; idx < radios.size(); ++idx) {
        std::cout << "--- Displaying information about radio #" << idx + 1 << " ---" << std::endl;
        auto info = radios[idx];
        std::wcout << "\tRadio name: " << info.szName << std::endl;
        // Check company ID in Section 7.1. https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned%20Numbers.pdf
        // E.g. 0x0002 means Intel
        std::cout << "\tManufacturer ID: " << std::format("{:#06x}", info.manufacturer) << std::endl;
        std::cout << "\tManufacturer specifics: " << info.lmpSubversion << std::endl;
        std::cout << "\tLocal radio device class: " << info.ulClassofDevice << std::endl;
    }

}
