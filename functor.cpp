#include "functor.h"

Functor::Functor()
{
    this->m_path = GetExecutablePath();
    this->m_id = GenerateDeviceID();
}

std::wstring Functor::GetExecutablePath() {
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    return std::wstring(buffer);
}

std::wstring Functor::GetMacAddress() {
    IP_ADAPTER_INFO AdapterInfo[32];
    DWORD dwBufLen = sizeof(AdapterInfo);
    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
    if (dwStatus != ERROR_SUCCESS)
        return L"";

    std::wstring macAddress;
    for (int i = 0; i < AdapterInfo[0].AddressLength; i++) {
        wchar_t buf[3];
        swprintf_s(buf, L"%02X", AdapterInfo[0].Address[i]);
        macAddress += buf;
    }

    return macAddress;
}

std::wstring Functor::GetCpuId() {
    int CPUInfo[4] = { -1 };
    __cpuid(CPUInfo, 1);

    return std::to_wstring(CPUInfo[1]) + std::to_wstring(CPUInfo[3]);
}

std::wstring Functor::GenerateDeviceID() {
    std::wstring macAddress = this->GetMacAddress();
    std::wstring cpuId = GetCpuId();
    return L"KSK" + cpuId + L"_MAC" + macAddress;
}

std::wstring Functor::CreateFullscreenHTML(const std::wstring& url) {
    std::wstring htmlFilePath = L"fullscreen.html";

    std::wofstream htmlFile(htmlFilePath);
    if (htmlFile.is_open()) {
        htmlFile << L"<!DOCTYPE html>\n";
        htmlFile << L"<html>\n\n";

        htmlFile << L"<head>\n";
        htmlFile << "<title>Fullscreen</title>\n";
        htmlFile << "<script>\n<!–\nwindow.open(“bigpage.html”, ”fs”, ”fullscreen = yes”)\n</script>\n";
        htmlFile << "</head>\n\n";

        htmlFile << L"<body>\n";
        htmlFile << L"<script>\n";
        htmlFile << L"window.open('",
        htmlFile << url; 
        htmlFile << L"/";
        htmlFile << this->m_id;
        htmlFile << L"', '_blank', 'fullscreen=yes');\n";
        //htmlFile << L"window.close();\n";
        htmlFile << L"</script>\n";
        htmlFile << L"</body>\n\n";

        htmlFile << L"</html>\n";
        htmlFile.close();

        return htmlFilePath;
    }

    return L"";
}

bool Functor::SaveIDToFile(const std::wstring& id) {
    std::wofstream file("settings.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return false;
    }
    file << id;
    file.close();
    return true;
}

bool Functor::ElevateToAdmin(const std::wstring& exePath) {
    wchar_t moduleName[MAX_PATH];
    GetModuleFileName(NULL, moduleName, MAX_PATH);

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = moduleName;
    sei.nShow = SW_NORMAL;

    if (!ShellExecuteEx(&sei)) {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED) {
            std::cerr << "User denied the elevation request." << std::endl;
        }
        else {
            std::cerr << "Error while elevating to admin rights: " << error << std::endl;
        }
        return false;
    }

    return true;
}

bool Functor::OnStartUp(const std::wstring& exePath) {
    HKEY hKey;
    LPCTSTR sk = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

    LONG openReg = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sk, 0, KEY_READ, &hKey);

    if (openReg == ERROR_SUCCESS) {
        LPCTSTR value = TEXT("menudget_script");
        wchar_t data[MAX_PATH];
        DWORD dataSize = sizeof(data);
        DWORD dataType;

        LONG queryRes = RegQueryValueEx(hKey, value, NULL, &dataType, reinterpret_cast<LPBYTE>(data), &dataSize);

        if (queryRes == ERROR_SUCCESS && dataType == REG_SZ && wcscmp(data, exePath.c_str()) == 0) {
            std::cout << endl << "Startup registry entry already exists!" << std::endl;
            RegCloseKey(hKey);
            return true;
        }

        RegCloseKey(hKey);
    }

    openReg = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sk, 0, KEY_ALL_ACCESS, &hKey);

    if (openReg == ERROR_SUCCESS) {
        LPCTSTR value = TEXT("menudget_script");
        LPCTSTR data = exePath.c_str(); 

        DWORD dataSize = (lstrlen(data) + 1) * sizeof(TCHAR);
        RegSetValueEx(hKey, value, 0, REG_SZ, (LPBYTE)data, dataSize);
        RegCloseKey(hKey);

        std::cout << "Entry added to Windows Registry for auto-startup." << std::endl;

        return true;
    }
    else {
        std::cerr << "Failed to open or set Registry key." << std::endl;
        return false;
    }
}


bool Functor::CheckInternetConnection() {
    DWORD flags = 0;
    return InternetGetConnectedState(&flags, 0);
}


bool Functor::LaunchChromeFullscreen(const std::wstring& url) {
    std::wstring htmlFilePath = CreateFullscreenHTML(url);

    if (!htmlFilePath.empty()) {
        ShellExecute(NULL, L"open", htmlFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        SaveIDToFile(GenerateDeviceID());
        return true;
    }
    else {
        std::cerr << "Failed to create HTML file for launching Chrome in fullscreen." << std::endl;
        SaveIDToFile(GenerateDeviceID());
        return false;
    }
}
