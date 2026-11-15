// POA12 - Vi du 3: Expedition Console v0.1 (Kien truc 3 tang + kiem tra EOF + Function Contract)
// Pham vi: Bai 001 -> 012 (C++20)

#include <iostream>
#include <string>

// Declaration (Khai bao ham)
// Contract:
// - Input : 1 std::string bat ky (pass-by-value)
// - Output: Chuoi mo ta ket qua phan loai (Empty / Action 1 / Action 2 / Unknown)
// - Side effects: Khong doc cin, khong in cout, khong sua bien ngoai
std::string DescribeCommand(std::string command);

int main() {
    std::string runName{};
    std::string command{};

    // Tang 1: Nhan ten chuyen di va kiem tra thao tac doc stream
    std::cout << "Run name: ";
    if (!std::getline(std::cin, runName)) {
        std::cout << "Input unavailable (EOF)\n";
        return 0;
    }

    // Kiem tra luat nghiep vu cho ten chuyen di
    if (runName.empty()) {
        std::cout << "Rejected: empty run name\n";
        return 0;
    }

    // Dat trong dau [] de de quan sat khoang trang dau/cuoi neu co
    std::cout << "Run created: [" << runName << "] (" << runName.size() << " bytes)\n";

    // Tang 1: Nhan lenh van ban
    std::cout << "Command (mine/status): ";
    if (!std::getline(std::cin, command)) {
        std::cout << "Input unavailable (EOF)\n";
        return 0;
    }

    // Tang 2: Dien giai lenh qua ham thuan
    std::cout << "Result: " << DescribeCommand(command) << '\n';

    return 0;
}

// Definition (Dinh nghia ham)
std::string DescribeCommand(std::string command) {
    const std::string kCmdMine{"mine"};
    const std::string kCmdStatus{"status"};

    if (command.empty()) {
        return "Rejected: empty command";
    }

    if (command == kCmdMine) {
        return "Mining action selected";
    }

    if (command == kCmdStatus) {
        return "Showing expedition status";
    }

    return "Unknown command: [" + command + "]";
}
