A simple webscraper in C++ using OpenSSL and HTTPS (Linux / MacOS compatible; does not run on Windows without WSL / Virtual Machines)

src/main.cpp:
    - Linux/MacOS compatable. Checks if TARGETURL exists, then performs OpenSSL handshake then HTTP GET request to display content in console.
    - Compile and run (on !Windows) > g++ src/main.cpp -o src/main -lssl -lcrypto && ./src/main

src/wndws.cpp:
    - Windows compatable. Checks if TARGETURL exists, then performs OpenSSL handshake then HTTP GET request to display content in console.
    - Compile and run (on Windows) > g++ src/main.cpp -o src/main -I../path/to/openssl/include -lssl -lcrypto -lws2_32 && ./src/main (DOES NOT WORK AFAIK; TODO! <openssl.h> is not found)
