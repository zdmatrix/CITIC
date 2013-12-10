/*++

Copyright (C) Microsoft Corporation, 1998 - 1999

Module Name:

    autoreg

Abstract:

    This module provides autoregistration capabilities to a CSP.  It allows
    regsvr32 to call the DLL directly to add and remove Registry settings.

Author:

    Doug Barlow (dbarlow) 3/11/1998

Environment:

    Win32

Notes:

    Look for "?vendor?" tags and edit appropriately.

--*/

// ?vendor? If this is a smart card CSP, uncomment the next line.
// #define SCARD_CSP

#ifndef SCARD_CSP
#define SCARD_CSP
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//#ifdef _AFXDLL
#include "stdafx.h"
//#else
//#include <windows.h>
//#endif
#ifndef WINVER
#define WINVER 0x0400
#endif
#include <wincrypt.h>
#ifdef SCARD_CSP
#include <winscard.h>
#endif
#include <tchar.h>
#include "HED_CryptoAPI.h"

static HMODULE
GetInstanceHandle(
    void);

#ifdef SCARD_CSP
static const TCHAR
    l_szCardName[]
        = TEXT("Ws Smart Card 0");
static const GUID   // Optional
    l_guidPrimaryProv
  //0xc0cf0646, 0x5f6e, 0x11d2, 0xb6, 0x77, 0x0, 0xc0, 0xdf, 0xe4, 0xc1, 0xf3);
        = { /* 00000000-0000-0000-0000-000000000000 */
            0xc0cf0646,
            0x5f6e,
            0x11d2,
            { 0xb6, 0x77, 0x0, 0xc0, 0xdf, 0xe4, 0xc1, 0xf3 }
          };
static const BYTE
    l_rgbATR[]     = { /* ?vendor? Add your ATR here */ 0x3b, 0x00 },
    l_rgbATRMask[] = { /* ?vendor? Add your ATR Mask here */ 0xff, 0xff };
#endif

static const TCHAR
    l_szProviderName[]
        = TEXT("HED_RSA_Cryptographic_Service_Provider_V1.0");
static const DWORD
    l_dwCspType
// ?vendor?  Change this to match your CSP capabilities
        = PROV_RSA_FULL;

#ifdef SCARD_CSP
typedef DWORD
(*LPSETCARDTYPEPROVIDERNAME)(
    IN SCARDCONTEXT hContext,
    IN LPCTSTR szCardName,
    IN DWORD dwProviderId,
    IN LPCTSTR szProvider);
#endif


/*++

DllUnregisterServer:

    This service removes the registry entries associated with this CSP.

Arguments:

    None

Return Value:

    Status code as an HRESULT.

Author:

    Doug Barlow (dbarlow) 3/11/1998

--*/

STDAPI
DllUnregisterServer(
    void)
{
    LONG nStatus;
    DWORD dwDisp;
    HRESULT hReturnStatus = NO_ERROR;
    HKEY hProviders = NULL;
#ifdef SCARD_CSP
    SCARDCONTEXT hCtx = NULL;
#endif

#ifdef _AFXDLL
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif


    //
    // Delete the Registry key for this CSP.
    //

    nStatus = RegCreateKeyEx(
                    HKEY_LOCAL_MACHINE,
                    TEXT("SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider"),
                    0,
                    TEXT(""),
                    REG_OPTION_NON_VOLATILE,
                    KEY_ALL_ACCESS,
                    NULL,
                    &hProviders,
                    &dwDisp);
    if (ERROR_SUCCESS == nStatus)
    {
        RegDeleteKey(hProviders, l_szProviderName);
        RegCloseKey(hProviders);
        hProviders = NULL;
    }


#ifdef SCARD_CSP
    //
    // Forget the card type.
    //

    hCtx = NULL;
    SCardEstablishContext(SCARD_SCOPE_SYSTEM, 0, 0, &hCtx);
    SCardForgetCardType(hCtx, l_szCardName);
    if (NULL != hCtx)
    {
        SCardReleaseContext(hCtx);
        hCtx = NULL;
    }
#endif


    //
    // ?vendor?
    // Delete vendor specific registry entries.
    //



    //
    // All done!
    //

    return hReturnStatus;
}


/*++

DllRegisterServer:

    This function installs the proper registry entries to enable this CSP.

Arguments:

    None

Return Value:

    Status code as an HRESULT.

Author:

    Doug Barlow (dbarlow) 3/11/1998

--*/

STDAPI
DllRegisterServer(
    void)
{
    TCHAR szModulePath[MAX_PATH];
    BYTE pbSignature[136];  // Room for a 1024 bit signature, with padding.
    OSVERSIONINFO osVer;
    LPTSTR szFileName, szFileExt;
    HINSTANCE hThisDll;
    HRSRC hSigResource;
    DWORD dwStatus;
    LONG nStatus;
    BOOL fStatus;
    DWORD dwDisp;
    DWORD dwIndex;
    DWORD dwSigLength;
    HRESULT hReturnStatus = NO_ERROR;
    HKEY hProviders = NULL;
    HKEY hMyCsp = NULL;
    HKEY hCalais = NULL;
    HKEY hVendor = NULL;
    BOOL fSignatureFound = FALSE;
    HANDLE hSigFile = INVALID_HANDLE_VALUE;
#ifdef SCARD_CSP
    BOOL fCardIntroduced = FALSE;
    SCARDCONTEXT hCtx = NULL;
#endif

#ifdef _AFXDLL
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif


    //
    // Figure out the file name and path.
    //

    hThisDll = GetInstanceHandle();
    if (NULL == hThisDll)
    {
        hReturnStatus = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
        goto ErrorExit;
    }

    dwStatus = GetModuleFileName(
                    hThisDll,
                    szModulePath,
                    sizeof(szModulePath) / sizeof(TCHAR));
    if (0 == dwStatus)
    {
        hReturnStatus = HRESULT_FROM_WIN32(GetLastError());
        goto ErrorExit;
    }

    szFileName = _tcsrchr(szModulePath, TEXT('\\'));
    if (NULL == szFileName)
        szFileName = szModulePath;
    else
        szFileName += 1;
    szFileExt = _tcsrchr(szFileName, TEXT('.'));
    if (NULL == szFileExt)
    {
        hReturnStatus = HRESULT_FROM_WIN32(ERROR_INVALID_NAME);
        goto ErrorExit;
    }
    else
        szFileExt += 1;


    //
    // Create the Registry key for this CSP.
    //

    nStatus = RegCreateKeyEx(
                    HKEY_LOCAL_MACHINE,
                    TEXT("SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider"),
                    0,
                    TEXT(""),
                    REG_OPTION_NON_VOLATILE,
                    KEY_ALL_ACCESS,
                    NULL,
                    &hProviders,
                    &dwDisp);
    if (ERROR_SUCCESS != nStatus)
    {
        hReturnStatus = HRESULT_FROM_WIN32(nStatus);
        goto ErrorExit;
    }
    nStatus = RegCreateKeyEx(
                    hProviders,
                    l_szProviderName,
                    0,
                    TEXT(""),
                    REG_OPTION_NON_VOLATILE,
                    KEY_ALL_ACCESS,
                    NULL,
                    &hMyCsp,
                    &dwDisp);
    if (ERROR_SUCCESS != nStatus)
    {
        hReturnStatus = HRESULT_FROM_WIN32(nStatus);
        goto ErrorExit;
    }
    nStatus = RegCloseKey(hProviders);
    hProviders = NULL;
    if (ERROR_SUCCESS != nStatus)
    {
        hReturnStatus = HRESULT_FROM_WIN32(nStatus);
        goto ErrorExit;
    }


    //
    // Install the trivial registry values.
    //

    nStatus = RegSetValueEx(
                    hMyCsp,
                    TEXT("Image Path"),
                    0,
                    REG_SZ,
                    (LPBYTE)szModulePath,
                    (_tcslen(szModulePath) + 1) * sizeof(TCHAR));
    if (ERROR_SUCCESS != nStatus)
    {
        hReturnStatus = HRESULT_FROM_WIN32(nStatus);
        goto ErrorExit;
    }

    nStatus = RegSetValueEx(
                    hMyCsp,
                    TEXT("Type"),
                    0,
                    REG_DWORD,
                    (LPBYTE)&l_dwCspType,
                    sizeof(DWORD));
    if (ERROR_SUCCESS != nStatus)
    {
        hReturnStatus = HRESULT_FROM_WIN32(nStatus);
        goto ErrorExit;
    }


    //
    // See if we're self-signed.  On NT5, CSP images can carry their own
    // signatures.
    //

    hSigResource = FindResource(
                        hThisDll,
                        MAKEINTRESOURCE(CRYPT_SIG_RESOURCE_NUMBER),
                        RT_RCDATA);


    //
    // Install the file signature.
    //

    ZeroMemory(&osVer, sizeof(OSVERSIONINFO));
    osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    fStatus = GetVersionEx(&osVer);
    // ?BUGBUG? -- This works on Windows Millenium, too.
    if (fStatus
        && (VER_PLATFORM_WIN32_NT == osVer.dwPlatformId)
        && (5 <= osVer.dwMajorVersion)
        && (NULL != hSigResource))
    {

        //
        // Signature in file flag is sufficient.
        //

        dwStatus = 0;
        nStatus = RegSetValueEx(
                        hMyCsp,
                        TEXT("SigInFile"),
                        0,
                        REG_DWORD,
                        (LPBYTE)&dwStatus,
                        sizeof(DWORD));
        if (ERROR_SUCCESS != nStatus)
        {
            hReturnStatus = HRESULT_FROM_WIN32(nStatus);
            goto ErrorExit;
        }
    }
    else
    {

        //
        // We have to install a signature entry.
        // Try various techniques until one works.
        //

        for (dwIndex = 0; !fSignatureFound; dwIndex += 1)
        {
            switch (dwIndex)
            {

            //
            // Look for an external *.sig file and load that into the registry.
            //

            case 0:
                _tcscpy(szFileExt, TEXT("sig"));
                hSigFile = CreateFile(
                                szModulePath,
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
                if (INVALID_HANDLE_VALUE == hSigFile)
                    continue;
                dwSigLength = GetFileSize(hSigFile, NULL);
                if ((dwSigLength > sizeof(pbSignature))
                    || (dwSigLength < 72))      // Accept a 512-bit signature
                {
                    hReturnStatus = NTE_BAD_SIGNATURE;
                    goto ErrorExit;
                }

                fStatus = ReadFile(
                                hSigFile,
                                pbSignature,
                                sizeof(pbSignature),
                                &dwSigLength,
                                NULL);
                if (!fStatus)
                {
                    hReturnStatus = HRESULT_FROM_WIN32(GetLastError());
                    goto ErrorExit;
                }
                fStatus = CloseHandle(hSigFile);
                hSigFile = NULL;
                if (!fStatus)
                {
                    hReturnStatus = HRESULT_FROM_WIN32(GetLastError());
                    goto ErrorExit;
                }
                fSignatureFound = TRUE;
                break;


            //
            // Other cases may be added in the future.
            //

            default:
                hReturnStatus = NTE_BAD_SIGNATURE;
                goto ErrorExit;
            }

            if (fSignatureFound)
            {
                for (dwIndex = 0; dwIndex < dwSigLength; dwIndex += 1)
                {
                    if (0 != pbSignature[dwIndex])
                        break;
                }
                if (dwIndex >= dwSigLength)
                    fSignatureFound = FALSE;
            }
        }


        //
        // We've found a signature somewhere!  Install it.
        //

        nStatus = RegSetValueEx(
                        hMyCsp,
                        TEXT("Signature"),
                        0,
                        REG_BINARY,
                        pbSignature,
                        dwSigLength);
        if (ERROR_SUCCESS != nStatus)
        {
            hReturnStatus = HRESULT_FROM_WIN32(nStatus);
            goto ErrorExit;
        }
    }

    nStatus = RegCloseKey(hMyCsp);
    hMyCsp = NULL;
    if (ERROR_SUCCESS != nStatus)
    {
        hReturnStatus = HRESULT_FROM_WIN32(nStatus);
        goto ErrorExit;
    }


#ifdef SCARD_CSP
    //
    // Introduce the vendor card.  Try various techniques until one works.
    //

    for (dwIndex = 0; !fCardIntroduced; dwIndex += 1)
    {
        switch (dwIndex)
        {
        case 0:
            {
                HMODULE hWinSCard = NULL;
                LPSETCARDTYPEPROVIDERNAME pfSetCardTypeProviderName = NULL;

                hWinSCard = GetModuleHandle(TEXT("WinSCard.DLL"));
                if (NULL == hWinSCard)
                    continue;
#if defined(UNICODE)
                pfSetCardTypeProviderName =
                    (LPSETCARDTYPEPROVIDERNAME)GetProcAddress(
                        hWinSCard,
                        TEXT("SCardSetCardTypeProviderNameW"));
#else
                pfSetCardTypeProviderName =
                    (LPSETCARDTYPEPROVIDERNAME)GetProcAddress(
                        hWinSCard,
                        TEXT("SCardSetCardTypeProviderNameA"));
#endif
                if (NULL == pfSetCardTypeProviderName)
                    continue;

                dwStatus = SCardIntroduceCardType(
                                NULL,
                                l_szCardName,
                                NULL,
                                NULL,
                                0,
                                l_rgbATR,
                                l_rgbATRMask,
                                sizeof(l_rgbATR));
                if ((ERROR_SUCCESS != dwStatus)
                    && (ERROR_ALREADY_EXISTS != dwStatus))
                    continue;
                dwStatus = (*pfSetCardTypeProviderName)(
                                NULL,
                                l_szCardName,
                                SCARD_PROVIDER_CSP,
                                l_szProviderName);
                if (ERROR_SUCCESS != dwStatus)
                {
                    if (0 == (dwStatus & 0xffff0000))
                        hReturnStatus = HRESULT_FROM_WIN32(dwStatus);
                    else
                        hReturnStatus = (HRESULT)dwStatus;
                    goto ErrorExit;
                }
                fCardIntroduced = TRUE;
                break;
            }

        case 1:
            dwStatus = SCardEstablishContext(SCARD_SCOPE_SYSTEM, 0, 0, &hCtx);
            if (ERROR_SUCCESS != dwStatus)
                continue;
            dwStatus = SCardIntroduceCardType(
                            hCtx,
                            l_szCardName,
                            NULL,
                            NULL,
                            0,
                            l_rgbATR,
                            l_rgbATRMask,
                            sizeof(l_rgbATR));
            if ((ERROR_SUCCESS != dwStatus)
                && (ERROR_ALREADY_EXISTS != dwStatus))
            {
                if (0 == (dwStatus & 0xffff0000))
                    hReturnStatus = HRESULT_FROM_WIN32(dwStatus);
                else
                    hReturnStatus = (HRESULT)dwStatus;
                goto ErrorExit;
            }
            dwStatus = SCardReleaseContext(hCtx);
            hCtx = NULL;
            if (ERROR_SUCCESS != dwStatus)
            {
                if (0 == (dwStatus & 0xffff0000))
                    hReturnStatus = HRESULT_FROM_WIN32(dwStatus);
                else
                    hReturnStatus = (HRESULT)dwStatus;
                goto ErrorExit;
            }
            nStatus = RegCreateKeyEx(
                            HKEY_LOCAL_MACHINE,
                            TEXT("SOFTWARE\\Microsoft\\Cryptography\\Calais\\SmartCards"),
                            0,
                            TEXT(""),
                            REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS,
                            NULL,
                            &hCalais,
                            &dwDisp);
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegCreateKeyEx(
                            hCalais,
                            l_szCardName,
                            0,
                            TEXT(""),
                            REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS,
                            NULL,
                            &hVendor,
                            &dwDisp);
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegCloseKey(hCalais);
            hCalais = NULL;
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegSetValueEx(
                            hVendor,
                            TEXT("Crypto Provider"),
                            0,
                            REG_SZ,
                            (LPBYTE)l_szProviderName,
                            (_tcslen(l_szProviderName) + 1) * sizeof(TCHAR));
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }

            nStatus = RegCloseKey(hVendor);
            hVendor = NULL;
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }

            fCardIntroduced = TRUE;
            break;

        case 2:
            nStatus = RegCreateKeyEx(
                            HKEY_LOCAL_MACHINE,
                            TEXT("SOFTWARE\\Microsoft\\Cryptography\\Calais\\SmartCards"),
                            0,
                            TEXT(""),
                            REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS,
                            NULL,
                            &hCalais,
                            &dwDisp);
            if (ERROR_SUCCESS != nStatus)
                continue;
            nStatus = RegCreateKeyEx(
                            hCalais,
                            l_szCardName,
                            0,
                            TEXT(""),
                            REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS,
                            NULL,
                            &hVendor,
                            &dwDisp);
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegCloseKey(hCalais);
            hCalais = NULL;
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegSetValueEx(
                            hVendor,
                            TEXT("Primary Provider"),
                            0,
                            REG_BINARY,
                            (LPCBYTE)&l_guidPrimaryProv,
                            sizeof(l_guidPrimaryProv));
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegSetValueEx(
                            hVendor,
                            TEXT("ATR"),
                            0,
                            REG_BINARY,
                            l_rgbATR,
                            sizeof(l_rgbATR));
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegSetValueEx(
                            hVendor,
                            TEXT("ATRMask"),
                            0,
                            REG_BINARY,
                            l_rgbATRMask,
                            sizeof(l_rgbATRMask));
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegSetValueEx(
                            hVendor,
                            TEXT("Crypto Provider"),
                            0,
                            REG_SZ,
                            (LPBYTE)l_szProviderName,
                            (_tcslen(l_szProviderName) + 1) * sizeof(TCHAR));
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            nStatus = RegCloseKey(hVendor);
            hVendor = NULL;
            if (ERROR_SUCCESS != nStatus)
            {
                hReturnStatus = HRESULT_FROM_WIN32(nStatus);
                goto ErrorExit;
            }
            fCardIntroduced = TRUE;
            break;

        default:
            hReturnStatus = ERROR_ACCESS_DENIED;
            goto ErrorExit;
        }
    }
#endif


    //
    // ?vendor?
    // Add any additional initialization required here.
    //



    //
    // All done!
    //

    return hReturnStatus;


    //
    // An error was detected.  Clean up any outstanding resources and
    // return the error.
    //

    ErrorExit:
#ifdef SCARD_CSP
    if (NULL != hCtx)
        SCardReleaseContext(hCtx);
    if (NULL != hCalais)
        RegCloseKey(hCalais);
#endif
    if (NULL != hVendor)
        RegCloseKey(hVendor);
    if (INVALID_HANDLE_VALUE != hSigFile)
        CloseHandle(hSigFile);
    if (NULL != hMyCsp)
        RegCloseKey(hMyCsp);
    if (NULL != hProviders)
        RegCloseKey(hProviders);
    DllUnregisterServer();
    return hReturnStatus;
}


/*++

GetInstanceHandle:

    This routine is CSP dependant.  It returns the DLL instance handle.  This
    is typically provided by the DllMain routine and stored in a global
    location.

Arguments:

    None

Return Value:

    The DLL Instance handle provided to the DLL when DllMain was called.

Author:

    Doug Barlow (dbarlow) 3/11/1998

--*/

extern HINSTANCE g_hModule;

static HINSTANCE
GetInstanceHandle(
    void)
{
#ifdef _AFXDLL
    return AfxGetInstanceHandle();
#else
    //  ?vendor?
    // Make sure this returns your DLL instance handle.
    return g_hModule;
#endif
}

