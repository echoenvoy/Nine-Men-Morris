
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



## 📁 Project Structure

```
Mill_Game/
├── CORE FILES
│   ├── fonctions_de_jeu.h        // Main header file (all declarations & global vars)
│   ├── start.c                   // Main menu & program initialization
│   ├── joueurJoueur.c            // Player vs Player mode
│   ├── joueurMachine1.c          // Player vs Beginner AI
│   ├── joueurMachine2.c          // Player vs Advanced AI
│   ├── fonctions_de_jeu.c        // Core game logic (board, mills, captures)
│   ├── fonctions_de_jeu2.c       // Beginner AI logic
│   ├── fonctions_de_jeu3.c       // Advanced AI logic
│
├── DOCUMENTATION
│   ├── Rapport.pdf               // Final project report
│   └── 1CS_Project_requirements_document_24-25-FrenchVersion-Finale.pdf
│
└── COMPILED OUTPUT
    └── jeu.exe (or generated executables after compilation)
```

---

## 🛠️ Compilation Command

Use this command to compile the entire project:

```
gcc start.c fonctions_de_jeu.c fonctions_de_jeu2.c fonctions_de_jeu3.c joueurJoueur.c joueurMachine1.c joueurMachine2.c -o jeu.exe
```
## 🎥 Test Video

A demonstration video is included showing a complete **Player vs Player** game in action :
👉 **https://drive.google.com/file/d/1ugOlfI8DJ1QFpVrhtq3PQwmW25eE8hJf/view?usp=sharing**

---

## 👨‍🏫 Authors

* Hamza Amhidi
* Omar Amdouni
* Saoud Amine

---
