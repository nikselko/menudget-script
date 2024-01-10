#pragma once

#include <iostream>
#include <Windows.h>
#include <Shellapi.h>
#include <WinInet.h>
#include <Shlwapi.h>
#include <fstream>
#include <iphlpapi.h>
#include <string>
#include <intrin.h>
#pragma comment(lib, "IPHLPAPI.lib")

using namespace std;

class Functor
{
public:
	Functor();

	std::wstring m_path; // Path to exe file

	std::wstring m_id; // Unique ID based on CPU & MAC Adress


	std::wstring GetExecutablePath();

	std::wstring GetMacAddress();

	std::wstring GetCpuId();

	std::wstring GenerateDeviceID();

	std::wstring CreateFullscreenHTML(const std::wstring& url);



	bool SaveIDToFile(const std::wstring& id);

	bool ElevateToAdmin(const std::wstring& exePath);

	bool OnStartUp(const std::wstring& exePath);

	bool CheckInternetConnection();

	bool LaunchChromeFullscreen(const std::wstring& url);
};
