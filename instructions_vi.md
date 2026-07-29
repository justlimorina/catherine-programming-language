# Hướng dẫn sử dụng Ngôn ngữ lập trình Catherine

[English Guide](instructions.md) | **Tiếng Việt**

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

## 🛠️ Cú pháp chi tiết

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
* **`clear`**: Lệnh xoá màn hình console ngay lập tức.
* **String Interpolation nâng cao**: Hỗ trợ chèn biểu thức `!{calc(a + b)}` hoặc trường struct `!{sv[i].name}` trực tiếp trong chuỗi `echo`.

---

### 3. Vòng lặp `for` (kèm từ khóa `up` và `down`)

Cú pháp: `for <khởi_tạo>, <điều_kiện>, <bước_nhảy> { ... }`

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

### 6. Định nghĩa hàm người dùng (`func`)

* **Cú pháp**: `define func <tên_hàm>(<tham_số...>) as <kiểu_trả_về> { ... }`
* **Trả về giá trị**: Gán giá trị cho `<tên_hàm>` bên trong khối lệnh để xác định giá trị trả về của hàm.
