#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <string>   

std::mutex fileMutex;


void pingAddress(const std::string& address) {
    std::string tempFile = address + "_temp.txt";

#ifdef _WIN32
    std::string command = "ping -n 4 " + address + " > " + tempFile;
#else
    std::string command = "ping -c 4 " + address + " > " + tempFile;
#endif

    int result = system(command.c_str());

    std::lock_guard<std::mutex> lock(fileMutex);
    std::ofstream output("result.txt", std::ios::app);
    if (output.is_open()) {
        output << "===== Ket qua ping: " << address << " =====\n";
        std::ifstream temp(tempFile);
        if (temp.is_open()) {
            output << temp.rdbuf();  
            temp.close();
            std::remove(tempFile.c_str()); 
        }
        else {
            output << "Khong the mo file tam.\n";
        }
        output << "\n";
        output.close();
    }
}

int main() {
    std::vector<std::string> addresses;
    std::string input;

    std::cout << "Nhap dia chi IP hoac ten mien (nhap 'xong' de ket thuc):\n";
    while (true) {
        std::getline(std::cin, input);
        if (input == "xong") break;
        if (!input.empty()) addresses.push_back(input);
    }

    std::ofstream clearFile("result.txt", std::ios::trunc);
    clearFile.close();

    std::vector<std::thread> threads;
    for (const auto& addr : addresses) {
        threads.emplace_back(pingAddress, addr);
    }

    for (auto& th : threads) {
        if (th.joinable()) th.join();
    }

    std::cout << "Hoan tat. Ket qua da luu vao file result.txt\n";
    return 0;
}
