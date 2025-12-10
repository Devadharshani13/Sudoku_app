# 🧩 Sudoku Game using C & SDL2

An interactive **Sudoku Puzzle Game** developed in **C programming** using the **SDL2 graphics library**, offering a **mobile-app-like user experience** on desktop platforms with real-time features such as **undo/redo, hints, auto-solve, timer, and move tracking**.

---
## Project Description

Sudoku is a classic **logic-based number puzzle game** played on a **9×9 grid**, where each row, column, and **3×3 sub-grid** must contain the digits **1 to 9 without repetition**.

This project implements a **real-time Sudoku desktop application** with:

* ✅ Responsive and user-friendly **GUI using SDL2**
* ✅ **Multiple difficulty levels** (Easy, Medium, Hard)
* ✅ **Real-time timer** and **move counter**
* ✅ **Undo, Redo, Hint, Solve, and New Game** controls
* ✅ **Number palette** for quick input
* ✅ **Highlighted selected cells & incorrect moves**
* ✅ **Auto-solver using backtracking algorithm**

The application is compiled into a **Sudoku executable (`sudoku.exe`)** and runs on **Windows/Linux** platforms.

---

## Features

* 🎮 Interactive Sudoku gameplay
* ⏱ Real-time timer tracking
* 🔁 Undo & Redo functionality
* 💡 Hint generation
* 🤖 Auto-solve using recursion
* 🎯 Move validation with color feedback
* 🧠 Data structures based design
* 📐 Resizable game window
* 📱 Mobile-app-style UI layout

---

## Technical Details

| Component            | Technology                             |
| -------------------- | -------------------------------------- |
| Programming Language | C                                      |
| Graphics Library     | SDL2, SDL2_ttf                         |
| Front-End            | SDL2 GUI (2D graphics, text rendering) |
| Back-End             | Sudoku validation & solving engine     |
| Data Structures      | 2D Arrays, Stack                       |
| Algorithm            | Recursive Backtracking                 |

---

## Data Structures Used

* **2D Array (9×9 Grid)** – Store Sudoku board
* **Stack (Array Implementation)** – Undo/Redo actions
* **Structures (`struct`)** – Cell properties & move history

---

## Implementation Steps

### 1️⃣ Setup Environment

* Install **SDL2** and **SDL2_ttf**
* Compile using:

```
gcc sudoku.c -lSDL2 -lSDL2_ttf -o sudoku
```

---

### 2️⃣ Board Representation

* Each cell stores:

  * Value
  * Fixed / Editable state

---

### 3️⃣ Core Sudoku Logic

* Move validation
* Empty cell detection
* Recursive backtracking solver

---

### 4️⃣ Game Features

* Undo / Redo using stack
* Hint generation
* Auto solve
* Move counter

---

### 5️⃣ GUI Design using SDL2

* Sudoku grid rendering
* Button controls
* Highlight selected cell
* Display timer and moves
* Number palette (1–9)

---

### 6️⃣ Testing & Debugging

* Puzzle validation tested
* Stack operations verified
* Window resizing handled

---

## Output

* ✅ Game opens in a separate window
* ✅ Displays main game interface
* ✅ Highlights:

  * ❌ Wrong input → **Red**
  * ✅ Correct input → **Green**
* ✅ Undo & Redo functional
* ✅ Completion message displayed on success

---

## Project File

* `sudoku.exe` – Final compiled executable
* `sudoku.c` – Source code
* `font.ttf` – Font file for number rendering

---

## Conclusion

The **Sudoku Game Project** successfully demonstrates how **logic-based games can be built using C and SDL2**. It provides an engaging and modern gameplay experience with advanced features such as:

* Undo/Redo
* Hint system
* Auto-solve
* Real-time timer & move tracking

This project strengthens understanding of:

* ✅ **Data Structures** (2D arrays, stacks)
* ✅ **Algorithms** (Backtracking)
* ✅ **Game Development**
* ✅ **GUI Programming with SDL2**

Overall, it is a strong demonstration of **problem-solving, graphics programming, and software development skills**.

---
