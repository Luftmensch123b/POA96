// POA12 - Vi du 1: 4 mau xu ly std::string pho bien trong Gameplay
// Pham vi kien thuc: Bai 001 -> 012 (C++20, chua dung vong lap hay reference)

#include <iostream>
#include <string>

// [Lien he Bai 010 & 011] Ham nhan std::string theo GIA TRI (ban sao cuc bo)
// Kiem tra ten chuyen di truoc khi hien len khung UI HUD
std::string chuanHoaTenChuyenDi(std::string rawName, int maxHudChars) {
    if (rawName.empty()) {
        return "Chuyen bay vo danh";
    }

    const int doDai{static_cast<int>(rawName.length())};
    if (doDai > maxHudChars) {
        return "[QUA DAI CHO HUD] " + rawName;
    }

    // Gan lai tren ban sao rawName, bien goc ben ngoai main() KHONG doi
    rawName = "Luftmensch :: " + rawName;
    return rawName;
}

// Mau tra ma hieu thiet bi dua tren ten ban thiet ke (in-game blueprint)
int layChiPhiPhaLeTheoTen(std::string blueprintId) {
    const std::string bpGiapDien{"bp_storm_shield"};
    const std::string bpMuiKhoan{"bp_crystal_drill"};

    if (blueprintId.empty()) {
        return -1; // Chua chon ban thiet ke
    } else if (blueprintId == bpGiapDien) {
        return 25;
    } else if (blueprintId == bpMuiKhoan) {
        return 40;
    } else {
        return 0; // Ma la khong ton tai trong danh muc
    }
}

int main() {
    std::cout << "=== MAU 1: KHOI TAO {}, .empty() VA .length() ===\n";
    std::string tenRong{};
    std::string tenHopLe{"Dao May Tinh Dien 07"};
    const int gioiHanHud{24};

    std::cout << "Ten rong ban dau co empty khong? " << tenRong.empty() << "\n";
    std::cout << "Do dai '" << tenHopLe << "': " << tenHopLe.length() << " byte\n";

    std::string nhanHud1{chuanHoaTenChuyenDi(tenRong, gioiHanHud)};
    std::string nhanHud2{chuanHoaTenChuyenDi(tenHopLe, gioiHanHud)};
    std::cout << "HUD 1: " << nhanHud1 << "\n";
    std::cout << "HUD 2: " << nhanHud2 << "\n";
    std::cout << "Bien tenHopLe goc van giu nguyen: '" << tenHopLe << "'\n\n";

    std::cout << "=== MAU 2: NOI CHUOI (+ va +=) XAY COMBAT LOG ===\n";
    const std::string tenCaVoi{"Whaleon-Aether"};
    // Chu y: Phai co it nhat 1 ve la std::string ngay phep + dau tien!
    std::string nhatKy{std::string{"[Khoi hanh] "} + tenCaVoi + " roi ben cang.\n"};
    nhatKy += "[Canh bao] Phat hien bao dien tich o tang may thap!\n";
    nhatKy += "[He thong] Kich hoat luoi chan pha le.\n";
    std::cout << nhatKy << "\n";

    std::cout << "=== MAU 3: BAY SO SANH CHUOI (HOA/THUONG & THU TU TU DIEN) ===\n";
    std::string lenhNhap{"Mine"};
    std::string lenhChuan{"mine"};
    std::cout << "'Mine' == 'mine' ? " << (lenhNhap == lenhChuan) << " (0 = khac nhau!)\n";

    std::string khuA{"Zone10"};
    std::string khuB{"Zone2"};
    // So sanh tung ky tu tu trai sang phai: '1' < '2' nen "Zone10" < "Zone2" la TRUE!
    std::cout << "'Zone10' < 'Zone2' ? " << (khuA < khuB)
              << " (1 = true! Vi ky tu '1' dung truoc '2', KHONG so theo gia tri so 10 va 2)\n\n";

    std::cout << "=== MAU 4: TRA CUU THEO MA DINH DANH CHUOI ===\n";
    std::cout << "Chi phi ''                 : " << layChiPhiPhaLeTheoTen("") << "\n";
    std::cout << "Chi phi 'bp_storm_shield'  : " << layChiPhiPhaLeTheoTen("bp_storm_shield") << "\n";
    std::cout << "Chi phi 'bp_storm_shield ' : " << layChiPhiPhaLeTheoTen("bp_storm_shield ")
              << " (du 1 dau cach cuoi -> khong khop!)\n";

    return 0;
}
