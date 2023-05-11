// sdk_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

static const wchar_t auth_json[] = L"{\"method\":\"auth\",\"data\":{\"id\":\"deeplink_test\",\"token\":\"0123456789\"}}";

int main()
{
    // connect named pipe
    HANDLE hPipe = CreateFile(
        L"\\\\.\\pipe\\deeplink_sdk",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cout << "Create pipe failed with error " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    // send message to pipe
    std::string message;
    {
        // message does not require the '\0' character at the end
        uint32_t length = sizeof(auth_json) / sizeof(wchar_t) - 1;
        char* arrlen = reinterpret_cast<char*>(&length);
        message.append(arrlen, 4);
        length = length * 2;
        arrlen = reinterpret_cast<char*>(&length);
        message.append(arrlen, 4);
        message.append((const char*)&auth_json[0], length);
    }
    DWORD dwBytesWritten;
    BOOL bWrite = WriteFile(
        hPipe,
        message.c_str(),
        message.length(),
        &dwBytesWritten,
        NULL
    );
    if (!bWrite) {
        std::cout << "Write pipe failed with error " << GetLastError() << std::endl;
    } else {
        std::cout << "Send message: " << message << std::endl;
    }

    // received message from pipe
    char szBuffer[1024] = { 0 };
    DWORD dwBytesRead;
    BOOL bRead = ReadFile(
        hPipe,
        szBuffer,
        sizeof(szBuffer),
        &dwBytesRead,
        NULL
    );
    if (!bRead) {
        std::cout << "Read pipe failed with error " << GetLastError() << std::endl;
    } else {
        // parse message header
        uint32_t length = *reinterpret_cast<uint32_t*>(szBuffer);
        // parse message body
        std::wstring body((wchar_t*)(szBuffer + 8), length);
        std::wcout << L"Received message: " << body << std::endl;
    }

    // close named pipe
    CloseHandle(hPipe);
    std::cout << "close named pipe" << std::endl;
    return 0;
}
