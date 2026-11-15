# PATH OF AWAKENING — BÀI 012: CHUỖI VÀ MỘT DÒNG NHẬP
**`std::string` · `std::getline` · Xử lý văn bản · Kiến trúc bộ nhận lệnh Gameplay (C++20)**

- **Vị trí trong lộ trình POA96:** Phần 02 — Hàm và dòng điều khiển (`Nền dùng được`)
- **Kiến thức tiền đề dùng lại:** Bài 004 (`{}` khởi tạo & phép gán), Bài 005 (`char`, `const`), Bài 006 (toán tử, `static_cast`), Bài 007 (`==`, `!=`, `<`, `if / else if / else`), Bài 009 (`&&`, `||`, `!`, luật phủ định), Bài 010–011 (hàm, `scope`, *shadowing*, truyền bằng giá trị).
- **Đích cần đạt của bài:** Làm chủ đối tượng `std::string` (khởi tạo, độ dài, kiểm rỗng, nối, so sánh); hiểu tận gốc cơ chế đọc dòng của `std::getline` trên luồng nhập; tách biệt rạch ròi giữa *hợp lệ về kiểu C++* và *hợp lệ theo luật gameplay*; xây dựng bộ nhận và phân loại lệnh văn bản an toàn đầu tiên cho Whaleon.

---

## Bản đồ bài giảng

| Phần | Trọng tâm kiến thức & kỹ thuật | Mục |
|---|---|---|
| **Phần I** | Bản chất đối tượng `std::string`, vùng nhớ, `scope` và truyền tham số | `01 – 07` |
| **Phần II** | Thao tác nội dung chuỗi, bảng mã UTF-8 và các bẫy kiểu dữ liệu | `08 – 17` |
| **Phần III** | Giải phẫu `std::getline`, vùng đệm luồng nhập (`stream`) và trạng thái đọc | `18 – 24` |
| **Phần IV** | Kiến trúc 3 tầng xử lý lệnh & 4 mẫu cấu trúc Gameplay thực chiến | `25 – 30` |
| **Phần V** | Whaleon Lab: Từ hàm diễn giải thuần đến tác động trạng thái Gameplay | `31 – 34` |
| **Phần VI** | Hệ thống 8 bài tập truy vết tư duy, AI Code Review & Đổi luật | `Bài A – H` |
| **Phần VII** | Tổng kết 6 nguyên lý cốt lõi & Checklist hoàn thành Bài 012 | `Checklist` |

---

# PHẦN I. BẢN CHẤT CỦA `std::string` & CƠ CHẾ QUẢN LÝ DỮ LIỆU

### 01. Bước ngoặt của Bài 012: Mở cánh cổng giao tiếp với người chơi
Từ Bài 001 đến Bài 011, mọi chương trình anh viết đều vận hành như một "hộp kín": mọi con số (`hp`, `ore`, `maxCargo`) được gán sẵn trong mã nguồn, bấm chạy là máy tính một mạch đến dòng cuối cùng rồi tắt.

Từ Bài 012, chương trình lần đầu tiên **dừng lại chờ người chơi lên tiếng** bằng văn bản: đặt tên chuyến thám hiểm, gõ lệnh điều hành tàu, nhập mã bản thiết kế.
Ở Bài 005, anh đã có kiểu `char`:
```cpp
char rankSymbol{'S'}; // Chỉ chứa được đúng 1 đơn vị ký tự 1 byte
```
Nhưng thế giới game không giao tiếp bằng từng chữ cái rời rạc. Tên một vùng trời trong Whaleon (`"Vanh Dai Bao Tu 04"`), mã một vật phẩm RPG (`"wpn_crystal_spear"`), hay câu lệnh gõ vào bảng điều khiển (`"deploy shield"`) đều là **dãy nhiều ký tự có độ dài co giãn tùy lúc chạy**. Để quản lý dữ liệu dạng đó, thư viện chuẩn C++ cung cấp kiểu đối tượng `std::string`:

```cpp
#include <string>

std::string expeditionName{"Crystal Whale Expedition"};
```
- `#include <string>`: nạp định nghĩa chính thức của `std::string` (không dựa vào việc `<iostream>` đôi khi kéo ké theo `<string>` trên một số trình biên dịch — luôn `#include <string>` tường minh!).
- `std::string`: kiểu đối tượng chuỗi văn bản thuộc không gian tên chuẩn `std::`.
- `{}`: tiếp tục dùng khởi tạo ngoặc nhọn thống nhất như từ Bài 004.

---

### 02. Giải phẫu 3 thực thể rất dễ đánh đồng: `'C'` vs `"C"` vs `std::string{"C"}`
Nhìn bằng mắt người thì cả ba đều biểu diễn chữ `C`, nhưng trong trình biên dịch C++ chúng là **ba thế giới khác hẳn nhau**:

| Biểu thức | Kiểu dữ liệu thật | Bản chất trong bộ nhớ | Khả năng thao tác |
|---|---|---|---|
| `'C'` | `char` | **1 byte** lưu mã số ASCII `67` của ký tự `C`. | Chỉ giữ đúng 1 ký tự cố định. |
| `"C"` | `const char[2]` *(string literal)* | Mảng tĩnh **2 byte** nằm trong vùng nhớ chỉ đọc của chương trình: byte đầu là `'C'`, byte thứ hai là ký tự kết thúc ngầm **`'\0'` (null terminator)**. | Đóng băng lúc biên dịch; không tự co giãn, không cộng trực tiếp hai `"..."` với nhau được. |
| `std::string{"C"}` | `std::string` | Một **đối tượng C++ năng động** sở hữu nội dung `'C'`, ghi nhớ độ dài hiện tại là `1`. | Nối thêm chữ, xóa chữ, thay thế nội dung, so sánh trực tiếp lúc chương trình đang chạy. |

**Ba quy tắc tránh lỗi ngay từ đầu:**
1. **Không dùng nháy đơn cho chuỗi nhiều ký tự:** Viết `std::string name{'Whaleon'};` là sai hoàn toàn (`'Whaleon'` là *multi-character literal* kiểu số nguyên `int` mang giá trị phụ thuộc trình biên dịch, không phải chuỗi văn bản!).
2. **Không đem `std::string` đi so `==` trực tiếp với `char`:**
   ```cpp
   std::string cmd{"m"};
   // if (cmd == 'm')  // ❌ LỖI BIÊN DỊCH: không thể so std::string với char!
   if (cmd == "m") {}  // ✅ HỢP LỆ: so với chuỗi 1 ký tự
   ```
3. Đừng hình dung `std::string` chỉ là "vài biến `char` đặt cạnh nhau". Nó là một **đối tượng quản lý trọn gói** cả vùng chứa ký tự, kích thước hiện có và bộ nhớ đi kèm.

---

### 03. Bên dưới nắp máy: `std::string` quản lý bộ nhớ và tối ưu tốc độ ra sao?
Khi anh khai báo `std::string cmd{"MINE"};`, đối tượng `cmd` chịu trách nhiệm quản lý 4 phần tử `char` nằm liên tiếp nhau:

```text
Đối tượng std::string cmd (biết độ dài hiện tại = 4)
   └──► Dãy ký tự liên tiếp:  [ 'M' | 'I' | 'N' | 'E' ]
         Chỉ số vị trí:          0     1     2     3
```

Ba cơ chế vận hành cốt lõi bên dưới mà người học sâu cần nắm:
1. **Tự động mở rộng khi nối dài:** Khi anh nối thêm nhiều câu chữ vượt quá sức chứa hiện tại, `std::string` tự động xin cấp phát một vùng nhớ mới rộng hơn (trên *heap*), sao chép các ký tự sang nhà mới và trả lại vùng nhớ cũ.
2. **Tự động giải phóng khi hết thời gian sống (*Lifetime* — Bài 011):** Ngay khoảnh khắc hàm kết thúc hoặc ra khỏi khối `{}`, đối tượng `std::string` tự thu hồi toàn bộ bộ nhớ nó đang nắm giữ. Anh không phải dọn bộ nhớ thủ công như thời ngôn ngữ C.
3. **Tối ưu hóa chuỗi ngắn — *Small String Optimization (SSO)*:**
   - Xin vùng nhớ ngoài *heap* vốn tốn thời gian hơn làm việc trên *stack*.
   - Vì trong thực tế (đặc biệt là trong game), phần lớn chuỗi đều khá ngắn (`"mine"`, `"status"`, `"ore_crystal"`, `"Rank A"`), các trình biên dịch C++ hiện đại (MSVC trên Visual Studio, GCC, Clang) đều thiết kế sẵn một **ngăn đệm nhỏ nằm ngay bên trong đối tượng `std::string`** (thường chứa vừa khoảng 15 đến 22 byte tùy trình biên dịch).
   - Nhờ SSO, các chuỗi lệnh ngắn nằm gọn ngay trên *stack*, **không tốn bất kỳ lần cấp phát heap nào**! *(Lưu ý chuẩn xác: SSO là kỹ thuật tối ưu phổ biến của các thư viện thực tế, chuẩn C++ không ép cứng con số bao nhiêu ký tự).*

---

### 04. Chuỗi rỗng `""`: Tách bạch "Hợp lệ về kiểu C++" với "Hợp lệ theo luật Gameplay"
```cpp
std::string emptyA{};     // Chuỗi rỗng "", độ dài = 0
std::string emptyB{""};   // Cũng là chuỗi rỗng "", độ dài = 0
```
- **Điểm an toàn hơn `int` (Bài 004):** Một biến `int hp;` bỏ quên `{}` sẽ mang giá trị rác không đoán trước được; ngược lại `std::string s;` dù viết có `{}` hay không vẫn tự khởi tạo thành chuỗi rỗng `""` hợp lệ. Dù vậy, ta luôn viết `std::string s{};` để giữ kỷ luật khởi tạo đồng nhất.
- **Tư duy thiết kế cốt lõi (nối tiếp Bài 009):**
  Chuỗi rỗng `""` là một giá trị `std::string` **hoàn toàn hợp lệ đối với ngôn ngữ C++**. Còn việc nó được chấp nhận hay bị từ chối trong game hoàn toàn phụ thuộc vào **luật nghiệp vụ (business/gameplay rule)** của từng trường dữ liệu:

| Trường dữ liệu trong Game | Chuỗi rỗng `""` hợp lệ không? | Lý do thiết kế |
|---|---|---|
| Tên chuyến thám hiểm / Tên nhân vật | **Không** | Hiển thị lên HUD và danh sách file save sẽ bị trống vô nghĩa. |
| Dòng gõ lệnh điều khiển (`command`) | **Không thực thi hành động** | Người chơi chỉ vừa bấm Enter trống; cần nhắc nhẹ, không đổi trạng thái. |
| Ô ghi chú cá nhân trên bản đồ | **Hợp lệ** | Người chơi có quyền ghim mốc tọa độ mà không cần viết lời chú thích. |
| Tiền tố danh hiệu (`titlePrefix`) | **Hợp lệ** | Nhân vật đầu game chưa mở khóa danh hiệu nào thì tiền tố là `""`. |

---

### 05. Thay đổi nội dung (`Mutation`), Phạm vi (`Scope`) và Che khuất tên (`Shadowing`)
Nội dung của một biến `std::string` (không gắn `const`) có thể thay đổi nhiều lần trong suốt vòng đời của nó:
```cpp
std::string whaleState{"Docked"}; // Khởi tạo đối tượng
whaleState = "Ascending";         // Thay đổi nội dung của CHÍNH đối tượng whaleState đó
```
Hãy đối chiếu với tình huống mở khối `{}` con (kiến thức Bài 011):
```cpp
std::string whaleState{"Docked"};
{
    std::string whaleState{"Ascending"}; // Khai báo ĐỐI TƯỢNG MỚI trùng tên (Shadowing)!
    // Trong khối này, tên whaleState chỉ nhìn thấy đối tượng bên trong ("Ascending")
} // Đối tượng bên trong bị hủy tại đây!

// Ra ngoài khối: whaleState ban đầu vẫn nguyên vẹn là "Docked"!
```
Quy tắc *scope*, *lifetime* và *shadowing* ở Bài 011 không hề dành riêng cho `int` — nó chi phối mọi đối tượng trong C++, bao gồm `std::string`.

---

### 06. Truyền `std::string` vào hàm bằng giá trị (`Pass-by-Value` — Bài 010 & 011)
Khi tham số hàm có kiểu `std::string`, C++ truyền dữ liệu theo cơ chế **sao chép giá trị**:

```cpp
void TryMutateInside(std::string zoneName) {
    zoneName = "Deadly Storm Sector"; // Chỉ sửa bản sao cục bộ zoneName!
}

std::string AddDangerTag(std::string zoneName) {
    zoneName = "[DANGER] " + zoneName;
    return zoneName;                  // Trả kết quả mới ra ngoài
}
```

Lần theo trạng thái của hai đối tượng khi gọi từ `main()`:
```cpp
std::string currentZone{"Cloud Pier 02"};
TryMutateInside(currentZone);
// -> currentZone VẪN LÀ "Cloud Pier 02"

currentZone = AddDangerTag(currentZone);
// -> Nhờ phép gán '=' hứng giá trị trả về, currentZone mới đổi thành "[DANGER] Cloud Pier 02"
```

**Góc nhìn hiệu năng thực tế:**
- Với `int` (Bài 011), tạo bản sao chỉ chép đúng 4 byte — rẻ như không.
- Với `std::string`, tạo bản sao nghĩa là khởi tạo một đối tượng chuỗi mới và **chép toàn bộ dãy ký tự** sang. Nếu chuỗi là một cuốn Lore dài 20.000 chữ mà gọi lặp đi lặp lại mỗi khung hình, chi phí sao chép sẽ rất đáng kể.
- Ở Bài 012, ta chủ động dùng *pass-by-value* vì các chuỗi tên/lệnh đều rất ngắn (thậm chí hưởng lợi từ SSO ở Mục 03) và giữ đúng nền kiến thức đã học. Đến **Bài 021 (`const std::string&`)**, anh sẽ học cách cho hàm đọc thẳng chuỗi gốc mà không tốn 1 byte sao chép nào!

---

### 07. Hằng chuỗi `const std::string` — Lá chắn chống lỗi ngầm "Magic Strings"
Kết hợp `const` (Bài 005) với `std::string` giúp khóa cứng các từ khóa quy ước trong game:
```cpp
const std::string kCmdMine{"mine"};
const std::string kCmdStatus{"status"};
```
**Tại sao thói quen này cứu sống hàng giờ debug khi làm game lớn?**
- Giả sử anh không dùng hằng mà gõ trực tiếp `"retreat"` ở 6 nhánh kiểm tra khác nhau trong game. Chỉ cần 1 chỗ anh gõ trượt thành `"retrat"`, **trình biên dịch vẫn báo Build thành công 100%** (vì `"retrat"` là cú pháp C++ hợp lệ!). Lỗi này nằm im cho tới khi người chơi lâm nguy bấm rút lui mà nhân vật đứng trơ ra.
- Ngược lại, nếu dùng hằng `kCmdRetreat` và gõ nhầm thành `kCmdRetrat`, **trình biên dịch báo lỗi đỏ ngay tức khắc lúc build**! Biến lỗi logic ngầm lúc chạy thành lỗi biên dịch bắt được ngay là một trong những nguyên tắc vàng của kỹ thuật phần mềm C++.

---

# PHẦN II. LÀM VIỆC VỚI NỘI DUNG CHUỖI, UNICODE & CÁC BẪY KIỂU DỮ LIỆU

### 08. Độ dài chuỗi với `.size()` và `.length()` — Lời gọi hàm thành viên qua dấu chấm `.`
```cpp
#include <iostream>
#include <string>

std::string command{"mine"};
std::cout << command.size() << '\n';   // In ra: 4
std::cout << command.length() << '\n'; // In ra: 4
```
1. **Cú pháp dấu chấm `.`:** Ở Bài 010, anh gọi hàm độc lập kiểu `TinhKhoang(30, 5)`. Còn `command.size()` là **lời gọi hàm thành viên (*member function*)** — tức là yêu cầu chính đối tượng `command` báo cáo thông tin nội bộ của nó.
2. **`.size()` và `.length()` có khác gì nhau không?** Trên `std::string`, hai hàm này **giống hệt nhau tuyệt đối**, đều trả về số phần tử `char` hiện có trong chuỗi. (`length()` đọc thuận miệng khi nói về văn bản; `size()` là tên chuẩn thống nhất dùng chung cho mọi cấu trúc chứa dữ liệu sau này như `std::vector` ở Bài 017).
3. **Tốc độ hằng số `O(1)`:** Trong C++ hiện đại, `std::string` luôn lưu sẵn con số độ dài bên trong đối tượng, nên gọi `.size()` trả kết quả ngay lập tức chứ không phải chạy đếm từng ký tự từ đầu đến cuối như chuỗi C cổ điển!

---

### 09. Kiểm tra chuỗi rỗng bằng `.empty()` và bẫy "Chuỗi chỉ có khoảng trắng"
`text.empty()` trả về `bool`: `true` nếu không có phần tử nào (`size() == 0`), `false` nếu có từ 1 phần tử trở lên.
Luôn ưu tiên viết `if (command.empty())` thay vì `if (command.size() == 0)` vì nó diễn đạt trực diện câu hỏi thiết kế: *"Chuỗi này có rỗng không?"*.

Hãy nhìn kỹ bảng đối chiếu các trường hợp biên dưới đây:

| Nội dung chuỗi | `.size()` | `.empty()` | Giải thích bản chất |
|---|---|---|---|
| `""` | `0` | **`true`** | Chuỗi thực sự trống rỗng, 0 phần tử. |
| `"M"` | `1` | `false` | Có 1 ký tự chữ. |
| `" "` *(1 dấu cách)* | `1` | **`false`** | Dấu cách `' '` là một ký tự thật (mã ASCII `32`), **không phải rỗng**! |
| `"   "` *(3 dấu cách)* | `3` | **`false`** | Gồm 3 phần tử `' '` đàng hoàng! |
| `"\t"` *(1 phím Tab)* | `1` | **`false`** | Ký tự Tab (mã ASCII `9`) cũng là 1 phần tử! |

> **Cạm bẫy thực tế:** Nếu người chơi lỡ đè phím Space 3 cái rồi nhấn Enter, `.empty()` vẫn trả về **`false`**! Do đó: *"Không rỗng" chưa đồng nghĩa với "có nội dung chữ có nghĩa"*.

---

### 10. Vì sao `.size()` không đếm "số chữ mắt người nhìn thấy"? (Giải mã UTF-8 & Unicode)
Với tiếng Anh không dấu thuộc bảng ASCII (`"Whaleon"`), mỗi chữ cái được mã hóa bằng đúng 1 byte → `.size()` bằng `7`, trùng khít với 7 chữ trên màn hình.

Nhưng với tiếng Việt hoặc văn bản đa ngôn ngữ dùng bảng mã **UTF-8** (mã hóa độ dài biến thiên từ 1 đến 4 byte cho mỗi ký hiệu Unicode), `.size()` thực chất **đếm số byte lưu trữ (`char`)**, không đếm chữ cái hiển thị!

| Khái niệm | Ý nghĩa thực chất | Ví dụ minh họa |
|---|---|---|
| **Byte (`char`)** | Đơn vị 8-bit mà `std::string` lưu trữ và `.size()` đếm. | Chuỗi UTF-8 `"Cá"` gồm `'C'` (1 byte) + `'á'` dựng sẵn (2 byte) → `.size()` trả về **`3`**! Chuỗi `"Đảo"` chiếm `2 + 3 + 1 =` **`6` byte**! |
| **Code point** | Một con số định danh ký hiệu trong bảng chuẩn Unicode quốc tế. | `'C'` là `U+0043`; `'á'` dựng sẵn là `U+00E1`. |
| **Grapheme cluster** | Một "con chữ hoàn chỉnh" theo cảm nhận thị giác của con người. | Mắt người đọc `"Đảo"` luôn đếm là **3 chữ**. |
| **Biểu diễn tương đương (NFC vs NFD)** | Unicode cho phép biểu diễn chữ `"á"` theo 2 cách khác nhau: **1 mã dựng sẵn** (`á` — chuẩn NFC) HOẶC **chữ cơ sở `a` ghép với dấu sắc rời `◌́`** (2 code point — chuẩn NFD)! | Hai chuỗi in lên màn hình giống hệt nhau từng pixel là `"Cá Voi"`, nhưng một bên `.size()` bằng `7`, bên kia bằng `8`, và đem so `a == b` cho kết quả **`false`**! |

**Định vị rõ cho Bài 012:**
- Anh **không cần** tự viết thuật toán xử lý Unicode lúc này.
- Điều cốt tử cần nhớ: khi làm việc trên Windows với Visual Studio, nếu thấy `.size()` của chữ tiếng Việt lớn hơn số chữ cái, hoặc thấy console in tiếng Việt bị lỗi ký tự lạ, đừng vội nghi ngờ `std::string` bị hỏng — đó là câu chuyện của **encoding (bảng mã file nguồn và bảng mã cửa sổ console)**, và vì `std::string` làm việc ở mức byte thô! *(Đây cũng chính là lý do đến Bài 052 trên Unreal Engine, engine cung cấp kiểu `FText` chuyên trách cho văn bản hiển thị đa ngôn ngữ).*

---

### 11. Cạm bẫy kiểu không dấu: Thảm họa `command.size() - 1` khi chuỗi rỗng!
Hàm `.size()` **không trả về kiểu `int`**, mà trả về `std::string::size_type` (tương đương `std::size_t` — **số nguyên KHÔNG DẤU**, chỉ biểu diễn số `>= 0`, không bao giờ có số âm!).

Chuyện gì xảy ra nếu anh lấy `command.size() - 1` khi `command` đang là chuỗi rỗng `""`?
1. `command.size()` bằng `0` (kiểu không dấu).
2. Phép trừ `0 - 1` thực hiện theo luật số nguyên không dấu: vì không thể biểu diễn `-1`, kết quả **cuộn vòng ngược (wrap-around / underflow)** lên giá trị dương cực đại! Trên chương trình 64-bit, `0 - 1` biến thành **`18,446,744,073,709,551,615`**!
3. Nếu anh dùng con số khổng lồ đó làm giới hạn duyệt hoặc truy cập vùng nhớ, chương trình sẽ sập ngay lập tức!

**Hai thói quen phòng vệ an toàn:**
- **Cách 1 (Chắn rỗng trước khi trừ):** Chỉ tính `size() - 1` bên trong nhánh `if (!command.empty())`.
- **Cách 2 (Chuyển kiểu chủ động từ Bài 006 khi so với ngưỡng nhỏ):** Khi kiểm tra giới hạn ngắn như độ dài tên nhân vật `const int kMaxNameBytes{24};`, dùng `static_cast<int>(name.size()) > kMaxNameBytes` để tránh trộn lẫn số có dấu và số không dấu (trình biên dịch bật `/W4` cũng sẽ cảnh báo nếu trộn ngầm hai kiểu này).

---

### 12. Ghép chuỗi bằng `+` và cơ chế Nạp chồng toán tử (`Operator Overloading`)
```cpp
int totalCrystal{10 + 5};                      // Kết quả: số 15
std::string badgeCode{std::string{"10"} + "5"}; // Kết quả: chuỗi "105"
```
Cùng một dấu `+`, nhưng trình biên dịch dựa vào **kiểu của các toán hạng** để quyết định thực hiện phép cộng đại số hay phép nối đuôi văn bản. Cơ chế một toán tử mang hành vi phù hợp theo từng kiểu đối tượng được gọi là **Operator Overloading**.

*(Lưu ý ranh giới kiểu: Trong C++, không viết trực tiếp `"HP: " + hp` khi `hp` là `int`. Nếu chỉ cần in ra màn hình, hãy nối tiếp bằng `<<` trên luồng xuất: `std::cout << "HP: " << hp << '\n';` — vừa nhanh vừa không tốn bộ nhớ tạo chuỗi tạm).*

---

### 13. Phân biệt 4 phép biến đổi chuỗi: `+`, `+=`, `=`, và `.clear()`
```cpp
std::string ship{"Whale"};
std::string modelName{ship + "on"}; // 1. Tạo chuỗi mới "Whaleon"; ship gốc VẪN LÀ "Whale"!

ship += "-04";                      // 2. Nối bồi thêm vào đuôi chính đối tượng ship -> "Whale-04"
ship = "Ark-Prime";                 // 3. Xóa sạch nội dung cũ của ship, thay bằng "Ark-Prime"
ship.clear();                       // 4. Đưa ship về chuỗi rỗng "" (ship.empty() == true)
```

| Thao tác | Tác động lên biến `a` | Khi nào dùng trong Gameplay? |
|---|---|---|
| `a + b` | **Không đổi `a`** (tạo chuỗi kết quả mới) | Tạo nhãn hiển thị tạm thời (`"Run: " + runName`) mà không làm bẩn tên gốc. |
| `a += b` | **Bồi thêm `b` vào đuôi `a`** | Tích lũy từng dòng diễn biến vào Nhật ký chiến đấu (*Combat Log*) qua nhiều bước. |
| `a = b` | **Ghi đè toàn bộ nội dung `a`** | Đổi trạng thái hiện tại, đổi tên mới hoàn toàn. |
| `a.clear()` | **Xóa rỗng `a` về `""`** | Xóa sạch ô nhập chat hoặc làm trống bộ đệm log khi sang lượt chơi mới. |

---

### 14. Bẫy biên dịch khi ghép hai String Literal & Thứ tự tính từ trái sang phải
Đây là lỗi hầu như ai mới dùng `std::string` cũng gặp ít nhất một lần:
```cpp
const std::string whale{"Aether"};

// ❌ LỖI BIÊN DỊCH 1: Hai mảng ký tự thô ("const char[]") không có phép + với nhau!
std::string bad1{"Sector " + "Alpha"};

// ❌ LỖI BIÊN DỊCH 2 (Rất hiểm!): Dù cuối dòng có biến std::string 'whale', VẪN LỖI!
// Lý do: Phép + kết hợp từ TRÁI SANG PHẢI -> máy tính ("Sector " + "Alpha - ") trước tiên,
// mà cả hai vế đầu tiên này đều là string literal thô!
std::string bad2{"Sector " + "Alpha - " + whale};

// ✅ CÁCH ĐÚNG 1: Biến toán hạng đầu tiên thành std::string
// Lúc này (std::string + literal) -> trả về std::string tạm -> cộng tiếp với whale hợp lệ!
std::string good1{std::string{"Sector "} + "Alpha - " + whale};

// ✅ CÁCH ĐÚNG 2: Hai string literal đặt cạnh nhau KHÔNG CÓ DẤU +
// Trình biên dịch tự gộp chúng thành một literal duy nhất "Sector Alpha" ngay lúc dịch mã nguồn!
std::string good2{"Sector " "Alpha"};
```
Hiểu rõ sự khác biệt giữa `good1` (ghép đối tượng lúc chương trình chạy) và `good2` (gộp hằng văn bản lúc dịch mã nguồn) giúp anh đọc code C++ chuyên nghiệp cực kỳ tự tin.

---

### 15. So sánh bằng `==` và khác `!=` — Nghiêm ngặt đến từng ký tự
`a == b` trả về `true` chỉ khi cả hai chuỗi **cùng số lượng byte và giống hệt nhau ở từng vị trí một**:
```cpp
std::string input{"Mine"};

input == "mine";  // false ('M' mã 77 != 'm' mã 109)
input == "Mine "; // false (độ dài 4 != độ dài 5 do dư khoảng trắng cuối)
input == "Mine";  // true
```
Trong Bài 012, ta cố ý chọn **so sánh khớp chính xác** làm quy tắc thiết kế: hành vi rõ ràng 100%, không đoán mò ý người dùng, rất dễ chứng minh tính đúng đắn.

---

### 16. So sánh thứ tự `<`, `>`, `<=`, `>=` và Cạm bẫy "Con số mặc áo chuỗi"
`std::string` cho phép so sánh thứ tự theo **quy tắc từ điển dựa trên giá trị mã ký tự từ trái sang phải (*Lexicographical comparison*)**:
- So `"ore"` với `"stone"`: ký tự đầu `'o'` (`111`) nhỏ hơn `'s'` (`115`) → `"ore" < "stone"` là `true`.
- **Cạm bẫy chết người nếu lưu con số dạng chuỗi để so lớn/bé:**
  ```cpp
  std::string zoneA{"Zone10"};
  std::string zoneB{"Zone2"};
  bool isSmaller{zoneA < zoneB}; // KẾT QUẢ LÀ TRUE!!
  ```
  Tại sao `"Zone10"` lại "nhỏ hơn" `"Zone2"`? Vì 4 ký tự đầu `"Zone"` hòa nhau; tới vị trí thứ 5, máy đem ký tự `'1'` (mã 49) so với ký tự `'2'` (mã 50). Vì `'1' < '2'`, máy kết luận ngay `"Zone10" < "Zone2"` mà không thèm nhìn tới số `'0'` phía sau! Tương tự, `"9" > "100"` là **`true`**!
- Ngoài ra, so sánh `<` của `std::string` **không phải bảng chữ cái tiếng Việt chuẩn** (chữ có dấu mang mã số lớn nên `"Ánh"` lại xếp sau `"Zebra"`).
> **Chốt nguyên tắc:** Đại lượng dùng để đo đếm, tính sát thương, kiểm ngưỡng tầng, so cấp độ → bắt buộc dùng `int` / `double` (Bài 005–007). `std::string` chỉ dùng để định danh, hiển thị và khớp mẫu văn bản!

---

### 17. Mở rộng nhận biết: Truy cập ký tự đơn theo chỉ số `command[i]`
Nếu chuỗi có độ dài `N`, các phần tử ký tự được đánh số từ **`0` đến `N - 1`**:
```cpp
std::string cmd{"mine"};
// cmd[0] -> 'm' | cmd[1] -> 'i' | cmd[2] -> 'n' | cmd[3] -> 'e'
```
- **Luật an toàn tuyệt đối:** Chuỗi rỗng `""` có `size() == 0`, tức là **không có cả phần tử `0`**! Viết `cmd[0]` khi `cmd` đang rỗng sẽ đọc bừa vào vùng nhớ không hợp lệ (*Undefined Behavior*). Muốn lấy ký tự đầu tiên, bắt buộc phải chắn trước:
  ```cpp
  if (!cmd.empty()) {
      char firstLetter{cmd[0]};
      std::cout << firstLetter << '\n';
  }
  ```
*(Còn một hàm thành viên khác là `cmd.at(0)` có tự kiểm tra biên và ném ngoại lệ exception khi vượt giới hạn, nhưng cơ chế exception chưa thuộc phạm vi hiện tại; kỹ năng làm việc với chỉ số và kiểm biên sẽ được rèn chính thức ở **Bài 017 — `std::vector`**).*

---

# PHẦN III. GIẢI PHẪU `std::getline` & CƠ CHẾ LUỒNG NHẬP (`STREAM`)

### 18. Vì sao đọc văn bản trong game không thể chỉ đọc "tới khoảng trắng"?
Giả sử người chơi đặt tên chuyến bay là `"The Last Crystal Whale"` hoặc gõ câu lệnh 2 từ `"deploy shield"`. Nếu dùng cơ chế đọc tách từ theo khoảng trắng, chương trình chỉ nhặt được chữ `"The"` (hoặc `"deploy"`), chặt đứt phần còn lại và tệ hơn nữa: **phần bị cắt đứt vẫn nằm chờ trong bộ đệm bàn phím**, tự động tràn vào câu hỏi tiếp theo của chương trình!

Để nhận nguyên vẹn một dòng văn bản do người chơi nhập, công cụ chuẩn xác là `std::getline`.

---

### 19. Mô hình Luồng (`Stream`) và cú pháp `std::getline`
```cpp
#include <iostream>
#include <string>

int main() {
    std::string runName{};
    std::cout << "Enter run name: ";
    std::getline(std::cin, runName);
    std::cout << "Run: [" << runName << "]\n";
}
```
Giải nghĩa từng thành phần:
- **Stream (Luồng dữ liệu):** Hãy hình dung một đường ống một chiều dẫn dãy ký tự chảy tuần tự vào chương trình. Khi chơi trực tiếp trên cửa sổ console, `std::cin` nối với bàn phím; khi kiểm thử tự động, `std::cin` có thể được nối với một file kịch bản có sẵn mà code chương trình không cần sửa một chữ nào!
- `std::getline(std::cin, runName)`: hàm của `<string>` rút các ký tự từ luồng `std::cin` đưa vào biến `runName` cho đến khi chạm ký tự ngắt dòng mặc định là **`'\n'` (newline)**.

---

### 20. Truy vết chính xác khoảnh khắc người chơi nhấn phím `Enter` (`'\n'`)
Người chơi gõ `Crystal Whale` rồi nhấn `Enter`. Dữ liệu nằm trong đường ống `std::cin` lúc đó là:

```text
Trước khi đọc:  [C][r][y][s][t][a][l][ ][W][h][a][l][e][\n]
                                                         ▲
                                              Delimiter kết thúc dòng
```

Khi `std::getline(std::cin, runName)` thực thi, nó làm **đúng 3 việc liên tiếp**:
1. Đọc toàn bộ các ký tự đứng trước `'\n'` nạp vào `runName` (`"Crystal Whale"`).
2. **Tiêu thụ (rút bỏ) luôn ký tự `'\n'` ra khỏi đường ống `std::cin`** để đường ống sạch sẽ cho lần đọc kế tiếp.
3. **Không chép `'\n'` vào cuối `runName`**! Nhờ vậy `runName.size()` đúng bằng `13`, không bị dính ký tự xuống dòng vô hình ở đuôi.

---

### 21. `std::getline` giữ nguyên mọi khoảng trắng & Kỹ thuật soi bằng ngoặc vuông `[...]`
`std::getline` là người vận chuyển trung thực: người chơi gõ sao, nó giao đúng vậy — **không tự cắt khoảng trắng đầu, giữa hay cuối dòng**!
- Nếu người chơi gõ `"  Crystal Whale  "` → `runName` chứa đúng `"  Crystal Whale  "`.
- Nếu ở bước nhập lệnh người chơi gõ `"mine "` (vô tình chạm phím cách trước khi nhấn Enter) → `command` chứa `"mine "` → đem so `command == "mine"` chắc chắn ra **`false`**!

> **Mẹo thực chiến cực hay khi làm log / debug:** Khi in chuỗi người dùng nhập ra màn hình kiểm tra, hãy kẹp nó giữa cặp ngoặc vuông:
> `std::cout << '[' << command << "]\n";`
> In trơn `mine ` mắt thường nhìn y hệt `mine`; nhưng in `[mine ]` thì dấu cách thừa lộ nguyên hình trong tích tắc!

---

### 22. `std::getline` GHI ĐÈ chứ không nối đuôi vào nội dung cũ
```cpp
std::string runName{"Default Expedition"};
std::getline(std::cin, runName); // Người chơi gõ: Sky Island 09 [Enter]
```
Ngay khi đọc dòng mới, `std::getline` **xóa sạch `"Default Expedition"`** và thay bằng `"Sky Island 09"`. Thậm chí nếu người chơi chỉ bấm thẳng `Enter` trống, `"Default Expedition"` cũng bị xóa sạch để thay bằng chuỗi rỗng `""`!

---

### 23. Phân biệt cực kỳ quan trọng: "Dòng rỗng" khác hẳn "Đọc luồng thất bại (EOF)"
Rất nhiều người mới học gộp chung hai chuyện này làm một. Thực tế đây là hai tầng hoàn toàn khác nhau:

| Tình huống đầu vào | `std::getline(std::cin, cmd)` trả về | Nội dung `cmd` | Bản chất sự việc |
|---|---|---|---|
| Người chơi bấm thẳng `Enter` | **Thành công (`true`)** | `""` (`empty() == true`) | Đường ống vẫn mở; đọc thành công một dòng có 0 ký tự. |
| Người chơi gõ `mine` + `Enter` | **Thành công (`true`)** | `"mine"` | Đọc thành công dòng bình thường. |
| File test kết thúc ngay sau chữ `mine` (gặp EOF, không có `\n` cuối) | **Thành công (`true`)** cho dòng cuối đó | `"mine"` | Đọc nốt những ký tự cuối cùng trước khi cạn luồng. |
| Luồng đã đóng / cạn sạch ngay trước khi đọc được bất kỳ ký tự nào (bấm `Ctrl+Z` + Enter trên Windows, hoặc hết file input) | **Thất bại (`false`)** | Không có dòng mới hợp lệ | **EOF (End Of File) / Input Unavailable** — không còn dữ liệu để đọc! |

Vì vậy, trong chương trình chuẩn mực, ta tách rõ hai câu hỏi bảo vệ theo đúng thứ tự:
```cpp
// Câu hỏi 1 (Tầng đường ống): Thao tác đọc một dòng có thành công không?
if (!std::getline(std::cin, command)) {
    std::cout << "Input unavailable\n";
    return 0;
}

// Câu hỏi 2 (Tầng nội dung): Dòng vừa đọc thành công đó có bị bỏ trống không?
if (command.empty()) {
    std::cout << "Empty command\n";
}
```
*(Các cờ trạng thái bên trong stream như `eofbit`, `failbit` và cách khôi phục luồng sẽ được mổ xẻ toàn diện ngay ở **Bài 013**).*

---

### 24. Cảnh báo trước cạm bẫy kinh điển nhất C++: Trộn `std::cin >>` trước `std::getline`
Hãy nhìn đoạn code rất dễ gặp khi kết hợp đọc số với đọc dòng:
```cpp
int difficulty{};
std::string runName{};

std::cin >> difficulty;          // Người chơi gõ: 3 [Enter]
std::getline(std::cin, runName); // BỊ BỎ QUA NGAY LẬP TỨC, không chờ người chơi gõ tên!
```
**Lần theo đường ống để hiểu tận gốc nguyên nhân:**
1. Người chơi gõ `3` rồi `Enter` → đường ống có `[3][\n]`.
2. Phép đọc định dạng `std::cin >> difficulty` chỉ rút đúng con số `3` ra, **để mặc ký tự `'\n'` nằm lại đầu đường ống**!
3. Tới dòng kế tiếp, `std::getline(std::cin, runName)` nhìn vào đường ống thấy ngay ký tự `'\n'` nằm chình ình ở đầu → theo đúng hợp đồng Mục 20, nó rút `'\n'` bỏ đi và kết luận người chơi vừa nhập một dòng rỗng `""`!
4. **Kết luận:** `std::getline` không hề bị lỗi — nó làm đúng 100% nhiệm vụ đọc tới `'\n'` đầu tiên còn trong luồng! Trong Bài 012, ta dùng thuần `std::getline` cho toàn bộ các bước nhập nên không bao giờ để sót `'\n'`. Sang **Bài 013**, mình sẽ học cách xử lý luồng khi nhập số.

---

# PHẦN IV. KIẾN TRÚC BỘ XỬ LÝ LỆNH & CÁC MẪU GAMEPLAY THỰC CHIẾN

### 25. Đọc được chữ chưa phải là hiểu lệnh: Kiến trúc tách 3 tầng
Khi xây dựng hệ thống điều khiển bằng văn bản (dù là game console, hộp gõ lệnh debug trong Unreal Engine, hay hệ thống kịch bản hội thoại), việc nhồi nhét đọc bàn phím, phân tích chuỗi và trừ máu/cộng khoáng vào chung một đám rối trong `main()` sẽ làm code cực kỳ khó sửa.

Hãy tập nhìn theo **3 tầng trách nhiệm tách biệt**:

| Tầng | Tên trách nhiệm | Công việc cụ thể | Ví dụ trong code |
|---|---|---|---|
| **Tầng 1** | **Input (Thu nhận)** | Giao tiếp với `std::cin`, nhận nguyên dòng, chắn lỗi cạn luồng EOF. | `if (!std::getline(std::cin, rawCmd))` |
| **Tầng 2** | **Interpretation (Diễn giải)** | Nhận `std::string`, phân loại xem nó là *Rỗng*, *Lệnh hợp lệ nào*, hay *Lệnh không tồn tại*. | Hàm `DescribeCommand(rawCmd)` |
| **Tầng 3** | **Action (Thực thi Gameplay)** | Đối chiếu điều kiện trạng thái game (`hp > 0 && ore < maxCargo`) rồi mới cập nhật dữ liệu. | Hàm cập nhật `crystalOre` / `hp` |

Bài 012 đặt trọng tâm làm thật chắc **Tầng 1 và Tầng 2**, đồng thời cho anh thấy cách nối sạch sang **Tầng 3** mà chưa cần tới vòng lặp (Bài 014).

---

### 26. Exact Matching vs Normalization: Chính sách dữ liệu tùy thuộc mục đích Gameplay!
- **Exact Matching (Khớp tuyệt đối):** Chỉ `"mine"` mới khớp `"mine"`; `"MINE"` hay `"mine "` đều bị từ chối.
- **Normalization (Chuẩn hóa dữ liệu trước khi xử lý):** Chẳng hạn cắt khoảng trắng thừa ở hai đầu (*trim*), hoặc chuyển chữ cái tiếng Anh sang chữ thường (*lowercase*) để người chơi lỡ bật Caps Lock gõ `"MINE"` vẫn chạy được.

> **Tư duy phản biện trong thiết kế hệ thống:** Đừng bao giờ viết một hàm "làm sạch văn bản" rồi áp máy móc cho mọi chuỗi trong game! Mỗi loại văn bản cần một chính sách riêng:
> - **Lệnh điều khiển (`command`):** Cắt dấu cách hai đầu + hạ chữ thường ASCII sau này là hợp lý để giảm ức chế khi gõ nhầm. *(Ở Bài 012 ta giữ Exact Matching để nắm chắc quy tắc gốc và dễ kiểm thử).*
> - **Tên nhân vật / Tên cá voi (`whaleName`):** Có thể cắt dấu cách thừa hai đầu, nhưng **tuyệt đối không tự tiện hạ hết chữ thường** — người chơi đặt `"Aetheris Mk II"` mà bị đổi thành `"aetheris mk ii"` là sai ý đồ!
> - **World Seed trong game Roguelike (`seedText`):** Giống Minecraft hay các game sinh bản đồ ngẫu nhiên, `"Whaleon"` và `"whaleon "` là hai hạt giống hoàn toàn khác nhau, giữ nguyên từng ký tự lại là tính năng!

---

### 27. Phễu phân loại 4 nhánh chuẩn mực (`Empty` → `Action 1` → `Action 2` → `Unknown`)
```cpp
if (command.empty()) {
    // Nhánh 1: Chưa nhập lệnh (chỉ nhấn Enter)
} else if (command == kCmdMine) {
    // Nhánh 2: Lệnh hợp lệ thứ nhất
} else if (command == kCmdStatus) {
    // Nhánh 3: Lệnh hợp lệ thứ hai
} else {
    // Nhánh 4: Đã gõ chữ nhưng không khớp lệnh nào trong danh mục (Unknown)
}
```
Tại sao phải tách riêng **Empty** (Nhánh 1) và **Unknown** (Nhánh 4) thay vì gộp chung?
- **Empty** nghĩa là *người chơi chưa ra quyết định* (lỡ tay chạm Enter): thông báo hợp lý là *"Chưa nhập lệnh"*.
- **Unknown** nghĩa là *người chơi có ý định ra lệnh nhưng gõ sai tên hoặc gọi tính năng chưa có* (ví dụ gõ `"minne"` hoặc `"attack"`): in lại `"Unknown command: [" + command + "]"` giúp người chơi nhìn thấy ngay chữ mình vừa gõ sai để sửa!

---

### 28. Bẫy tư duy phủ định trong nhánh `else` (Liên hệ trực tiếp Bài 009)
Hãy soi đoạn code rất nguy hiểm sau (đây cũng là lỗi kinh điển khi nhờ AI viết nhanh mà không đọc kỹ):
```cpp
// ❌ LỖI THIẾT KẾ NGHIÊM TRỌNG:
if (command == "mine") {
    std::cout << "Dang khai thac pha le...\n";
} else {
    std::cout << "Rut lui ve can cu!\n"; // Nguy hiem!
}
```
- Nhớ lại Bài 009: nhánh `else` chạy khi điều kiện `if` sai — tức là khi **`command != "mine"`**.
- Tập hợp `command != "mine"` bao gồm `"retreat"`, nhưng cũng bao gồm luôn cả `"status"`, `""` (bấm nhầm Enter), `"minne"` (gõ trượt phím), `"help"`...!
- Đặt một hành động gameplay hợp lệ vào nhánh `else` mặc định nghĩa là **biến mọi lỗi gõ phím của người chơi thành hành động đó**!
- **Quy tắc bất biến:** Mọi hành động hợp lệ đều phải có điều kiện `else if (command == ...)` tường minh của riêng nó. Nhánh `else` cuối cùng **chỉ được phép đóng vai trò rào chắn từ chối lệnh lạ và giữ nguyên trạng thái game**.

---

### 29. Bốn mẫu cấu trúc chuỗi thực chiến đa dạng trong phát triển Game
Để anh quen tay dùng `std::string` giải quyết nhiều bài toán thiết kế khác nhau (cả trong thế giới Luftmensch của Whaleon lẫn các thể loại game khác), hãy xem 4 mẫu cấu trúc nền tảng sau:

#### Mẫu 1 — Tra cứu thông số từ Mã bản thiết kế (Survival / Crafting Blueprint ID)
*(Lưu ý thuật ngữ của giáo trình: **blueprint trong game** ở đây là bản thiết kế chế tạo đồ mà người chơi nhặt được, khác với công cụ lập trình trực quan **Unreal Blueprint** sẽ học ở Bài 043).*
```cpp
int GetBlueprintCrystalCost(std::string blueprintId) {
    if (blueprintId.empty()) {
        return -1; // Ma loi: chua chon ban thiet ke nao
    } else if (blueprintId == "bp_storm_shield") {
        return 25;
    } else if (blueprintId == "bp_sky_harpoon") {
        return 40;
    } else {
        return 0;  // Ma ban thiet ke la / khong ton tai
    }
}
```

#### Mẫu 2 — Cổng điều kiện kép kết hợp Chuỗi & Chỉ số (RPG / Adventure Quest Gate)
Kết hợp so sánh chuỗi `==` (Bài 012) với toán tử logic `&&`, `||` và so sánh số `>=` (Bài 007, 009):
```cpp
bool CanEnterAncientSanctum(std::string factionTag, std::string passPhrase, int whaleBondLevel) {
    const bool isTrustedGuild{factionTag == "sky_keepers" && whaleBondLevel >= 3};
    const bool knowsSecretWord{passPhrase == "aether awakens"};
    return isTrustedGuild || knowsSecretWord;
}
```

#### Mẫu 3 — Xây dựng Nhật ký diễn biến trận đấu (Turn-Based / Action Combat Log)
Dùng `+=` để gom nhiều sự kiện xảy ra trong 1 nhịp xử lý thành một bản báo cáo hoàn chỉnh trước khi hiển thị lên khung UI:
```cpp
std::string BuildEncounterReport(std::string creatureType, int damageTaken, int remainingHp) {
    std::string report{"[Cham tran] Doi thu: " + creatureType + "\n"};
    if (damageTaken > 0) {
        report += " -> Lop vo pha le chiu chan dong!\n";
    }
    if (remainingHp <= 20) {
        report += " -> [CANH BAO DO] HP xuong muc nguy kich!\n";
    }
    return report;
}
```

#### Mẫu 4 — Bộ lọc kiểm duyệt đặt tên Hồ sơ lưu game (UI / Save Slot Validator)
```cpp
std::string ValidateSaveProfileName(std::string profileName) {
    const int kMaxBytes{24};
    if (profileName.empty()) {
        return "Loi: Ten ho so khong duoc de trong.";
    } else if (static_cast<int>(profileName.size()) > kMaxBytes) {
        return "Loi: Ten vuot qua gioi han khung hien thi (24 byte).";
    } else if (profileName == "autosave" || profileName == "system") {
        return "Loi: Ten nay danh rieng cho he thong tu dong.";
    } else {
        return "OK";
    }
}
```

---

### 30. Hai mở rộng kỹ thuật đáng giá để nhìn xa hơn
1. **Cách phát hiện chuỗi "chỉ toàn dấu cách hoặc Tab" mà chưa cần vòng lặp:**
   Như đã phân tích ở Mục 09, `"   ".empty()` trả về `false`. Nếu muốn coi chuỗi chỉ toàn khoảng trắng cũng là không hợp lệ ngay từ bây giờ, thư viện `std::string` có hàm tìm kiếm:
   ```cpp
   const bool isBlank{
       runName.find_first_not_of(" \t") == std::string::npos
   };
   ```
   - `find_first_not_of(" \t")` đi tìm vị trí đầu tiên chứa ký tự **không phải** dấu cách `' '` và không phải tab `'\t'`.
   - Nếu không tìm thấy bất kỳ ký tự chữ nào (tức chuỗi là `""`, `" "`, `"   "`, `"\t\t"`), nó trả về giá trị đặc biệt `std::string::npos` (*no position*), làm `isBlank` bằng `true`!
   - *(Đây là công cụ thư viện bổ trợ để mở rộng tầm mắt, không ép buộc phải dùng trong bài cơ bản, và lưu ý nó chỉ quét khoảng trắng ASCII `' '` và `'\t'` chứ chưa quét mọi ký tự khoảng trắng đặc thù của Unicode).*

2. **Tại sao chưa nên tự viết bộ chẻ lệnh đa tham số kiểu `"mine 15 crystal_vein"` ở Bài 012?**
   Những lệnh ghép nhiều mảnh như `"mine 15"`, `"craft harpoon 2"` hay `"fly north 300"` đòi hỏi cả một chuỗi kỹ thuật phía sau: tách dòng thành danh sách các từ (*tokenizing*), chuyển chuỗi `"15"` sang số nguyên `int` kèm xử lý khi người chơi gõ `"mine abc"`, kiểm miền số âm, kiểm thiếu/thừa từ. Lộ trình `POA96` cố ý tách từng lớp: Bài 012 làm chủ **đọc dòng & phân loại chuỗi nguyên khối**, Bài 013 làm chủ **nhập số & bắt lỗi định dạng**, Bài 014–016 ghép vào **vòng lặp menu hoàn chỉnh**.

---

# PHẦN V. WHALEON LAB: TỪ DIỄN GIẢI THUẦN ĐẾN TÁC ĐỘNG GAMEPLAY

Trong thư mục `POA12/`, em đã chuẩn bị và biên dịch sẵn hai cấp độ Lab để anh thấy trọn vẹn cả tính kỷ luật kiến trúc lẫn cách gắn vào dữ liệu máu/khoáng thật:

### 31. Bản Lab chuẩn kiến trúc — `Expedition Console v0.1`
*(Mã nguồn đầy đủ nằm tại [vd3_expedition_console_chuan.cpp](file:///d:/Whaleon/Antigravity/POA96/POA12/vd3_expedition_console_chuan.cpp))*

**Đặc tả yêu cầu:**
1. Đọc tên chuyến đi bằng `std::getline` (giữ nguyên khoảng trắng bên trong); kiểm tra đọc luồng thành công trước khi dùng.
2. Từ chối ngay nếu tên chuyến đi rỗng.
3. Đọc một dòng lệnh văn bản bằng `std::getline`; kiểm tra đọc luồng thành công.
4. Tách riêng hàm thuần `DescribeCommand(std::string command)` để phân biệt 4 trường hợp: lệnh rỗng, lệnh `"mine"`, lệnh `"status"`, và lệnh không tồn tại.

```cpp
#include <iostream>
#include <string>

// Khai báo hàm (Declaration)
std::string DescribeCommand(std::string command);

int main() {
    std::string runName{};
    std::string command{};

    // Bước 1: Thu nhận tên chuyến đi + chắn lỗi kết thúc luồng (EOF)
    std::cout << "Run name: ";
    if (!std::getline(std::cin, runName)) {
        std::cout << "Input unavailable (EOF)\n";
        return 0;
    }

    // Bước 2: Kiểm tra luật nghiệp vụ cho tên chuyến đi
    if (runName.empty()) {
        std::cout << "Rejected: empty run name\n";
        return 0;
    }

    std::cout << "Run created: [" << runName << "] (" << runName.size() << " bytes)\n";

    // Bước 3: Thu nhận một dòng lệnh
    std::cout << "Command (mine/status): ";
    if (!std::getline(std::cin, command)) {
        std::cout << "Input unavailable (EOF)\n";
        return 0;
    }

    // Bước 4: Diễn giải lệnh qua hàm độc lập và in kết quả
    std::cout << "Result: " << DescribeCommand(command) << '\n';
    return 0;
}

// Định nghĩa hàm (Definition)
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
```

---

### 32. Giải phẫu Hợp đồng hàm (`Function Contract`) & Sự phối hợp Bài 001–012

| Hợp đồng của `DescribeCommand` | Nội dung cam kết |
|---|---|
| **Đầu vào (`Input`)** | Một đối tượng `std::string` bất kỳ được truyền bằng giá trị (bản sao độc lập). |
| **Đầu ra (`Output`)** | Một `std::string` mô tả chính xác nhóm phân loại của lệnh. |
| **Quy tắc khớp (`Policy`)** | Exact Matching: chỉ chấp nhận chính xác `"mine"` và `"status"`. |
| **Tác dụng phụ (`Side Effects`)** | **Không có:** Hàm không đụng vào `std::cin`, không tự in `std::cout`, không sửa biến bên ngoài `main()`. |

> **Lợi ích khi nối sang Tầng 3 (Gameplay Action):** Trong file thực hành thứ hai [vd2_thuc_hanh_chuyen_di.cpp](file:///d:/Whaleon/Antigravity/POA96/POA12/vd2_thuc_hanh_chuyen_di.cpp), anh sẽ thấy khi nối lệnh `"mine"` vào dữ liệu thật (`hp = 80`, `crystalOre = 4`, `maxCargo = 10`), chỉ nhánh khớp đúng lệnh hợp lệ **và** thỏa mãn `hp > 0 && currentOre < maxCargo` (Bài 009) mới cộng khoáng và trả về số khoáng mới; mọi lệnh rỗng hay lệnh lạ đều trả nguyên `currentOre` cũ!

---

### 33. Ma trận kiểm thử 9 ca biên (Test Matrix)

| Mã ca | Nhập `runName` | Nhập `command` | Kết quả chương trình | Bất biến (Invariant) được kiểm chứng |
|---|---|---|---|---|
| **T1** | `Crystal Whale Expedition` | `mine` | `Mining action selected` | Giữ trọn vẹn nhiều khoảng trắng trong tên; nhận đúng lệnh 1. |
| **T2** | `Dao May Bao Tu 04` | `status` | `Showing expedition status` | Phân biệt chính xác lệnh 2 với lệnh 1. |
| **T3** | `Dao May Bao Tu 04` | *(chỉ bấm Enter)* | `Rejected: empty command` | Lệnh rỗng bị bắt riêng, không thực thi hành động nào. |
| **T4** | `Dao May Bao Tu 04` | `attack` | `Unknown command: [attack]` | Lệnh lạ không tồn tại bị báo rõ, không biến tướng thành `status`. |
| **T5** | `Dao May Bao Tu 04` | `MINE` | `Unknown command: [MINE]` | Chứng minh tính phân biệt HOA/thường của `==`. |
| **T6** | `Dao May Bao Tu 04` | `mine ` *(dư dấu cách cuối)* | `Unknown command: [mine ]` | `getline` không tự cắt dấu cách; ngoặc `[]` làm lộ dấu cách thừa. |
| **T7** | *(chỉ bấm Enter)* | *(không hỏi tới)* | `Rejected: empty run name` | Dừng sớm an toàn ngay từ cổng kiểm duyệt tên chuyến đi. |
| **T8** | `   ` *(3 dấu cách)* | `mine` | Chấp nhận tên `[   ]`, chạy `mine` | Chứng minh `.empty()` chỉ bắt chuỗi 0 byte, không bắt chuỗi khoảng trắng! |
| **T9** | `Ctrl+Z` rồi `Enter` | *(không hỏi tới)* | `Input unavailable (EOF)` | Đóng luồng đột ngột không bị xử lý nhầm thành chuỗi hợp lệ. |

---

### 34. Ghi chú thực tế trên môi trường Windows + Visual Studio của máy anh
- Trình biên dịch MSVC trên máy anh (`D:\Whaleon\Visual Studio`) khi truyền cờ `/utf-8` sẽ hiểu toàn bộ mã nguồn `.cpp` lưu ở chuẩn UTF-8 một cách chính xác.
- Tuy nhiên, cửa sổ Terminal/PowerShell của Windows đôi khi mặc định dùng bảng mã vùng cũ khi nhận phím gõ tiếng Việt có dấu trực tiếp từ bộ gõ. Vì vậy khi thử nghiệm console ở giai đoạn C++ nền, anh dùng tiếng Việt không dấu hoặc tiếng Anh cho từ khóa lệnh (`"mine"`, `"extract"`, `"khai thac"`) sẽ giúp đối chiếu số byte của `.size()` trực quan nhất, không bị nhiễu bởi bộ gõ của hệ điều hành.

---

# PHẦN VI. HỆ THỐNG BÀI TẬP TRUY VẾT TƯ DUY, AI CODE REVIEW & ĐỔI LUẬT

> **Nguyên tắc luyện sâu:** Anh thử tự nhẩm truy vết từng biến trên giấy hoặc trong đầu trước khi chạy `.exe`. Khi làm sản phẩm cùng AI sau này, năng lực "nhìn code thấy luồng dữ liệu chạy" chính là vũ khí mạnh nhất của người cầm lái!

### Bài A. Truy vết Đối tượng độc lập, `+`, `+=`, `=` và `Shadowing`
```cpp
#include <iostream>
#include <string>

int main() {
    std::string whale{"Aether"};
    std::string callSign{whale + "-01"};
    whale += " Prime";

    std::cout << "1: " << whale << '\n';
    std::cout << "2: " << callSign << '\n';

    {
        std::string whale{"Shadow Whale"};
        std::cout << "3: " << whale << '\n';
    }

    whale = "Luftmensch";
    std::cout << "4: " << whale << '\n';
    std::cout << "5: " << callSign << '\n';
}
```
- **Câu hỏi:** Dự đoán chính xác 5 dòng in ra (`1` → `5`). Giải thích vì sao khi `whale` bị đổi bằng `+=` và `=`, biến `callSign` vẫn đứng yên không đổi, và vì sao `"Shadow Whale"` ở dòng `3` biến mất ngay sau dấu `}`?

---

### Bài B. Bẫy khớp lệnh chính xác (Exact Matching Trap)
Cho hàm `DescribeCommand` ở Mục 31. Hãy dự đoán kết quả trả về ứng với 6 chuỗi `command` sau:
1. `"status"`
2. `"Status"`
3. `"status "`
4. `" mine"`
5. `""`
6. `"   "`
- **Câu hỏi bản chất:** Vì sao điều kiện `!command.empty()` bằng `true` vẫn **chưa đủ** để kết luận người chơi đã nhập một lệnh có ý nghĩa?

---

### Bài C. Truy vết `std::getline`: Ghi đè và dòng rỗng
```cpp
#include <iostream>
#include <string>

int main() {
    std::string zone{"Starting Hangar"};

    std::getline(std::cin, zone);
    std::cout << "Lan 1: [" << zone << "]\n";

    std::getline(std::cin, zone);
    std::cout << "Lan 2: [" << zone << "]\n";
}
```
Giả sử ở lần dừng thứ nhất người chơi gõ `Storm Archipelago` rồi nhấn `Enter`; ở lần dừng thứ hai người chơi **chỉ nhấn thẳng phím `Enter`**.
1. Hai dòng in ra màn hình chính xác là gì?
2. Vì sao `"Starting Hangar"` không còn sau lần 1, và vì sao `"Storm Archipelago"` cũng không được giữ lại sau lần 2?
3. Lần đọc thứ hai nhận về chuỗi rỗng — thao tác `std::getline` lần 2 đó là **đọc thành công** hay **đọc thất bại**?

---

### Bài D. Pass-by-Value với chuỗi
```cpp
#include <iostream>
#include <string>

std::string UpgradeTitle(std::string name) {
    name += " Mk.II";
    return name;
}

int main() {
    std::string gear{"Crystal Drill"};

    UpgradeTitle(gear);
    std::cout << "A: " << gear << '\n';

    std::string preview{UpgradeTitle(gear)};
    std::cout << "B: " << gear << '\n';
    std::cout << "C: " << preview << '\n';

    gear = preview;
    std::cout << "D: " << gear << '\n';
}
```
- **Câu hỏi:** Dự đoán `A, B, C, D`. Chỉ ra **đúng dòng lệnh duy nhất** thực sự làm thay đổi nội dung của biến `gear` trong `main()`, và giải thích mối quan hệ giữa tham số `name` của hàm với biến `gear`.

---

### Bài E. AI Code Review — Bắt thóp bộ phân loại lệnh viết ẩu
Giả sử anh yêu cầu AI viết hàm phân loại 2 lệnh `"mine"` và `"retreat"`, và AI sinh ra đoạn code sau:
```cpp
std::string ClassifyAction(std::string command) {
    if (command == "mine") {
        return "Start mining crystal";
    }
    return "Retreat to sky dock";
}
```
1. Đoạn code trên có biên dịch thành công không?
2. Chuyện gì xảy ra nếu người chơi bấm nhầm `Enter` (dòng rỗng) hoặc gõ `"scan"`?
3. Chỉ ra **2 lỗi thiết kế luật** trong hàm trên và viết lại phiên bản chuẩn.

---

### Bài F. Hai chiếc bẫy ngầm: So sánh thứ tự & Kiểu trả về của `.size()`
1. Cho `std::string floorA{"Floor9"};` và `std::string floorB{"Floor12"};`. Biểu thức `(floorA > floorB)` trả về `true` hay `false`? Hãy chỉ rõ máy dừng lại kết luận ngay tại cặp ký tự thứ mấy!
2. Cho `std::string note{};`. Giải thích vì sao biểu thức `note.size() - 1` không mang giá trị `-1`, và cách viết điều kiện an toàn trước khi trừ `1` khỏi `.size()`.

---

### Bài G. Nhiệm vụ Thực hành chính thức của Giáo trình Bài 012 (Đổi luật sản phẩm)
Anh có thể chọn làm trên [vd3_expedition_console_chuan.cpp](file:///d:/Whaleon/Antigravity/POA96/POA12/vd3_expedition_console_chuan.cpp) (bản thuần diễn giải) **hoặc** trên [vd2_thuc_hanh_chuyen_di.cpp](file:///d:/Whaleon/Antigravity/POA96/POA12/vd2_thuc_hanh_chuyen_di.cpp) (bản có gắn HP và khoáng):
- **Yêu cầu thay đổi thiết kế:**
  1. Đổi tên hành động khai thác từ `"mine"` sang tên mới (ví dụ `"extract"`, hoặc một lệnh có khoảng trắng như `"khai thac"` để tận dụng `std::getline`!).
  2. Lệnh `"mine"` cũ phải tự động trở thành **lệnh không tồn tại**.
  3. Đảm bảo **lệnh rỗng** (`""`) được kiểm tra và báo thông điệp riêng biệt, không gộp chung với lệnh không tồn tại.
  4. Liệt kê tất cả các vị trí cần cập nhật đồng bộ (cả phần kiểm tra logic lẫn dòng lời nhắc hướng dẫn người chơi ở `main()`).

---

### Bài H. Câu hỏi tư duy sâu về Unicode (Giải thích bằng lời)
Trong màn hình đặt tên tàu nhiều người chơi, người chơi A gõ `"Cá Voi"` bằng bộ gõ dựng sẵn (NFC), người chơi B copy-paste `"Cá Voi"` từ một tài liệu dùng tổ hợp dấu rời (NFD). Trên màn hình cả hai tên hiển thị giống hệt nhau.
1. Phép so sánh `nameA == nameB` bằng `std::string` thuần có đảm bảo ra `true` không?
2. `nameA.size() == nameB.size()` có đảm bảo bằng nhau không?
3. Nếu sau này muốn hệ thống cấm đặt tên trùng theo cảm nhận mắt người, game sẽ cần thêm một bước xử lý loại gì trước khi so sánh `==`?

---

# PHẦN VII. TỔNG KẾT & CHECKLIST HOÀN THÀNH BÀI 012

### Sáu nguyên lý cốt lõi cần khắc sâu sau Bài 012
1. **`std::string` là đối tượng quản lý dãy byte văn bản tự động**, khác bản chất với ký tự đơn `char` (`'C'`) và hằng mảng ký tự tĩnh `"C"` (vốn có `'\0'` cuối mảng và không tự ghép bằng `+`).
2. **Kiểu dữ liệu hợp lệ chưa chắc đã hợp lệ theo luật gameplay:** chuỗi rỗng `""` luôn hợp lệ với C++, còn chấp nhận hay từ chối là quyết định thiết kế của từng ô dữ liệu.
3. **`.size()` đếm số phần tử `char` (byte) dưới dạng số không dấu (`std::size_t`):** luôn cẩn trọng với tiếng Việt UTF-8 nhiều byte và tuyệt đối không lấy `size() - 1` khi chưa chắn `!empty()`.
4. **`std::getline` đọc trọn một dòng tới `'\n'`, tiêu thụ luôn `'\n'` và ghi đè biến đích**, nhưng nó trung thực giữ nguyên mọi khoảng trắng và không tự hiểu ý định người chơi.
5. **Thao tác đọc thất bại (`!std::getline(...)` do EOF) và chuỗi rỗng (`cmd.empty()`) là hai tầng hoàn toàn khác nhau**, cần tách biệt rõ câu hỏi kiểm tra.
6. **Bộ phân loại lệnh an toàn không bao giờ dùng nhánh `else` mặc định để thực thi một hành động hợp lệ khác** — mọi đầu vào rỗng hoặc không nhận diện được phải bị chặn lại mà không làm hỏng dữ liệu trạng thái của game.

### Checklist tự rà soát trước khi chốt Bài 012
- [ ] Phân biệt được `'C'`, `"C"` và `std::string{"C"}`.
- [ ] Giải thích được sự khác nhau giữa `a + b`, `a += b`, `a = b`, `a.clear()`, và vì sao `"A" + "B"` lỗi biên dịch.
- [ ] Nắm rõ `.size()`, `.empty()`, bẫy chuỗi toàn khoảng trắng `"   "` và bẫy `0 - 1` trên kiểu không dấu.
- [ ] Giải thích được bẫy so sánh thứ tự từ điển (`"Zone10" < "Zone2"`) và giới hạn đếm byte trên UTF-8.
- [ ] Truy vết được luồng ký tự của `std::getline`, khác biệt giữa EOF và dòng rỗng, và lý do trộn `cin >>` trước `getline` gây trôi dòng.
- [ ] Hoàn tất phần trả lời truy vết / sửa luật thực hành ở Phần VI.
