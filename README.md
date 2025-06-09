**BEFORE BUILDING/RUNNING**\
-Make sure to have the x86_64 elf gnu toolchain in your path\
-Install VirtualBox\
-Install Qemu (optional)\
-Install qemu_img (required)\
-Run
```
make first_time
```
to initialize\
\
**DEBUGGING**\
Run 
```
make target
```
to compile a debug build and start VirtualBox' GDB stub\
\
Run
```
make debug
```
on another terminal screen to run gdb and connect to the GDB stub
