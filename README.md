# C-C-Learning-Application
# C² Learn (C Square Learn)

**Learn C and C++ by riding the rollercoaster.**

C Square Learn is a desktop learning app written in **C** with **[Raylib](https://www.raylib.com/)**. It helps students and curious learners prepare for coding assessments and sharpen their problem solving. Each topic is a level on a rollercoaster. Pass the challenges and the coaster carries you to the next one.

> Status: early development. The project has just started, so the roadmap below shows what is planned, not what is finished.

## Features

| Feature | Description |
| --- | --- |
| Welcome loading screen | Animated C² logo shown when the app starts |
| Rollercoaster progression | A level map where each topic is a stop and progress moves the coaster forward |
| Practice challenges | Multiple choice, fill in the blank, spot the bug and short code questions |
| Hint bot | A helper that gives step by step hints without handing over the full answer |
| Personal accounts | Create a profile with your name and details, saved to a CSV file |
| Puzzle games | Games where your C/C++ knowledge is the key to winning |

## Topics Covered

The content follows the Advanced Programming syllabus and the lab exam questions, and goes beyond them.

1. Programming fundamentals (loops, functions, arrays, strings)
2. Lab exam problems (leap year, palindromes, primes, binary to decimal, patterns, array operations, word counting)
3. Pointers and pointer arithmetic
4. Memory management (stack vs heap, `new`, `delete`, leaks)
5. Structs
6. Classes and object oriented programming
7. Linked lists
8. Templates
9. The STL: vectors, lists and maps

## Project Structure

```text
C-C-Learning-Application/
├── CMakeLists.txt      Build configuration (fetches Raylib 5.5)
├── main.c              Application entry point
├── README.md
├── src/                Application source code (planned)
├── assets/             Logo, fonts, images and sounds (planned)
├── data/               Lesson and question files, user accounts CSV (planned)
└── examples/           C++ study files used as lesson material
    ├── ArrayWalking.cpp
    ├── CppObjectOriented.cpp
    ├── CppPointers.cpp
    ├── CppStruct.cpp
    ├── LinkedListCpp.cpp
    └── Lab1.cpp
```

## Getting Started

### Requirements

* A C compiler (GCC, Clang or MSVC)
* [CMake](https://cmake.org/) 3.20 or newer
* Git
* An internet connection for the first build, because CMake downloads Raylib

On Linux you may also need the Raylib system dependencies (X11, OpenGL and ALSA development packages). See the [Raylib build notes](https://github.com/raysan5/raylib/wiki) for your platform.

### Build and Run

```bash
git clone https://github.com/<your-username>/C-C-Learning-Application.git
cd C-C-Learning-Application

cmake -B build
cmake --build build

./build/C_C___Learning_Application
```

On Windows the executable is in `build\Debug` or `build\Release`, depending on the generator.

## User Accounts

Profiles are stored in a plain CSV file so they are easy to read, back up and debug. A planned layout:

```text
id,name,email,course,level,score,last_login
1,Oyindamola,example@email.com,Advanced Programming,3,420,2026-11-05
```

Only data needed for the app is stored, and it stays on your own machine.

## Roadmap

- [ ] Window, loading screen and main menu
- [ ] Account creation, login and CSV saving
- [ ] Rollercoaster level map
- [ ] Question screen and answer checking
- [ ] Hint bot
- [ ] Pointers and linked lists levels
- [ ] Memory management, classes, templates and STL levels
- [ ] Puzzle games
- [ ] Polish, testing and documentation

## Tech Stack

* **Language:** C (C11)
* **Graphics and input:** Raylib 6.2
* **Build system:** CMake with FetchContent
* **Data storage:** CSV files

## Contributing

Ideas and suggestions are welcome. Open an issue to discuss a change before sending a pull request.

## Author

**Oyindamola Olaosun**
Student ID: C00313475
**Owen Nathanael**
Student ID: C00313648
Module: Advanced Programming

## Acknowledgements

* [Raylib](https://github.com/raysan5/raylib) by Ramon Santamaria and contributors
