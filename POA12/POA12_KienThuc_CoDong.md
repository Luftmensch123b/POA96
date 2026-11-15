# Sổ tay cô đọng POA12 — `std::string` và `std::getline`

**Phạm vi bài:** Bài 012 (`Nền dùng được`) | **Dùng lại trực tiếp:** Bài 007 (`if/else`), Bài 010–011 (hàm & truyền bằng giá trị)

---

## 1. Bảng tra nhanh cú pháp

| Mục đích | Cú pháp chuẩn C++20 | Ghi chú quan trọng |
|---|---|---|
| Thư viện bắt buộc | `#include <string>` | Đi kèm `#include <iostream>` khi in/nhập |
| Tạo chuỗi rỗng | `std::string cmd{};` | Mặc định là `""` (0 ký tự), an toàn ngay khi tạo |
| Tạo chuỗi có sẵn chữ | `std::string name{"Whaleon"};` | Luôn dùng nháy kép `"..."` (nháy đơn `'...'` là `char`) |
| Kiểm tra chuỗi rỗng | `cmd.empty()` | Trả về `bool` (`true` nếu `""`, `false` nếu có ≥ 1 ký tự kể cả dấu cách) |
| Lấy độ dài chuỗi | `name.length()` hoặc `name.size()` | Trả về số **byte** kiểu không âm (`std::size_t`); ép về `int` bằng `static_cast<int>(name.length())` khi cần |
| Nối chuỗi | `a + b` hoặc `a += b` | Ít nhất **1 trong 2 vế** của phép `+` đang tính phải là `std::string` |
| So sánh bằng / khác | `a == b`, `a != b` | So khớp chính xác từng ký tự (phân biệt chữ HOA/thường và khoảng trắng thừa) |
| So sánh thứ tự | `a < b`, `a > b` | So theo **thứ tự bảng mã từ trái sang phải** (`"Zone10" < "Zone2"` là `true`), không so giá trị số! |
| Đọc trọn 1 dòng | `std::getline(std::cin, line);` | Giữ được khoảng trắng ở giữa; tự tiêu thụ và bỏ ký tự `\n` cuối dòng |

---

## 2. Sơ đồ luồng `std::getline` với vùng đệm bàn phím (Input Buffer)

```text
Người chơi gõ:   Đ ả o _ M â y [Enter]
Vùng đệm có:    [Đ][ả][o][ ][M][â][y][\n]
                                       │
std::getline(std::cin, name) chạy:     ▼
  1. Đọc từ đầu tới trước '\n'  ──► name = "Đảo Mây" (không dính '\n')
  2. Rút bỏ luôn '\n' khỏi đệm  ──► Vùng đệm sạch sẽ []
```

- **Nếu người chơi bấm thẳng `Enter`:** vùng đệm chỉ có `[\n]` → `getline` rút bỏ `\n`, trả `name = ""` (`name.empty() == true`), chương trình chạy tiếp bình thường.
- **Nếu người chơi gõ `"   "` rồi `Enter`:** `name = "   "` → `name.empty()` là **`false`** (vì có 3 ký tự khoảng trắng).

---

## 3. Năm lỗi tiềm ẩn hay gặp khi làm Gameplay

1. **Cộng hai chuỗi thô trực tiếp:** `"Khu " + "A"` báo lỗi compile vì cả hai đều là `const char[]`, chưa phải `std::string`. Sửa: `std::string{"Khu "} + "A"`.
2. **Dùng chuỗi để so lớn/bé cho con số:** `"9" > "100"` ra `true`! Mọi đại lượng tính toán (HP, khoáng, cấp độ) phải lưu bằng `int`/`double`, không lưu bằng `std::string`.
3. **Dư khoảng trắng vô hình khi gõ lệnh:** `"mine "` khác `"mine"`.
4. **Gõ sai chuỗi lệnh rải rác nhiều nơi (Magic Strings):** đặt `const std::string cmdMine{"mine"};` ở một chỗ để dùng thống nhất.
5. **Gộp chung lệnh rỗng, lệnh hợp lệ và lệnh lạ:** luôn tách rõ `if (cmd.empty())` → `else if (cmd == ...)` → `else` (báo lệnh không tồn tại, giữ nguyên dữ liệu game).

---

## 4. Lệnh biên dịch & chạy nhanh trên máy (MSVC Developer Command Prompt)

```powershell
cmd /c "\"D:\Whaleon\Visual Studio\Common7\Tools\VsDevCmd.bat\" -no_logo && cl /nologo /std:c++20 /EHsc /W4 /utf-8 POA12\vd1_mau_code_gameplay.cpp /Fe:POA12\vd1.exe && POA12\vd1.exe"
```
