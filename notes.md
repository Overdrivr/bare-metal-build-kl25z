### TODOS

* Run the programs (flash a board with it ?)
* Build for a specific platform
* https://mcuoneclipse.com/2015/03/25/command-line-programming-and-debugging-with-gdb/
* https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html

### Resources

* http://www.embedded.com/design/mcus-processors-and-socs/4007119/Building-Bare-Metal-ARM-Systems-with-GNU-Part-1--Getting-Started
* http://gnuarmeclipse.github.io/tutorials/blinky-arm/
# Installation
1. Install ninja
2. Install arm-gcc
3. Type `arm-none-eabi-g++ -v` and `ninja -v` to make sure it is properly set in the path
4. Let's write the first build script

# Explore the toolchain

```
arm-none-eabi-g++ --help
```

# First build

Create a file `./main/main.cpp`

```
int main()
{
  return 0;
}
```

Let's create a `build.ninja` file

```
cflags = -Wall

rule cc
  command = arm-none-eabi-g++ $cflags -c $in -o $out

build build/main.o : cc main/main.c

```

Run it

```
ninja
```

At this point, no big deal. We are calling the ARM version of gcc, using the
`-Wall` flag. When calling gcc, we first provide the -c flag to tell it to
compile `$in`, then `-o` to tell it to link into `$out` file.

At this point, the program is compiled and linked, but not archived into a .elf file.

Let's add a second command for it. Archiving the `.o` files is done with
`arm-none-eabi-ar` program.

```
cflags = -Wall

rule cc
  command = arm-none-eabi-g++ $cflags -c $in -o $out

rule ar
  command = arm-none-eabi-ar -rcs $out $in

build build/foo.o : cc main/main.c

build build/main.exe : ar build/foo.o
```

We have added the rule `ar` that executes the packaging of our application.
We provided the `-rcs` flags for ... ?

If you look into the `./build` folder, you should find a `main.elf` file.
Congratulations ! You have archived your first embedded program without opening any IDE.

# Adding a dependency

Our main function is for now, empty. Let's make it some computations

```
int main()
{
  uint8_t x = 0xEA;
  uint16_t y = 234;
  uint16_t z = x - y
  return z;
}
```

```
ninja
```
Running this command gives the following error

```
FAILED: arm-none-eabi-g++ -Wall -c main/main.cpp -o build/foo.o
main/main.cpp: In function 'int main()':
main/main.cpp:5:3: error: 'uint8_t' was not declared in this scope
   uint8_t x = 0xEA;
   ^
main/main.cpp:6:3: error: 'uint16_t' was not declared in this scope
   uint16_t y = 234;
   ^
main/main.cpp:7:12: error: expected ';' before 'z'
   uint16_t z = x - y
            ^
ninja: build stopped: subcommand failed.
```

This is normal, this is because the fixed-size integers are not part of the
core C library. They are however, a must-have for embedded computations.

Let's add the library `stdint.h`, provided with the `GNU ARM Embedded` toolchain.

```
#include "stdint.h"

int main()
{
  uint8_t x = 0xEA;
  uint16_t y = 234;
  uint16_t z = x - y
  return z;
}
```
The build should now pass.
Ninja handled the include without us having to modify our build (normal, no ?).

# Build for an actual board and flash

For now, the archive is useless.
It is build for an ARM processor, but doesnt contain startup info, etc.

Let's add this files to do it

From Kinetis SDK 1.3

Copy `core_m0plus.h` to `platform/MKL25Z4/include`

Now complains that core_cmInstr is missing, but this file is imported with `<>`.
So need to add the file to a local folder and add that folder to search path.

Modify build.ninja to add the `/core` subfolder to search path.

```
cflags = -Wall -Icore -mcpucortex-m0plus

rule cc
  command = arm-none-eabi-g++ $cflags -c $in -o $out

rule ar
  command = arm-none-eabi-ar -rcs $out $in

build build/foo.o : cc main/main.cpp

build build/main.elf : ar build/foo.o
```

Also missing, core_cmFunc.h and core_cmInstr.h. Do the same.

Now, error

```
FAILED: arm-none-eabi-g++ -Wall -Icore -c main/main.cpp -o build/foo.o
In file included from main/main.cpp:2:0:
main/../platform/MKL25Z4/include/MKL25Z4.h:199:80: fatal error: system_MKL25Z4.h
: No such file or directory
 #include "system_MKL25Z4.h"            /* Device specific configuration file */
```

Move the missing file to the same folder than `MKL25Z4.h`.

Build succeeds. But we are not done yet.
We haven't specified an cpu to the toolchain for which to build the code.

We will use the `-mcpu` compiler flag to tell the toolchain to build for a cortex M0+ (which is the core used in KL25Z).

Modify the build.ninja

```
cflags = -Wall -Icore -mcpu=cortex-m0plus

rule cc
  command = arm-none-eabi-g++ $cflags -c $in -o $out

rule ar
  command = arm-none-eabi-ar -rcs $out $in

build build/foo.o : cc main/main.cpp

build build/main.elf : ar build/foo.o
```

error

```
FAILED: arm-none-eabi-g++ -Wall -Icore -mcpu=cortex-m0plus -c main/main.cpp -o b
uild/foo.o
main/main.cpp:1:0: error: target CPU does not support ARM mode
 #include "stdint.h"
 ^
ninja: build stopped: subcommand failed.
```

# Adding a local library

Now, we are going to use the library defined in `./mylib`.
Let's modify the main file again.
