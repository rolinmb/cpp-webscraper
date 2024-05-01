#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 1024

const char *TARGETURL = "finance.yahoo.com";

int main() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    struct addrinfo hints, *result, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(TARGETURL, "https", &hints, &result) != 0) {
        std::cerr << "Error resolving hostname" << std::endl;
        return 1;
    }
    int sockfd;
    for (p = result; p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) != -1) {
            break;
        }
        close(sockfd);
    }
    if (p == nullptr) {
        std::cerr << "Error connecting to server" << std::endl;
        freeaddrinfo(result);
        return 1;
    }
    freeaddrinfo(result);
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ssl_ctx) {
        std::cerr << "Error creating SSL context" << std::endl;
        close(sockfd);
        return 1;
    }
    SSL *ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        std::cerr << "Error creating SSL connection" << std::endl;
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return 1;
    }
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) != 1) {
        std::cerr << "Error performing SSL handshake" << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return 1;
    }
    std::string http_request = "GET / HTTP/1.1\r\n";
    http_request += "Host: ";
    http_request += TARGETURL;
    http_request += "\r\n";
    http_request += "Connection: close\r\n\r\n";
    if (SSL_write(ssl, http_request.c_str(), http_request.length()) < 0) {
        std::cerr << "Error sending request" << std::endl;
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ssl_ctx);
        close(sockfd);
        return 1;
    }
    char buffer[BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = SSL_read(ssl, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_received] = '\0';
        std::cout << buffer;
    }
    if (bytes_received < 0) {
        std::cerr << "Error receiving response" << std::endl;
    }
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ssl_ctx);
    close(sockfd);
    return 0;
}
