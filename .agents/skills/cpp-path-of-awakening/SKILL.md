---
name: cpp-path-of-awakening
description: Phương pháp dạy và tiến trình lộ trình học C++ đến Unreal Engine (Path Of Awakening, Lý luận cấp cao C++). Kích hoạt khi người dùng học C++, hỏi lý thuyết/bài tập C++ hoặc tiếp tục lộ trình Path Of Awakening.
---

# Học C++: Path Of Awakening

Lộ trình 96 bài từ nền tảng C++ đến Unreal Engine, hướng tới một bản demo Whaleon có thể chơi được và được phát triển cùng AI.

## Phương pháp dạy & Nguyên tắc hỗ trợ

- Cách dạy: **bản chất → lần theo luồng chạy/dữ liệu → ví dụ code nhỏ → trường hợp biên → kiểm chứng**.
- Giải thích thuật ngữ mới; bám sát phạm vi bài hiện tại. Chỉ bổ sung kiến thức tiên quyết khi cần.
- Người dùng muốn hiểu sâu, không học thuộc cú pháp hoặc bị ép tự gõ lại toàn bộ code do AI tạo.
- AI có thể hỗ trợ kiến trúc, triển khai, debug, tạo tài nguyên và refactor. Người dùng cần hiểu đầu vào, trạng thái thay đổi, đầu ra và ít nhất một trường hợp lỗi liên quan.
- Ưu tiên cách khởi tạo thống nhất bằng `{}` khi phù hợp.
- Không tự chuyển bài hoặc giao bài tập khi người dùng chỉ yêu cầu giải thích.

## Tổ chức phiên học & Tiến trình hiện tại

- Bài học chính nằm trong các phiên `Path Of Awakening Lv.x`; câu hỏi lý luận bổ trợ nằm trong phiên **Lý luận cấp cao C++**.
- Đã xác nhận hoàn thành: **bài 001 đến 011 (001–011)**.
- Bài **012 (`std::string` và `std::getline`)** đã được yêu cầu học nhưng chưa có xác nhận hoàn thành.
- Khi được yêu cầu tiếp tục, bắt đầu từ **bài 012**; không học lại từ đầu và không tự đánh dấu đã hoàn thành.

### Tài nguyên & Ghi chú phiên Lý luận cấp cao C++ (Bài 012)

- Tài nguyên Bài 012 hiện có trong `POA12/`: giáo trình `POA12_GiaoTrinh_ChuyenSau.md`, sổ tay `POA12_KienThuc_CoDong.md` và 3 file ví dụ `vd1_mau_code_gameplay.cpp`, `vd2_thuc_hanh_chuyen_di.cpp`, `vd3_expedition_console_chuan.cpp`.
- Các điểm lý luận đã giải quyết ở phiên **Lý luận cấp cao C++** (Bài 012):
  - **Bản chất `std::to_string(x)`:** chuyển giá trị số (`int`, `double`...) sang đối tượng `std::string` để ghép chuỗi lưu log/biến, không làm đổi biến gốc; không cần dùng khi chỉ in trực tiếp bằng `std::cout << x`.
  - **Quy tắc nối chuỗi bằng `+`:** Cả 2 vế phải là văn bản (mọi biến số `int` phải bọc bằng `std::to_string()` trước, vì `std::string + int` sẽ lỗi biên dịch còn `"..." + int` dính bẫy dịch con trỏ), và ít nhất 1 trong 2 vế ở phép `+` đầu tiên (tính từ trái sang phải) phải có kiểu `std::string`.
