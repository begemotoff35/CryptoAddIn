//
// Created by begemot on 29.01.2021.
//

#ifndef CRYPTO_MYCRYPTO_H
#define CRYPTO_MYCRYPTO_H

#include <windows.h>
#include <tchar.h>

class MyCrypto {
public:
    MyCrypto();
    MyCrypto(LPCTSTR szContainer);
    ~MyCrypto();

    BOOL Init(LPCTSTR szContainer = _T("MyKeyContainer"));
    BOOL GenerateRandom(DWORD dwLen, BYTE *pbBuffer);


protected:
    HCRYPTPROV hCryptProv;
};


#endif //CRYPTO_MYCRYPTO_H
