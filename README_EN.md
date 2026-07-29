# Catherine Programming Language (Draft Specification)

> [!NOTE]
> **Catherine** is an experimental programming language designed for simplicity, human readability, and intuitive syntax. Catherine source files use the **`.rine`** extension.

[Tiếng Việt](README.md) | **English Version**

---

## 🚀 Build & Run

1. **Build Catherine Interpreter**:
   ```bash
   ./build.sh
   ```
2. **Run Catherine `.rine` source files**:
   ```bash
   ./catherine calculator.rine
   ./catherine phuogntrinhbac2.rine
   ./catherine example_for_struct_array.rine
   ```

---

## 📌 Core Features Overview

* **File Extension**: `.rine` (e.g. `calculator.rine`, `phuogntrinhbac2.rine`).
* **Philosophy**: Simple, concise, and easy to understand for beginners.
* **Supported Data Types**:
  * `number`: Numeric values (includes `int`, `float`, `double`).
  * `string`: Text strings.
  * `boolean`: Truth values (`true`/`false`).
  * `struct`: Custom user-defined data structures.
  * `Type[]`: Dynamic array lists.

---

## 🛠️ Basic Syntax

### 1. Variables, Structs & Arrays

* **Basic Variable Declaration**: `define a, b as number`
* **Declaration with Initialization**: `define count = 3, i as number`
* **Struct Declaration**:
  ```catherine
  define Student as struct {
      define name, dob as string
      define year as number
      define gpa as number
  }
  ```
* **Array Declaration (`Type[]`)**:
  ```catherine
  define sv as Student[]    // Array of Student structs
  define list as number[]   // Array of numbers
  ```

---

### 2. Input/Output (`echo` / `listen`) & Screen Clear (`clear`)

* **`echo`**: Prints values or strings to the console.
* **`listen`**: Reads user input into variables, array elements, or struct fields (e.g., `listen sv[i].name`).
  * *Buffer Note*: Unlike C++'s `cin >>` which leaves trailing `\n` requiring `cin.ignore()`, Catherine uses full line `getline` input processing for all `listen` statements. **The input buffer is always 100% clean and never skips prompts**!
* **`clear`**: Clears console screen immediately.
* **Advanced String Interpolation**: Interpolates struct fields and array elements directly like `!{sv[i].name}`.

---

### 3. `for` Loop (with `up` and `down` keywords)

Syntax: `for <init>, <condition>, <step> { ... }`

* **`up`**: Increments loop counter by 1 (`i++` / `i = i + 1`).
* **`down`**: Decrements loop counter by 1 (`i--` / `i = i - 1`).

```catherine
define count = 3, i as number

for i = 0, i < count, up {
    echo "Iteration !{i + 1}"
}
```

---

## 📝 Example Source Code: Student Management (`example_for_struct_array.rine`)

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
