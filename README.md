# 🎲 LUDO like UCSC - SDL2 Edition

A custom-built **LUDO game written in C using SDL2**, tailored for the University of Colombo School of Computing (UCSC).  
This is a fully playable, GUI-based board game with support for player movement, captures, and winning conditions.

---

## 📸 Preview

![image](https://github.com/user-attachments/assets/6f91dd1a-bb68-4efe-8c0c-b82bf1b045c1)

---

## ✨ Features

- 🔁 Token movement with arrow keys
- 🔺 Support for 4 different player colors: Red, Green, Blue, Yellow
- 🏠 Home paths and logic
- 🚫 Collision and capture system (knock tokens back to base)
- 🏆 Win detection when all 4 tokens reach final home
- 🎨 Clean UI with circular tokens and colored borders
- 🧱 Uses SDL2, SDL2_ttf and SDL2_image for rendering

---

## 💻 Requirements

- OS: Windows (MSYS2 environment)
- Compiler: `gcc` via MinGW (64-bit)
- Libraries: SDL2, SDL2_image, SDL2_ttf

---

## 🛠️ Setup Instructions

### ✅ 1. Download & Install MSYS2

If you haven't already, grab MSYS2 from the official site:  
👉 [https://www.msys2.org](https://www.msys2.org)

> ⚠️ **Important:** Use the *MinGW 64-bit shell*, not the default MSYS shell.

---

### ✅ 2. Launch MinGW 64-bit Shell

Search in Start Menu:  
📂 **MSYS2 64bit → MSYS2 MinGW 64-bit**

Once the shell is open, proceed below.

---

### ✅ 3. Update MSYS2 (Recommended)

```bash
pacman -Syu
```

> 🔁 After this, **close the shell** and reopen it again.

Then run:

```bash
pacman -Su
```

---

### ✅ 4. Install SDL2 & SDL2_image

Install required libraries using `pacman`:

```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image
```

> 📦 These are essential for rendering graphics and loading images.

---

### ✅ 5. Clone or Download the Game

If using Git:

```bash
git clone https://github.com/oshadhalakshan/ludo.git
cd ludo
```

Or download and extract the ZIP.

---

### ✅ 6. Compile the Game

Make sure you're in the directory where `main.c` and `icon.res` are located. Then run:

```bash
gcc main.c logic.c icon.res -o app.exe -Iinclude -Llib -lSDL2 -lSDL2_image -lSDL2_ttf
```

✅ If successful, this will create `app.exe`.
---

### ✅ 7. Run the Game

Launch the game with:

```bash
app
```

Enjoy the colorful UCSC-themed LUDO experience! 🎉

---

## 🎮 Controls

| Key         | Action                         |
|-------------|--------------------------------|
| `7`         | Select Red token group         |
| `9`         | Select Green token group       |
| `1`         | Select Blue token group        |
| `3`         | Select Yellow token group      |
| `A / D`     | Cycle between tokens in upper  |
| `J / L`     | Same for lower tokens in cycle |
| `←` `→`     | Move token counter/clockwise   |
| `TAB`       | Switch between same tokens     |

> ⚠️ Tokens must be selected and placed on the board before movement.

---

## 🧠 Game Logic Overview

- When a token is placed, it begins on the universal path.
- If it reaches its home entry cell, it switches to the private home path.
- If a token lands on a spot occupied by another **different-color** token:
  - That token gets sent back to base.
- If **all 4 tokens** of a color reach the final home cell:
  - 🎉 That player **wins** the game.

---

## 🧰 Project Structure

```
.
├── main.c          # Main function
├── logic.c         # Game logic
├── icon.rc         # Optional: icon resource file
├── icon.res        # Compiled icon resource
├── include/        # Include headers
├── lib/            # Libraries
├── assets/         # Images and future assets (if any)
├── <app.exe>       # Application
└── <random.dll>    # DLL files
```

---

## 💡 Tips

- Always use **MinGW 64-bit shell** for all steps.
- If SDL2 is not found at compile-time, re-check your installed packages.
- Make sure `main.c` uses the proper header:

```c
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
```

---

## 🧪 Troubleshooting

### ❌ `undefined reference to SDL_...`

This means you’re missing the SDL2/SDL2_image libraries. Make sure you installed them:

```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image
```

### ❌ `icon.res: No such file`

Ensure `icon.rc` is compiled like this:

```bash
windres icon.rc -o icon.res
```

---

## 👨‍💻 Author

**🧑‍🏫 Developed for UCSC — customized C-based SDL2 project.**  
You may extend it to include dice, multiplayer support, network play, or animations.

---

## 📜 License

MIT License. Use it for fun or for learning — feel free to fork and improve!

---

## 🌟 Give it a Star!

If you enjoyed this project, give it a ⭐ on GitHub!
