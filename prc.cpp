// prc.cpp
#include "prc.h"
#include <windows.h>
#include <TlHelp32.h>

PRC::PRC() {}

QStringList PRC::getProcessList()
{
    QStringList processList;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE)
        return processList;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            processList.append(QString::fromWCharArray(pe.szExeFile));
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return processList;
}
