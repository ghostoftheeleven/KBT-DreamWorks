#include <windows.h>
#include <string>
#include <tlhelp32.h>
#include <iostream>

int main()
{
    // Получение прав администратора
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, nullptr, nullptr);
        CloseHandle(hToken);
    }

    // Имя процесса, который мы хотим закрыть
    std::wstring processName = L"steam.exe";

    // Скрытие консольного окна
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Отсоединение приложения от консоли
    FreeConsole();

    // Бесконечный цикл мониторинга диспетчера задач
    while (true)
    {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        // Создание снимка процессов
        HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (Process32First(processSnapshot, &processEntry))
        {
            do
            {
                if (std::wstring(processEntry.szExeFile) == processName)
                {
                    // Закрытие процесса Steam через TerminateProcess()
                    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, processEntry.th32ProcessID);
                    if (processHandle != nullptr)
                    {
                        TerminateProcess(processHandle, 0);
                        CloseHandle(processHandle);
                    }
                }
            } while (Process32Next(processSnapshot, &processEntry));
        }
        CloseHandle(processSnapshot);

        // Ожидание перед следующей проверкой
        Sleep(1000);
    }

    return 0;
}