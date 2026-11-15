// POA12 - Vi du 2 (File Thuc Hanh): Nhap ten chuyen di va mot lenh hanh dong
// Dung lai: Bai 004-007 (bien, const, phep tinh, if), Bai 009 (&&), Bai 010-011 (ham, truyen gia tri)
// Kien thuc moi Bai 012: std::string, .empty(), .length(), +, ==, std::getline

#include <iostream>
#include <string>

// Ham thuc thi 1 lenh tren lung Whaleon va tra ve luong khoang pha le MOI sau lenh
int xuLyLenhChuyenDi(std::string command, int hp, int currentOre, int maxCargo) {
    // [VIEC CUA ANH - BUOC 2]: Doi ten 2 hanh dong mac dinh duoi day thanh ten lenh anh muon
    // (Co the dung lenh co khoang trang nhu "khai thac", "rut lui" de thu suc manh cua getline!)
    const std::string cmdAction1{"mine"};
    const std::string cmdAction2{"retreat"};

    // [VIEC CUA ANH - BUOC 1]: Xu ly rieng truong hop lenh rong (nguoi choi chi bam Enter)
    // Goi y: Kiem tra command.empty() TRUOC KHI so sanh voi cac lenh hop le.
    // Neu rong -> in thong bao nhac nguoi choi chua nhap lenh va giu nguyen currentOre.

    if (command == cmdAction1) {
        // Luat ket hop (Bai 009): Chi khai thac duoc khi con HP va kho chua day
        if (hp > 0 && currentOre < maxCargo) {
            const int oreThuDuoc{3};
            int oreMoi{currentOre + oreThuDuoc};
            if (oreMoi > maxCargo) {
                oreMoi = maxCargo; // Chan vuot suc chua (Bai 007)
            }
            std::cout << "-> [Thanh cong] Da thu hoach pha le! Khoang trong kho: "
                      << currentOre << " -> " << oreMoi << "/" << maxCargo << "\n";
            return oreMoi;
        } else {
            std::cout << "-> [Tu choi] Khong du dieu kien khai thac (HP = "
                      << hp << ", Kho = " << currentOre << "/" << maxCargo << ").\n";
            return currentOre;
        }
    } else if (command == cmdAction2) {
        std::cout << "-> [Rut lui] Whaleon ha do cao ve ben an toan. Bao toan "
                  << currentOre << " vien pha le!\n";
        return currentOre;
    } else {
        std::cout << "-> [Loi lenh] Lenh '" << command
                  << "' khong ton tai! Trang thai giu nguyen.\n";
        return currentOre;
    }
}

int main() {
    const int maxCargo{10};
    int hp{80};
    int crystalOre{4};

    std::cout << "=== BANG DIEU HANH CHUYEN DI WHALEON (POA12) ===\n";
    std::cout << "Trang thai dau: HP = " << hp
              << " | Khoang pha le = " << crystalOre << "/" << maxCargo << "\n\n";

    // 1. Nhap ten chuyen di (giu duoc khoang trang nho std::getline)
    std::cout << "Nhap ten chuyen di (vi du: Dao May Bao Tu 04): ";
    std::string expeditionName{};
    std::getline(std::cin, expeditionName);

    if (expeditionName.empty()) {
        expeditionName = "Chuyen bay chua dat ten";
    }

    std::cout << "\n[ Khoi hanh ] Chuyen di: \"" << expeditionName
              << "\" (do dai: " << expeditionName.length() << " byte)\n";

    // 2. Nhap mot lenh dang chu
    std::cout << "Nhap lenh hanh dong (hien tai ho tro: mine / retreat): ";
    std::string playerCommand{};
    std::getline(std::cin, playerCommand);

    // 3. Goi ham xu ly va nhan lai so khoang moi (Bai 010 & 011)
    crystalOre = xuLyLenhChuyenDi(playerCommand, hp, crystalOre, maxCargo);

    std::cout << "\n=== TONG KET SAU 1 BUOC ===\n";
    std::cout << "Chuyen di : " << expeditionName << "\n";
    std::cout << "HP        : " << hp << "\n";
    std::cout << "Khoang    : " << crystalOre << "/" << maxCargo << "\n";

    return 0;
}
