# Catherine Programming Language (Draft Specification)

> [!NOTE]
> **Catherine** là một ngôn ngữ lập trình thử nghiệm hướng tới sự đơn giản, cú pháp thân thiện, dễ đọc và trực quan. File mã nguồn Catherine sử dụng đuôi mở rộng **`.rine`**.

**Tiếng Việt** | [English Version](README_EN.md)

---

## 🚀 Biên dịch & Chạy dự án (Build & Run)

1. **Biên dịch trình thông dịch Catherine**:
   ```bash
   ./build.sh
   ```
2. **Thực thi file mã nguồn `.rine`**:
   ```bash
   ./catherine calculator.rine
   ./catherine phuogntrinhbac2.rine
   ./catherine example_for_struct_array.rine
   ```

---

## 📌 Tổng quan các đặc trưng cốt lõi

* **Đuôi file mã nguồn**: `.rine` (ví dụ: `calculator.rine`, `phuogntrinhbac2.rine`).
* **Triết lý**: Đơn giản, ngắn gọn và dễ hiểu đối với người mới bắt đầu.
* **Kiểu dữ liệu hỗ trợ**:
  * `number`: Đại diện cho số (bao gồm số nguyên `int`, số thực `float`, `double`).
  * `string`: Chuỗi ký tự.
  * `boolean`: Giá trị đúng/sai (`true`/`false`).
  * `struct`: Kiểu dữ liệu cấu trúc do người dùng tự định nghĩa.
  * `Type[]`: Kiểu mảng động chứa danh sách các phần tử cùng kiểu.

---

## 🛠️ Cú pháp cơ bản

### 1. Khai báo Biến, Cấu trúc (`struct`) & Mảng (`Array`)

* **Khai báo biến cơ bản**: `define a, b as number`
* **Khai báo kèm khởi tạo**: `define soLuong = 3, i as number`
* **Khai báo Cấu trúc (`struct`)**:
  ```catherine
  define Student as struct {
      define name, dob as string
      define year as number
      define gpa as number
  }
  ```
* **Khai báo Mảng (`Type[]`)**:
  ```catherine
  define sv as Student[]    // Mảng chứa danh sách các sinh viên
  define list as number[]   // Mảng chứa danh sách số
  ```

---

### 2. Nhập & Xuất dữ liệu (`echo` / `listen`) & Màn hình (`clear`)

* **`echo`**: Xuất giá trị hoặc chuỗi ra màn hình.
* **`listen`**: Nhận dữ liệu từ bàn phím gán vào biến, phần tử mảng hoặc trường của struct (ví dụ `listen sv[i].name`).
  * *Lưu ý về Bộ nhớ đệm*: Khác với `cin >>` trong C++ bị đọng `\n` cần `cin.ignore()`, Catherine sử dụng cơ chế đọc trọn dòng `getline` nâng cao cho mọi lệnh `listen`, giúp **bộ nhớ đệm luôn sạch sẽ 100% và không bao giờ bị trôi lệnh nhập**!
* **`clear`**: Lệnh xoá màn hình console ngay lập tức.
* **String Interpolation nâng cao**: Hỗ trợ truy xuất phần tử mảng và trường struct trực tiếp như `!{sv[i].name}` hoặc `!{calc(a + b)}`.

---

### 3. Vòng lặp `for` (kèm từ khóa `up` và `down`)

Cú pháp vòng lặp `for`: `for <khởi_tạo>, <điều_kiện>, <bước_nhảy> { ... }`

* **`up`**: Tương đương tăng 1 đơn vị (`i++` / `i = i + 1`).
* **`down`**: Tương đương giảm 1 đơn vị (`i--` / `i = i - 1`).

```catherine
define soLuong = 3, i as number

for i = 0, i < soLuong, up {
    echo "Lần lặp thứ !{i + 1}"
}
```

---

### 4. Cấu trúc điều kiện (`if`, `ef` / `else if`, `else`)

```catherine
if d == 0 {
    echo "Nghiem kep"
} ef d > 0 {
    echo "2 nghiem phan biet"
} else {
    echo "Vo nghiem"
}
```

---

### 5. Vòng lặp khác (`repeat`, `while`, `until`) & Điều khiển (`stop`, `skip`)

* `repeat <count> { ... }`
* `while <condition> { ... }`
* `until <condition> { ... }`
* `stop` (`break`) và `skip` (`continue`).

---

## 📝 Mã nguồn minh họa Quản lý Sinh viên với Struct, Array & For (`example_for_struct_array.rine`)

```catherine
define Student as struct {
    define name, dob as string
    define year as number
    define gpa as number
}

define soLuong = 3, i as number
define sv as Student[]

echo "Nhap danh sach sinh vien"
for i = 0, i < soLuong, up {
    echo "Nhap thong tin sinh vien thu !{i + 1}"
    echo "Nhap ten sinh vien"
    listen sv[i].name
    echo "Nhap ngay sinh"
    listen sv[i].dob
    echo "Sinh vien nay hoc nam thu may?"
    listen sv[i].year
    echo "Nhap diem GPA cua sinh vien"
    listen sv[i].gpa
    clear
}

echo "Thong tin sinh vien"
for i = 0, i < soLuong, up {
    echo "+++++++++++++++++++++++++++++++++++++++"
    echo "Thong tin sinh vien thu !{i + 1}"
    echo "Ten: !{sv[i].name} - Ngay sinh: !{sv[i].dob} - Nam thu: !{sv[i].year} - GPA: !{sv[i].gpa}"
}
```
