#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <ctype.h>
#include <math.h>

//For linux
#include <arpa/inet.h>

//For Windows
//#include <Winsock2.h>
//#include <WS2tcpip.h>

#define SIZE 2048

char bufferA[SIZE];
char bufferB[SIZE];
char bufferSend[SIZE] = {0};

int serverKey = 79;
int clientAKey = 0;
int clientBKey = 0;

int FastExponention(int bit, int n, int* y, int* a)
{
    if (bit == 1) {
        *y = (*y * (*a)) % n;
    }
 
    *a = (*a) * (*a) % n;
}


int FindT(int a, int m, int n)
{

    int r;
    int y = 1;
 
    while (m > 0)
    {
        r = m % 2;
        FastExponention(r, n, &y, &a);
        m = m / 2;
    }
    return y;
}

int Encryption(int value, FILE* out)
{
    int e = 2997;
    int n = 20131;
    int cipher;
    cipher = FindT(value, e, n);
    fprintf(out, "%d ", cipher);
}

int Decryption(int value, FILE* out)
{
    int d = 10333;
    int n = 20131;
    int decipher;
    decipher = FindT(value, d, n);
    fprintf(out, "%c", decipher);
}

 
void Encode (int key1, int key2, char * buf, char * begin) {
    // encryption starts
    
    FILE *inp;
    FILE *out;
    
    out = fopen("interen.txt", "w+");
    fprintf(out, "%s", buf);
    fclose(out);
    
    
    inp = fopen("interen.txt", "r+");
    if (inp == NULL)
    {
        printf("Error opening Source File.\n");
        exit(1);
    }
 
    out = fopen("cipher.txt", "w+");
    if (out == NULL)
    {
        printf("Error opening Destination File.\n");
        exit(1);
    }
    while (1)
    {
        char ch = getc(inp);
        if (ch == -1) {
            break;
        }
        int value = toascii(ch);
        Encryption(value, out);
    }
    fclose(inp);
    fclose(out);

    out = fopen("cipher.txt", "r");
    fread(bufferSend, 1, 1000, out);
    fclose(out);

}


void Decode (int key1, int key2, char * buf, char * begin) {

   FILE *inp;
   FILE *out;
    // decryption starts
    out = fopen("interde.txt", "w+");
    fprintf(out, "%s", buf);
    fclose(out);
  	
    memset( buf, '\0', strlen(bufferA));
    inp = fopen("interde.txt", "r");
    if (inp == NULL)
    {
        printf("Error opening Cipher Text.\n");
        exit(1);
    }


    out = fopen("decipher.txt", "w+");
    if (out == NULL)
    {
        printf("Error opening File.\n");
        exit(1);
    }

    while (1)
    {
        int cip;
        if (fscanf(inp, "%d", &cip) == -1) {
            break;
        }
        Decryption(cip, out);
    }
    fclose(out);
    fclose(inp);
 
    out = fopen("decipher.txt", "r");
    fread(bufferA, 1, 1000, out);
    fclose(out);
    }
    
    
//Takes a socket and reads incoming data into a file from it, data coming from client A
void write_fileA(int sockfd) {
    int n;
    FILE *fp;
    char *filename = "fileStorageA.txt";

    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error in writing file.");
        exit(1);  
    }

    n = recv(sockfd, bufferA, SIZE, 0);
    char*begin = &bufferA[0];

    Decode(serverKey, clientAKey, bufferA, begin);
    fprintf(fp, "%s", bufferA);
    bzero(bufferA, SIZE);
    fclose(fp);

    return;
}

//Takes a socket and reads incoming data into a file from it, data coming from client A
void write_fileB(int sockfd) {
    int n;
    FILE *fp;
    char *filename = "fileStorageB.txt";

    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error in writing file.");
        exit(1);  
    }

    n = recv(sockfd, bufferB, SIZE, 0);
    char*begin = &bufferB[0];

    Decode(serverKey, clientBKey, bufferB, begin);
    fprintf(fp, "%s", bufferB);
    bzero(bufferB, SIZE);
    fclose(fp);

    return;
}

//Takes a file and a socket and sends the file out of the socket
void send_file(FILE *fp, int sockfd) {

    while (fgets(bufferSend, SIZE, fp) != NULL) {
        char*begin = &bufferSend[0];
        if (sockfd == 4) {   
            Encode(serverKey, clientAKey, bufferSend, begin);
        }
        else if (sockfd == 5) {
            Encode(serverKey, clientBKey, bufferSend, begin);
        }

        if (send(sockfd, bufferSend, sizeof(bufferSend), 0) == -1) {
            perror("Error in sending data");
            exit(1);
        }
        bzero(bufferSend, SIZE);
    }
}

int main() {

    char *ip = "134.226.44.171";
    int port = 33338;
    int e;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    //File variables for client A
    FILE *fpA;
    char *filenameA ="fileStorageA.txt";

    //File variables for client B
    FILE *fpB;
    char *filenameB ="fileStorageB.txt";

    //Create the listening socket and check for errors
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error in socket.");
        exit(1);
    }
    printf("Server socket created.\n");

    //Set up the port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //Bind the socket to the port and check for errors
    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0) {
        perror("Error in binding");
        exit(1);
    }
    printf("Binding successful.\n");

    //Set the listening port to start listening for connections, also check for errors
    e = listen(sockfd, 10);
    if (e == 0) {
        printf("Listening...\n");
    }
    else {
        perror("Error in listening");
        exit(1);
    }

    //Establish a master socket set for ysing select()
    fd_set master;
    
    //Zero the set
    FD_ZERO(&master);

    //Add the listening socket to the set
    FD_SET(sockfd, &master);

    //Server Interface

    //Declare variables for running the server interface
    int flag = 1;

    int sig= 999;

    fd_set copy = master;

    int socketCount, nbytes = 0;

    //Set the maximum socket as the listening socket (the only socket)
    int fd_maxi = sockfd;

    //Server loop to process requests
    while (flag == 1) {
        //sleep(1);

        //Make a copy of the master set
        copy = master;

        //Use select to watch all the active sockets
        socketCount = select(fd_maxi+1, &copy, NULL, NULL, NULL);

        //Loop through values of i up until the biggest socket
        for (int i = 0; i <= fd_maxi; i++) {

            //Check if they're active sockets
            if (FD_ISSET(i, &copy)) {

                //If the socket is the listener, a new client is trying to connect
                if (i == sockfd) {
                    
                    //Add the client as a new socket for the server
                    addr_size = sizeof(new_addr);
                    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);

                    printf("Socket %i connected.\n", new_sock);
                    int m;

                    //Receive the client key
                     if (new_sock == 4) {
                        m = recv(new_sock, &clientAKey, sizeof(clientAKey), 0);
                        printf("Received security key %i.\n", clientAKey);
                    }
                    else if (new_sock == 5) {
                        m = recv(new_sock, &clientBKey, sizeof(clientBKey), 0);
                        printf("Received security key %i.\n", clientBKey);
                    }

                    //Send the server key
                    send(new_sock, &serverKey, sizeof(serverKey), 0);
                    printf("Sent security key %i.\n", serverKey);
                    //sleep(0.5);

                    //Check for errors, if no errors then add the socket to the socket set and edit the max socket if necessary
                    if (new_sock == -1) {
                        perror("accept");
                    } 
                    else {
                        FD_SET(new_sock, &master); // add to master set
                        if (new_sock > fd_maxi) {    // keep track of the max
                            fd_maxi = new_sock;
                        }
                    }
                }

                //If the socket isn't the listener, its a client trying to send a message
                else {

                    //If there is a signal to be received, receive it
                    if ((nbytes = recv(i, &sig, sizeof(sig), 0)) > 0) {
                        
                        //This first signal will be a command to the server from a client

                        //printf("Signal: %i from socket: %i\n", signal, i);

                        //Start the switch for the various possible commands from the server
                        switch (sig) {
                            default:
                                break;

                            //Requesting data - server must send available file, check which client is requesting data
                            case 1:
                                //sleep(1);

                                //Check which client is sending this command, open the appropriate file and send it to them
                                if (i == 4) {
                                    fpA = fopen(filenameB, "r");
                                    if (fpA == NULL) {
                                        perror("Error in reading file.");
                                        exit(1);  
                                    }

                                    send_file(fpA, i);
                                    printf("File data send successfully to socket: %i.\n", i);
                                    fclose(fpA);
                                    }
                                else if (i == 5) {
                                    fpB = fopen(filenameA, "r");
                                    if (fpB == NULL) {
                                        perror("Error in reading file.");
                                        exit(1);  
                                    }

                                    send_file(fpB, i);
                                    printf("File data send successfully to socket: %i.\n", i);
                                    fclose(fpB);
                                }

                                break;

                            //Sending data - server must accept incoming file, and check which client is sending the file
                            case 2:

                                //Check which client sending this command, store the file appropriately
                                if (i == 4) {
                                    write_fileA(i);
                                    printf("Data written in the text file from socket: %i.\n", i);
                                }
                                else if (i == 5) {
                                    write_fileB(i);
                                    printf("Data written in the text file from socket: %i.\n", i);
                                }

                                break;

                            //Client wants to disconnect - kill the server
                            //NOTE: RIGHT NOW THIS ONLY KILLS THE CLIENT COMMANDING THIS AND THE SERVER, SECOND CLIENT IS LEFT RUNNING
                            case 3:
                            printf("Client disconnect requested.\n");
                            FD_CLR(i, &master);
                            close(i);
                            //flag = 0;
                            printf("Client connection on socket %i terminated.\n", i);
                            break;

                        }
                    }
                }
            }
        }  
    }

    return 0;
}
