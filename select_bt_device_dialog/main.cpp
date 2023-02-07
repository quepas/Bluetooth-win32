#include <Windows.h>
#include <bluetoothapis.h>
#include <iostream>


int main() {
    BLUETOOTH_SELECT_DEVICE_PARAMS select_params;
    select_params.dwSize = sizeof(select_params);
    // Search all classes of devices
    select_params.cNumOfClasses = 0;
    select_params.fShowUnknown = TRUE;
    select_params.fAddNewDeviceWizard = TRUE;
    wchar_t text[] = L"Select your BT device. This text will be shown in the Selection Window. Or will it?";
    select_params.pszInfo = text;
    // Don't attach the selection window to any other
    select_params.hwndParent = nullptr;

    BOOL selection_result = BluetoothSelectDevices(&select_params);
    if (selection_result) {
        BLUETOOTH_DEVICE_INFO *device_info = select_params.pDevices;
        for (ULONG cDevice = 0; cDevice < select_params.cNumDevices; cDevice++) {
            std::wcout << "Name" << device_info->szName << std::endl;
            if (device_info->fAuthenticated || device_info->fRemembered) {}
            device_info = (BLUETOOTH_DEVICE_INFO *) ((LPBYTE) device_info + device_info->dwSize);
        }

        BluetoothSelectDevicesFree(&select_params);
    } else {
        std::cout << "ID of the last ERROR: " << GetLastError() << std::endl;
    }
}
