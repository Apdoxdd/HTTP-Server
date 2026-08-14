#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>

int main() {
        SSL_library_init();
            std::cout << "OpenSSL version: " << OpenSSL_version(OPENSSL_VERSION) << std::endl;
                return 0;
}
