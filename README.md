#SweeperSlop
Sweeperslop is my working title name for a web game that will be based on minesweeper. It is currently early in development and there is much to come. I have built this project in C using the Raylib game development library. It targets the web as a first-class citizen build target, and the project's infrastructure is built to support the web, but it also has a linux-native build target that I use to simplify testing. 

## Build and library information

Raylib is written by Ramon Santamaria, and is distributed as is. It is not included with this github repository.

this repo on it's own isn't everything you need to build. It doesn't include tools, and it also doesn't include any libraries. To build this project, you need the following:

- The Emscripten and/or GCC toolchain
- A copy of raylib for your desired build targets
    - Place the three raylib headers in the include/ directory. They aren't platform specific because they're just headers, so they work for every platform.
    - For each platform you wish to compile for, place the Raylib libraries compiled for that platform in build/target/lib, where target is the platform you're targeting. I currently have it configured to build using the shared library file and to configure the executable to look for libary files in "../lib/" at runtime (via -Wl,--rpath=../lib/). In the future, I may configure support for the static library instead (I had too many errors with it on some systems so I gave up after a little while).
- Make

to get this project ready for build, you need to do the following:

1. Download or otherwise obtain your desired release of Raylib for whatever platform you want to compile for
2. Create an "include" directory in the root of this project and put raylib's .h headers in there
3. Run "make setup" to set up th build/ directory. It'll create empty folders for each platform.
4. For your desired target platform within build/, put the static Raylib library in the lib/ subdirectory.
5. To build for the web, just run "make". To build for Linux, run "make linux_amd64"

To run the built projects, I wrote shell scripts for the various platforms. I understand I can write makefile rules to do this too, and I may investigate this option more in the future. For some special functionality, you may need to run the program manually rather than via shell script.

To run the linux-native build: simply navigate to the folder where the binary is contained, and run ./sweeperslop_linux_amd64.

To run the webassembly build: navigate to the "build/emscripten/bin", and run a web server program pointing to that directory. Any web server of your choosing should do. My script defaults to a web server integrated into python3 (uses the following command: `python3 -m http.server 8000` to run a python webserver on port 8000). Then, open your web browser and point it to "localhost:8000" as the address (or, replace 8000 with whatever port you chose for the web server).

Just a heads up: you cannot load your webassembly program directly into the web browser without running a webserver program to host it. This is a limitation imposed into web browsers for security reasons.
