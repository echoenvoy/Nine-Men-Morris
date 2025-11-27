
---

# 🎮 Jeu du Moulin – C Project

## ✅ Description

Implementation of the **Nine Men’s Morris (Jeu du Moulin)** game in C.
The game includes **Player vs Player** and **Player vs Machine** modes, with **two AI difficulties** (easy + advanced).

---

## 🧩 Features

* Two game modes:

  * **Player vs Player**
  * **Player vs Machine (Easy & Advanced AI)**
* Two phases of the game:

  * **Placement phase** (9 pieces per player)
  * **Movement phase** (move pieces, free movement at 3 pieces)
* **Automatic mill detection**
* **Capture system** (cannot capture protected mills)
* **Win detection** (opponent blocked or ≤2 pieces)
* **Input validation** to avoid crashes
* **Colored console display** for players and last move
* Modular code (several `.c` files + a `.h` header)

---

## 🏗 Project Structure (important only)

```
main.c
start.c
joueurJoueur.c
joueurMachine1.c   (easy AI)
joueurMachine2.c   (advanced AI)
fonctions_de_jeu*.c
header.h
Makefile
```

---

## ⚙️ How to Compile

```
make
./mill_game.exe
```

Or:

```
gcc src/*.c -o moulin.exe
./moulin.exe
```

---

## 👨‍🏫 Authors

* Omar Amdouni
* Saoud Amine
* Hamza Amhidi

---

If you want, I can also generate a **very short README** (10 lines max) or a **GitHub-ready stylish version**.
