
#TODO
- session behaviour
- more message types
- version byte

compile for testing:
gcc server.c -o server.exe -lws2_32 -Wall -Wextra
.\server.exe
gcc client.c -o client.exe -lws2_32 -Wall -Wextra
.\client.exe