# Catherine Programming Language User Guide

**English** | [Tiếng Việt](instructions_vi.md)

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

## 🛠️ Detailed Syntax

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
* **`clear`**: Clears console screen immediately.
* **Advanced String Interpolation**: Interpolates expressions `!{calc(a + b)}` and struct fields `!{sv[i].name}` directly inside `echo` strings.

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

### 4. Conditionals (`if`, `ef` / `else if`, `else`)

```catherine
if d == 0 {
    echo "Double root"
} ef d > 0 {
    echo "2 distinct roots"
} else {
    echo "No real roots"
}
```

---

### 5. Other Loops (`repeat`, `while`, `until`) & Controls (`stop`, `skip`)

* `repeat <count> { ... }`
* `while <condition> { ... }`
* `until <condition> { ... }`
* `stop` (`break`) and `skip` (`continue`).

---

### 6. Function Definitions (`func`)

* **Syntax**: `define func <func_name>(<params...>) as <return_type> { ... }`
* **Return Value**: Assigning a value to `<func_name>` inside the function block sets its return value.
