# Catherine Programming Language User Guide

**English** | [Tiếng Việt](instructions_vi.md)

---

## 🚀 Build & Run

* **Linux / macOS**:
  ```bash
  ./build.sh
  ./build/catherine calculator.rine
  ```

* **Windows (PowerShell / Command Prompt)**:
  ```powershell
  .\build.ps1        # or build.bat
  .\build\Release\catherine.exe calculator.rine
  ```

* **Interactive REPL**: Run `catherine` (or `catherine.exe`) with no file argument for a live prompt. Cross-platform arrow key navigation and history are supported.

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
* **Array Literal**: initialize arrays inline — `define x = [1, 2, 3] as number[]`. Arrays are dynamic: elements may mix types (`[1, "two", true]`) and indexes auto-expand on assignment (`x[5] = 1`).

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

---

### 7. Numbers, Strings & Built-in Functions

* **Negative numbers**: `-5`, `-a`, `-(a + b)` work directly — no need for `0 - b`.
* **String comparison**: strings support `<`, `>`, `<=`, `>=` (alphabetical) as well as `==` / `!=`.
* **`len(x)`**: number of elements in an array, or length of a string.
* **`push(arr, x)`**: returns a new array with `x` appended — use `arr = push(arr, x)`.
* **`calc(expr)`**: evaluates an arithmetic expression (handy inside `!{ }` interpolation).
* **`sqrt(x)`**: square root.
* **`sum/sub/multi/div(a, b)`**: arithmetic helpers; also `idiv` (integer division) and `mod` (remainder).
* **Errors**: runtime errors report the offending line, e.g. `Line 3: Runtime Error: Division by zero`.
