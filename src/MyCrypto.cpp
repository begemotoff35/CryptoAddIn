//
// Created by begemot on 29.01.2021.
//

#include "MyCrypto.h"

MyCrypto::MyCrypto() : hCryptProv(NULL) {}

MyCrypto::MyCrypto(LPCTSTR szContainer) : hCryptProv(NULL) {
    this->Init(szContainer);
}

MyCrypto::~MyCrypto() {
    if (hCryptProv)
        CryptReleaseContext(hCryptProv, 0);
}

BOOL MyCrypto::Init(LPCTSTR szContainer) {
    if (hCryptProv)
        return TRUE;
//-------------------------------------------------------------------
// Attempt to acquire a context and a key
// container. The context will use the default CSP
// for the RSA_FULL provider type. DwFlags is set to zero
// to attempt to open an existing key container.

    BOOL success;
    success = CryptAcquireContext(
            &hCryptProv,               // handle to the CSP
            szContainer,                  // container name
            NULL,                      // use the default provider
            PROV_RSA_FULL,             // provider type
            0);                        // flag values
    if (!success)
    {
//-------------------------------------------------------------------
// An error occurred in acquiring the context. This could mean
// that the key container requested does not exist. In this case,
// the function can be called again to attempt to create a new key
// container. Error codes are defined in Winerror.h.
        if (GetLastError() == NTE_BAD_KEYSET) {
            if (CryptAcquireContext(
                    &hCryptProv,
                    szContainer,
                    NULL,
                    PROV_RSA_FULL,
                    CRYPT_NEWKEYSET)) {
                success = TRUE;
            }
        }
    }
    return success;
}

BOOL MyCrypto::GenerateRandom(DWORD dwLen, BYTE *pbBuffer) {
    if (hCryptProv) {
        return CryptGenRandom(hCryptProv, dwLen, pbBuffer);
    }
    return FALSE;
}
