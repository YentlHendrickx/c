# Learning C

## Overview
This repository is a collection of projects designed to help learn and practice the C programming language. It includes fundamental data structures like linked lists and stacks, as well as graphical programs using SDL, such as Conway's Game of Life and a bouncing ball simulation.

## Directory Structure
Each directory contains a C file implementing a specific feature or project, along with a `Makefile` for easy compilation.

### Projects Included:
- **Linked List Implementation**
  - A basic singly linked list implementation with insert, delete, and display operations.
- **Stack Implementation**
  - A simple stack implementation using an array and/or linked list.
- **SDL Game of Life**
  - A graphical implementation of Conway's Game of Life using SDL.
- **Bouncing Ball**
  - A simple physics simulation of a bouncing ball using SDL.

## Requirements
To build and run these projects, you need:
- A C compiler (`gcc` recommended)
- `make`
- SDL2 library (for graphical projects)

On Debian/Ubuntu, install SDL2 with:
```bash
sudo apt install libsdl2-dev
```
On macOS, use Homebrew:
```bash
brew install sdl2
```

## Compilation & Execution
Navigate into the desired project directory and run:
```bash
make
```
To execute the compiled program:
```bash
./program_name
```
To clean up compiled files:
```bash
make clean
```

## Contributing
Feel free to contribute improvements or additional exercises by submitting a pull request.

## License
This project is open-source and available under the MIT License.
