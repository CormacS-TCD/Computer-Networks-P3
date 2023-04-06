README FILE

Setup:
-A linux environment of WSL 1 is preferred for testing, for both running the clients and the server
on a single computer or for running all three files on separate machines.
-A linux environment of WSL 2 may be used for testing ONLY if the computer running WSL 2 is the 
server, and computers of WSL 1 are the clients. This is due to an IP issue between WSL 1 and WSL 2.
-A windows environment is not recommended.

It is recommended that the code be compiled and run in Visual Studio Code. The recommended compiler 
is GCC, if you do not have this compiler and the relevant c/c++ extensions enabled on Visual Studio Code,
follow this guide here to set that up: https://code.visualstudio.com/docs/cpp/config-mingw

Once vscode and GCC are setup, the IP addresses for the files must be configured.
NOTE: In any instance, every file's IP address will be the IP address of the server file computer.
-If you're using all three files on one computer, open the windows command prompt and use the command "ipconfig".
-Copy the value beside IPv4 Address into each file for the variable ip. The format for the IP should be "10.23.22.1".

-If you're using the server on WSL 1 and the clients on WSL 2, open the windows command prompt on the WSL 1 computer
and use the command "ipconfig".
-Copy the value beside IPv4 Address into each file for the variable ip. The format for the IP should be "xx.xx.xx.xx".

Compiling:
-Once the setup is complete, open vscode and start up WSL. 
-Open up the terminal in vscode and use the cd command to navigate the file path to the folder of the server. 
-Open a second terminal and drag the second terminal beside the first, creating a split terminal.
-Navigate the file path of the second terminal to the folder of ClientA
-Repeat the previous two steps until there are three split terminals that each access one of the three folders.

-Use the command "gcc server.c -o server" in the server terminal to compile the server.c file.
-Use the command "gcc clientA.c -o clientA" in the clientA terminal to compile the clientA.c file.
-Use the command "gcc clientB.c -o clientB" in the clientB terminal to compile the clientB.c file.

Running:
ALWAYS start the server first, as it needs to be running for the clients to connect to it.

-Use "./server" in the server terminal to start the server up.
-Use "./clientA" in the clientA terminal to start up clientA. It should connect almost immediately to the server.
-Use "./clientB" in the clientB terminal to start up clientB. It should connect almost immediately to the server.

Test Cases:
There are no explicit test cases for the code, as the only unique difference between each use of the network is the
contents of the clients' files. These files may be opened and edited easily.

