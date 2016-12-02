#include "CageService.h"

#include <iostream>


CageService::CageService() {
}


CageService::~CageService() {
}


void CageService::startCageManager(LPCTSTR appName, DWORD sessionId) {
    startCageManager(appName, nullptr, sessionId);
}


std::string CageService::GetLastErrorAsString(DWORD errorID) {
    //Get the error message, if any.
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
                                 errorID,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPSTR)&messageBuffer,
                                 0,
                                 NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

// Must be part of the service
void CageService::startCageManager(LPCTSTR appName, LPTSTR desktopName, DWORD sessionId) {
    HANDLE hServiceToken;
    HANDLE hUserSessionToken;

    STARTUPINFO si = { sizeof si };
    si.lpDesktop = desktopName;
    PROCESS_INFORMATION pi;

    if (ImpersonateSelf(SecurityImpersonation)) {
        std::cout << "ImpersonateSelf was successful\n";
        if (OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, false, &hServiceToken)) {
            std::cout << "OpenThreadToken was successful\n";
            if (DuplicateTokenEx(hServiceToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hUserSessionToken)) {
                std::cout << "DuplicateTokenEx was successful\n";
                if (SetTokenInformation(hUserSessionToken, TokenSessionId, &sessionId, sizeof DWORD)) {
                    std::cout << "SetTokenInformation was successful\n";
                    if (CreateProcessAsUser(hUserSessionToken,
                                            appName,
                                            NULL,
                                            NULL,  // <- Process Attributes
                                            NULL,  // Thread Attributes
                                            false, // Inheritaion flags
                                            0,     // Creation flags
                                            NULL,  // Environment
                                            NULL,  // Current directory
                                            &si,   // Startup Info
                                            &pi)) {
                        std::cout << "CreateProcess (" << appName << ") was succesful\n";
                        CloseHandle(&si);
                        CloseHandle(&pi);
                    } else {
                        std::cout << "CreateProcess (" << appName << ") failed (" << GetLastError() << "): " << GetLastErrorAsString(GetLastError());
                    }
                } else {
                    std::cout << "SetTokenInformation failed (" << GetLastError() << "): " << GetLastErrorAsString(GetLastError());
                }
            } else {
                std::cout << "DuplicateTokenEx failed (" << GetLastError() << "): " << GetLastErrorAsString(GetLastError());
            }
        } else {
            std::cout << "OpenThreadToken failed (" << GetLastError() << "): " << GetLastErrorAsString(GetLastError());
        }
    } else {
        std::cout << "ImpersonateSelf failed (" << GetLastError() << "): " << GetLastErrorAsString(GetLastError());
    }
}

void CageService::handleMessage(std::string message) {
    // Parse message (ENUM constant (MSG_Service) + path/to/Exe)
    // Do action (start / stop application)
}
