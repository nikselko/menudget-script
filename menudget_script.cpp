#pragma once

#include "functor.h"
 
int main() {

    HANDLE mutex = CreateMutex(NULL, TRUE, L"MenudgetMutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(mutex); return 0;}
  
    Functor functor_obj;
    cout << "*MENUDGET LAUNCH SCRIPT*" << endl;
    cout << endl << "-Path:  ";
    wcout << functor_obj.m_path << endl;

    std::wifstream file("settings.txt");
    std::wstring storedID;

    if (file.is_open()) {
        file >> storedID;
        file.close();
        functor_obj.m_id = storedID;
    }
    else{

        if (functor_obj.ElevateToAdmin(functor_obj.m_path)) {
            cout << endl << "-Admin rights: active" << endl;

            if (functor_obj.OnStartUp(functor_obj.m_path))
                cout << endl << "-Startup registry: active" << endl;
            else
                cout << endl << "-Startup registry: disabled" << endl;
        }
        else
            cout << endl << "-Admin rights: disabled" << endl;
    }
    
    if (functor_obj.CheckInternetConnection())
        cout << endl << "-Internet Connections: active" << endl;
    else
        cout << endl << "-Internet Connections: disabled" << endl;

    cout << endl << "-Device ID: ";
    wcout << functor_obj.m_id << endl;

    cout << endl << "-Laucnhing link: ";
    wcout << L"https://www.test.menudget.com/kiosk-sign-in" << L"/" << functor_obj.m_id;

    functor_obj.LaunchChromeFullscreen(L"https://www.test.menudget.com/kiosk-sign-in");
    
    while (true) {

        Sleep(1000);
    }

    CloseHandle(mutex);

    return 0;
}