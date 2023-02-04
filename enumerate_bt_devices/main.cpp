#include <Windows.h>
#include <bluetoothapis.h>
#include <iostream>
#include <format>
#include <vector>

std::wstring format_datetime(const SYSTEMTIME* system_time) {
    const int max_length = 50;
    auto date = new WCHAR[max_length];
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, system_time, nullptr, date, max_length, nullptr);
    auto time = new WCHAR[max_length];
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, system_time, nullptr, time, max_length);
    return std::wstring(date) + L" at " + std::wstring(time);
}

bool enumerate_bt_devices(std::vector<BLUETOOTH_DEVICE_INFO>& device_collection) {
    BLUETOOTH_DEVICE_SEARCH_PARAMS search_params;
    search_params.dwSize = sizeof(search_params);
    // Look for any device!
    search_params.fReturnConnected = TRUE;
    search_params.fReturnAuthenticated = TRUE;
    search_params.fReturnRemembered = TRUE;
    search_params.fReturnUnknown = TRUE;
    search_params.fIssueInquiry = TRUE;
    // Device scanning time in seconds
    search_params.cTimeoutMultiplier = 5;
    BLUETOOTH_DEVICE_INFO device_info;
    device_info.dwSize = sizeof(device_info);

    HBLUETOOTH_DEVICE_FIND finding_handle = BluetoothFindFirstDevice(&search_params, &device_info);
    if (finding_handle) {
        do {
            device_collection.push_back(device_info);
        } while(BluetoothFindNextDevice(finding_handle, &device_info));
    }
    auto last_error = GetLastError();
    BluetoothFindDeviceClose(finding_handle);
    return last_error == 0 || last_error == ERROR_NO_MORE_ITEMS;
}


int main() {
    std::vector<BLUETOOTH_DEVICE_INFO> devices;
    if (!enumerate_bt_devices(devices)) {
        std::cout << "Couldn't correctly enumerate BT devices" << std::endl;
    }

    for (int idx = 0; idx < devices.size(); ++idx) {
        std::cout << "--- Displaying information about device #" << idx + 1 << " ---" << std::endl;
        auto device_info = devices[idx];
        std::wcout << "\tDevice name     : " << device_info.szName << std::endl;
        // See Section 2.8 Class of Device in "Assigned Numbers" document from BT spec
        // https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned%20Numbers.pdf
        std::cout  << "\tClass           : " << device_info.ulClassofDevice << std::endl;
        std::cout  << "\tIs connected    : " << device_info.fConnected << std::endl;
        std::cout  << "\tIs remembered   : " << device_info.fRemembered << std::endl;
        std::cout  << "\tIs authenticated: " << device_info.fAuthenticated << std::endl;
        std::wcout << "\tLast seen       : " << format_datetime(&device_info.stLastSeen) << std::endl;
        // If you see this weird date: 01/01/1601 at 00:00:00, it is because win32 uses ANSI date format,
        // and the device has never been used before, hence date 0.
        // "This structure is a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601."
        // More info: https://stackoverflow.com/questions/10849717/what-is-the-significance-of-january-1-1601
        //            https://learn.microsoft.com/en-us/previous-versions/aa915351(v=msdn.10)
        std::wcout << "\tLast used       : " << format_datetime(&device_info.stLastUsed) << std::endl;
    }

}
