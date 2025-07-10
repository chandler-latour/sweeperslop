# The goal of this makefile is to function for both native web assembly builds and native Linux builds
# perhaps it can be expanded later on with win32 or mac support
# Web is intended as the first class citizen, so the default build target will be index.html
# however, there will be an alternate target for Linux native and switching to this target will require different resources.

# Write the working directories to objects and libraries to target-specific variables for each target
build/emscripten/bin/index.html: dir_plat = ./build/emscripten
build/emscripten/bin/index.html: dir_obj = ./build/emscripten/obj
build/emscripten/bin/index.html: dir_lib = ./build/emscripten/lib

#This is the phony label for the Linux build target, not the real file.
#Simplifies writing the requirements and variables for it.
.PHONY: linux_amd64

linux_amd64: dir_plat = ./build/linux_amd64/
linux_amd64: dir_obj = ./build/linux_amd64/obj
linux_amd64: dir_lib = ./build/linux_amd64/lib

# A variable of all the desired object files
# To add a new object file to the build, just add it to this list in this format, 
# then add it's rule below near the bottom of the file, 
# and be sure to amend the requirements of other files that may now require this new object/source file.
# THIS IS THE PART TO CHANGE WHEN ADDING NEW SOURCE FILES

files_obj_raw = main.o bitmap.o minefield.o gamestate.o gs_classic.o

# variables for the cc and flags
# flags change depending on whether we're compiling for native or the web browser
# more specifically, EMCC demands a LOT of flags. GCC, not so much.
build/emscripten/bin/index.html: cc = emcc
linux_amd64: cc = gcc
# neither emcc nor gcc requires any extra flags than the shared ones for individual object files.
cc_flags = -DPLATFORM_WEB 
# the final flags are only required for the final target, and not for individual object files.
build/emscripten/bin/index.html: cc_flags_final = -o $(dir_plat)/bin/index.html -L$(dir_lib) -s USE_GLFW=3 -DPLATFORM_WEB $(dir_lib)/libraylib.a --preload-file assets/
linux_amd64: cc_flags_final = -o $(dir_plat)/bin/sweeperslop_linux_amd64 -L$(dir_plat)/lib -lm -lraylib -Wl,--rpath=../lib/
linux_amd64: cc_flags = -g
# some "shared" variables, meaning they're supposed to stay the same regardless of compiler/target file in general, 
# but I might change them for entirely unrelated purposes.

flags_shared = -Wall -I./include/
flags_shared_final = 

# An implicit compile rule for object file targets.
# Make already has a very basic implicit rule for compiling object files from their source, but I considered
# it inadequate for my needs. I wish to separate my source and object files into different directories,
# and I needed to include the include folder that has the headers I need for raylib.

build/emscripten/bin/index.html: files_obj = $(addprefix $(dir_obj)/, $(files_obj_raw))
linux_amd64: files_obj = $(addprefix $(dir_obj)/, $(files_obj_raw))

build/emscripten/obj/%.o : ./src/%.c 
	$(cc) $(flags_shared) $(cc_flags) -c $^ -o $@

build/linux_amd64/obj/%.o: ./src/%.c
	$(cc) $(flags_shared) $(cc_flags) -c $^ -o $@

# This is the default target of the file, the final goal. It compiles a binary for web assembly.
# Index.html is where my (prototype) build of the game lives.
# If I compile this, I need to host a web server running this page and I can connect to it to play the game.
build/emscripten/bin/index.html: $(addprefix build/emscripten/obj/, $(files_obj_raw))
	echo $(dir_plat)
	echo $(dir_obj)
	echo $(files_obj)
	$(cc) $(flags_shared) $(flags_shared_final) $(cc_flags) $(cc_flags_final) $(files_obj)

linux_amd64: build/linux_amd64/bin/sweeperslop_linux_amd64

#Here's the REAL (non-phony) build target for the Linux executable
build/linux_amd64/bin/sweeperslop_linux_amd64: $(addprefix build/linux_amd64/obj/, $(files_obj_raw))
	echo $(dir_plat)
	echo $(dir_obj)
	echo $(files_obj)
	$(cc) $(flags_shared) $(files_obj) $(flags_shared_final) $(cc_flags) $(cc_flags_final) 
	cp -r ./assets build/linux_amd64/

# object file definitions and rerequisites. Ideally, most if not all of these should just use the implicit rule.
$(dir_obj)/main.o: src/main.c
$(dir_obj)/minefield.o: src/minfield.c
$(dir_obj)/bitmap.o: src/bitmap.c
$(dir_obj)/gamestate.o: src/gamestate.c
$(dir_obj)/gs_classic.o: src/gs_classic.c



#You can see that main.o doesn't have any instructions for fulfilling the target. That's because it uses the implicit rule.

#Commented these out because it kept running stuff automatically after compiling and I didn't like it very much
#I'll figure it out later (TODO)
#run web:
#	./run_web.sh & disown;

#run linux_amd64:
#	./run_linux_amd64.sh & disown;

clean: 
	rm build/*/obj/*.o build/*/bin/*
	rm -r build/linux_amd64/assets
#todo: make the cleanup clean up ALL POSSIBLE obj and bin directories.

setup:
	mkdir include
	mkdir build
	mkdir build/emscripten
	mkdir build/emscripten/obj
	mkdir build/emscripten/lib
	mkdir build/emscripten/bin
	mkdir build/linux_amd64/
	mkdir build/linux_amd64/obj
	mkdir build/linux_amd64/lib
	mkdir build/linux_amd64/bin

