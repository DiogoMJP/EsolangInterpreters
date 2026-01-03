@echo off
gcc -D DEBUG ./src/state_machine_loader.c ./src/state_machine_buffer.c ./src/state_machine.c -Iinclude -o ./dest/state_machine_loader.exe