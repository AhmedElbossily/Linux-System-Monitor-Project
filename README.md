# Linux System Monitor

![System Monitor](images/monitor.png)

- This project builds a **system monitor** that resembles [Htop](https://en.wikipedia.org/wiki/Htop) program to monitor **Linux OS**.
- It uses [ncurses] library (https://www.gnu.org/software/ncurses/) to facilitate text-based graphical output in the terminal. 

## Make
This project uses [Make](https://www.gnu.org/software/make/). The Makefile has four targets:
* `build` compiles the source code and generates an executable
* `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
* `debug` compiles the source code and generates an executable, including debugging symbols
* `clean` deletes the `build/` directory, including all of the build artifacts
