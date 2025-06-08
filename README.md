# LUDO like UCSC

C Language LUDO game unique for UCSC


**🔧 Step-by-Step Fix**

**Download and Install MSYS2**
If not installed, download from:
👉 <https://www.msys2.org>

**Run the Correct Shell**
After installing MSYS2, launch:
MSYS2 MinGW 64-bit (not just the regular MSYS shell).
You can find this in the Start Menu under "MSYS2 64bit".

**Update MSYS2 (Optional but Recommended)**
Run these in the MSYS2 shell:
pacman -Syu

**Then close the window and re-open the same shell, and run:**
pacman -Su

**Install SDL2 and SDL2_image**
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image

**Compile with icon, SDL2 and SDL2_image**
gcc sdl.c icon.res -o sdl.exe -lSDL2 -lSDL2_image