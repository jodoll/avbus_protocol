#ifndef CERTIFICATE_STORE_H
#define CERTIFICATE_STORE_H

#include <FS.h>
#include <SSLCert.hpp>

class CertificateStore{
public:
    static CertificateStore* getInstance(){
        if(instance == nullptr) instance = new CertificateStore();
        return instance;
    }

    httpsserver::SSLCert* getCertificate();
private:
    static CertificateStore* instance;
};

#endif