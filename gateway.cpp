#include <iostream>
#include <string>
#include <winsock2.h>

class Gateway
{
public:
  Gateway()
  {
    getDefaultGateway();
  }
  std::string getDefaultGateway(){
    char gateway[100];
    unsigned long sz = 100;

    if (GetDefaultGateway(gateway, &sz) == NO_ERROR)
    {
      return std::string(gateway);
    }
    else
      return "";
  }

private:
  unsigned long GetDefaultGateway(char * gateway, unsigned long * sz)
  {
    return GetAdaptersInfo(NULL, sz);
  }

};


//#include "gateway.h"
//#include <windows.h>
//#include <iphlpapi.h>
//#include <QtNetwork>

//#pragma comment(lib, "iphlpapi.lib")

//Gateway::Gateway()
//{
//}

//QString Gateway::getDefaultGateway()
//{
//    QString gtw = "";
//    DWORD dwSize = 0;
//    GetIpForwardTable(NULL, &dwSize, TRUE);
//    PMIB_IPFORWARDTABLE pIpForwardTable = (PMIB_IPFORWARDTABLE)malloc(dwSize);
//    if (GetIpForwardTable(pIpForwardTable, &dwSize, TRUE) == NO_ERROR) {
//        for (int i = 0; i < (int) pIpForwardTable->dwNumEntries; i++) {
//            if (pIpForwardTable->table[i].dwForwardDest == 0) {
//                gtw = QString("%1.%2.%3.%4")
//                      .arg((pIpForwardTable->table[i].dwForwardNextHop & 0xff000000) >> 24)
//                      .arg((pIpForwardTable->table[i].dwForwardNextHop & 0x00ff0000) >> 16)
//                      .arg((pIpForwardTable->table[i].dwForwardNextHop & 0x0000ff00) >> 8)
//                      .arg((pIpForwardTable->table[i].dwForwardNextHop & 0x000000ff));
//                break;
//            }
//        }
//    }
//    free(pIpForwardTable);
//    return gtw;
//}
