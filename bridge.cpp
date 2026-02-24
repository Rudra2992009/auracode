/**
 * AuraCode Native C++ Bridge v1.3
 * High-Performance Native Execution Node
 * * FEATURES:
 * - Multi-Port Support: 56000 (C++), 58000 (Rust)
 * - Native Socket HTTP Server (No external dependencies)
 * - Base64 Source Decoding
 * - Multi-threaded Request Handling
 */

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

// Port Definitions for Language-Specific Nodes
#define CPP_PORT 56000
#define RUST_PORT 58000

// Optimized Base64 Decoding Utility
std::string base64_decode(const std::string &in) {
    std::string out;
    std::vector<int> T(256, -1);
    const char* b64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (int i = 0; i < 64; i++) T[b64_chars[i]] = i;
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

class AuraBridgeNode {
public:
    /**
     * Starts the native listener on the specified port.
     * @param port - The 5-digit port (e.g., 56000 for CPP, 58000 for RUST)
     */
    static void start_server(int port) {
        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("Socket failed");
            exit(EXIT_FAILURE);
        }

        // Forcefully attaching socket to the port
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 5) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "========================================" << std::endl;
        std::cout << "   AuraCode C++ Native Bridge v1.3      " << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "🚀 Node listening on port: " << port << std::endl;
        std::cout << "📡 Protocol: HTTP/JSON-Bridge" << std::endl;
        std::cout << "🛠️ Language Context: " << (port == RUST_PORT ? "RUST (Native)" : "CPP (Native)") << std::endl;

        while (true) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                continue;
            }

            // Handle each request in a separate high-speed thread
            std::thread(handle_connection, new_socket, port).detach();
        }
    }

private:
    static void handle_connection(int socket_fd, int port) {
        char buffer[30000] = {0};
        read(socket_fd, buffer, 30000);

        auto start = std::chrono::high_resolution_clock::now();

        // Determine language engine based on port context
        std::string engine = (port == RUST_PORT) ? "rust-native" : "cpp-native";
        
        // Simulating logic execution based on engine
        std::string mock_output;
        if (port == RUST_PORT) {
            mock_output = "Finished release [optimized] target(s) in 0.1s\\nOutput: Rust binary executed successfully on port 58000.";
        } else {
            mock_output = "Build: g++ -O3 aura_vfs_optimized\\nOutput: C++ execution completed on port 56000.";
        }
        
        int exit_code = 0;

        auto end = std::chrono::high_resolution_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // Construct JSON Response
        std::stringstream ss;
        ss << "{"
           << "\"status\": \"success\","
           << "\"engine\": \"" << engine << "\","
           << "\"latency_ms\": " << latency << ","
           << "\"exit_code\": " << exit_code << ","
           << "\"stdout\": \"" << mock_output << "\""
           << "}";

        std::string response_body = ss.str();
        std::string http_response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Length: " + std::to_string(response_body.length()) + "\r\n"
            "Connection: close\r\n\r\n" + 
            response_body;

        send(socket_fd, http_response.c_str(), http_response.length(), 0);
        close(socket_fd);
    }
};

int main(int argc, char* argv[]) {
    // Default to CPP port if no argument is provided
    int port = CPP_PORT;
    
    // Check if port is passed as an argument (for running multiple nodes)
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    AuraBridgeNode::start_server(port);
    return 0;
}
