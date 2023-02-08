#include <Windows.h>
#include <bluetoothapis.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>

using namespace std::chrono_literals;


BOOL PfnAuthenticationCallbackEx(LPVOID pvParam, PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pAuthCallbackParams) {
    std::cout << "--- Handling authentication of an incoming device ---" << std::endl;
    auto device_info = pAuthCallbackParams->deviceInfo;
    std::wcout << "Device name     : " << device_info.szName << std::endl;
    std::cout << "\tAddress         : " << device_info.Address.ullLong << std::endl;
    // See Section 2.8 Class of Device in "Assigned Numbers" document from BT spec
    // https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned%20Numbers.pdf
    std::cout << "\tClass           : " << device_info.ulClassofDevice << std::endl;
    std::cout << "\tIs connected    : " << device_info.fConnected << std::endl;
    std::cout << "\tIs remembered   : " << device_info.fRemembered << std::endl;
    std::cout << "\tIs authenticated: " << device_info.fAuthenticated << std::endl;
    std::cout << "Device I/O capability: " << pAuthCallbackParams->ioCapability << std::endl;
    std::cout << "BT authentication method: " << pAuthCallbackParams->authenticationMethod << std::endl;
    std::cout << "BT authentication requirements: " << pAuthCallbackParams->authenticationRequirements << std::endl;
    std::cout << "Numeric value: " << pAuthCallbackParams->Numeric_Value << std::endl;
    std::cout << "Passkey: " << pAuthCallbackParams->Passkey << std::endl;
    std::wstring passkey = std::to_wstring(pAuthCallbackParams->Passkey);
    std::wcout << "Converted passkey: " << passkey << "; size=" << passkey.size() << std::endl;
//    BluetoothAuthenticateDevice(NULL, NULL, &device_info, const_cast<PWSTR>(passkey.c_str()), passkey.size());
//    std::cout << "Sending passkey status: "
//              << BluetoothSendAuthenticationResponse(NULL, &device_info, passkey.c_str()) << std::endl;
    BLUETOOTH_AUTHENTICATE_RESPONSE response;
    response.bthAddressRemote = device_info.Address;
    response.authMethod = pAuthCallbackParams->authenticationMethod;
    BLUETOOTH_PASSKEY_INFO bt_passkey = {pAuthCallbackParams->Passkey};
    response.passkeyInfo = bt_passkey;

    std::cout << "Sending passkey status ex: "
              << BluetoothSendAuthenticationResponseEx(NULL, &response) << std::endl;
    return TRUE;
}

int main() {
    std::cout << "\tIs any radio connectable?: " << BluetoothIsConnectable(NULL) << std::endl;
    std::cout << "\tIs any radio discoverable?: " << BluetoothIsDiscoverable(NULL) << std::endl;
    std::cout << "\tEnable incoming connections on all radios: " << BluetoothEnableIncomingConnections(NULL, TRUE)
              << std::endl;
    std::cout << "\tEnable discovery of all radios" << BluetoothEnableDiscovery(NULL, TRUE) << std::endl;
    // Checking again what has changed
    std::cout << "\tIs any radio connectable?: " << BluetoothIsConnectable(NULL) << std::endl;
    std::cout << "\tIs any radio discoverable?: " << BluetoothIsDiscoverable(NULL) << std::endl;

    HBLUETOOTH_AUTHENTICATION_REGISTRATION auth_registration_handle;
    BluetoothRegisterForAuthenticationEx(NULL, &auth_registration_handle, PfnAuthenticationCallbackEx, NULL);

    std::thread t1([]() {
        auto sleep_time = 30s;
        std::cout << "Allowing for the BT radio to be found by sleeping for: " << sleep_time << std::endl;;
        std::this_thread::sleep_for(sleep_time);
    });
    t1.join();
}
