// sdk_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

static const char auth_json[] = u8"{\"method\":\"auth\",\"data\":{\"id\":\"deeplink_test\",\"token\":\"0123456789\"}}";

int main()
{
    // connect named pipe
    HANDLE hPipe = CreateFileA(
        "\\\\.\\pipe\\deeplink_sdk",
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
        uint32_t length = sizeof(auth_json) / sizeof(char) - 1;
        message.resize(length + 4);
        std::memset(&message[0], 0, length + 4);
        char* arrlen = reinterpret_cast<char*>(&length);
        std::memcpy(&message[0], arrlen, 4);
        std::memcpy(&message[4], auth_json, length);
    }

    for (int i = 0; i < message.size(); i++) {
        std::cout << "0x" << std::hex << (int)message[i] << " ";
    }
    std::cout << std::endl;

    DWORD dwBytesWritten;
    BOOL bWrite = WriteFile(
        hPipe,
        &message[0],
        message.size(),
        &dwBytesWritten,
        NULL
    );
    if (!bWrite) {
        std::cout << "Write pipe failed with error " << GetLastError() << std::endl;
    } else {
        std::cout << "Send message: " << message.size() << " " << dwBytesWritten << " " << message << std::endl;
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
        std::string body((szBuffer + 4), length);
        std::cout << "Received message: " << body << std::endl;
        for (int i = 0; i < length + 4; i++) {
            std::cout << std::hex << (int)szBuffer[i] << " ";
        }
        std::cout << std::endl;
    }

    // close named pipe
    CloseHandle(hPipe);
    std::cout << "close named pipe" << std::endl;
    return 0;
}
