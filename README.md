# TCP Socket Client Side

## Usage

### 1) WSAStartup function

Initialize WinSock, specify the Socket library version used

```c++
int WSAStartup( WORD wVersionRequested, LPWSADATA lpWSAData );
```

Programs using Socket must call the WSAStartup function before using Socket. The first parameter of this function indicates the Socket version requested by the program. The high byte indicates the minor version and the low byte indicates the major version. The operating system uses the second parameter to return the requested socket version. When an application calls the WSAStartup function, the operating system searches for the corresponding Socket library according to the requested Socket version, and then binds the found Socket library to the application. Later, the application can call other Socket functions in the requested Socket library. The function returns 0 after successful execution.

Example: If a program uses Socket version 2.2, the program code is as follows

```c++
wVersionRequested = MAKEWORD( 2, 2 );

ret = WSAStartup( wVersionRequested, &wsaData );
```

### 2) Create a socket
```c++
SOCKET socket( int af, int type, int protocol );
```
The application calls the socket function to create a socket capable of network communication. The first parameter specifies the protocol family of the communication protocol used by the application. For the TCP/IP protocol family, this parameter is set to PF_INET; the second parameter specifies the type of socket to be created. The stream socket type is SOCK_STREAM, datagram The socket type is SOCK_DGRAM; the third parameter specifies the communication protocol used by the application. This function returns the descriptor of the newly created socket if the call succeeds, and returns INVALID_SOCKET if it fails.

The socket descriptor is an integer value. There is a socket descriptor table in the process space of each process, which stores the corresponding relationship between the socket descriptor and the socket data structure. There is a field in the table to store the descriptor of the newly created socket, and the other field to store the address of the socket data structure, so the corresponding socket data structure can be found according to the socket descriptor. Each process has a socket descriptor table in its own process space, but the socket data structure is in the kernel buffer of the operating system.

### 3) Send a connection request to the server
```c++
int connect( SOCKET s, const struct sockaddr FAR *name, int namelen );
```
The client program calls the connect function to connect the client Socket s with the service Socket listening on the specific port of the computer specified by name. If the connection is successful, connect returns 0; if it fails, it returns SOCKET_ERROR.


### 4) Communicate with the server (send/recv)

send function

```c++
int send( SOCKET s, const char FAR *buf, int len, int flags );
```

Both client and server applications use the send function to send data to the other end of the TCP connection. The client program usually uses the send function to send requests to the server, and the server usually uses the send function to send responses to the client program.

The first parameter of this function specifies the sender socket descriptor; the second parameter specifies a buffer for storing the data to be sent by the application; the third parameter specifies the number of bytes of data to be sent; the fourth The parameter is generally set to 0.

Here only describes the execution process of the send function of the synchronous Socket. When calling this function, Send first compares the length of the data to be sent (len) with the length of the send buffer of the socket (s).

If len is greater than the length of the send buffer of s, the function returns SOCKET_ERROR; if len is less than or equal to the length of the send buffer of s, then send first checks whether the protocol is sending the data in the buffer of s, if it is, wait for the protocol Send the data.

If the protocol has not yet started to send the data in the send buffer of s or there is no data in the send buffer of s, then Send compares the remaining space of the send buffer of s with len. If len is greater than the remaining space, Send will wait for the protocol to transfer s. The data in the sending buffer is sent.

If len is less than the size of the remaining space, Send will only copy the data in buf to the remaining space (note that it is not Send that transmits the data in the send buffer of s to the other end of the connection, but the protocol. Send only Copies the data in buf to the remaining space of the send buffer of s).

If the send function copy data is successful, it returns the actual number of bytes copied. If send has an error while copying data, then send returns SOCKET_ERROR; if send is waiting for the protocol to transmit data while the network is disconnected, then the send function also returns SOCKET_ERROR .

It should be noted that the send function successfully copies the data in buf to the remaining space of the send buffer of s and then returns, but at this time these data may not be transmitted to the other end of the connection immediately. If a network error occurs during the subsequent transmission of the protocol, the next Socket function will return SOCKET_ERROR. (Each Socket function except send must first wait for the data in the socket's send buffer to be transmitted by the protocol before continuing. If a network error occurs while waiting, the Socket function will return SOCKET_ERROR)

### recv function
```c++
int recv( SOCKET s, char FAR *buf, int len, int flags );
```
Both client and server applications use the recv function to receive data from the other end of the TCP connection. The first parameter of this function specifies the receiver socket descriptor; the second parameter specifies a buffer, which is used to store the data received by the recv function; the third parameter specifies the length of buf; the fourth parameter is generally set to 0.

Here only describes the execution flow of the recv function of the synchronous Socket. When the application program calls the recv function, recv first waits for the data in the send buffer of s to be transmitted by the protocol. If the protocol has a network error while transmitting the data in the send buffer of s, then the recv function returns SOCKET_ERROR,

If there is no data in the sending buffer of s or the data is successfully sent by the protocol, recv first checks the receiving buffer of the socket s. If there is no data in the receiving buffer of s or the protocol is receiving data, then recv will wait until The protocol has received the data.

When the protocol receives the data, the recv function copies the data in the receiving buffer of s to buf (note that the data received by the protocol may be larger than the length of buf, so in this case, you need to call the recv function several times to get The data in the receiving buffer of s copied. The recv function is just to copy the data, and the actual received data is completed by the protocol. The recv function returns the number of bytes actually copied.

If recv makes an error during copy, it returns SOCKET_ERROR; if the recv function is waiting for the protocol to receive data while the network is interrupted, then it returns 0.

Note: In the Unix system, if the recv function is waiting for the protocol to receive data while the network is disconnected, the process that calls recv will receive a SIGPIPE signal, and the default processing of the signal is process termination.


### 5) Close the socket and release resources
```c++
int closesocket( SOCKET s );
```
The closesocket function is used to close a socket with descriptor s. Since each process has a socket descriptor table, each socket descriptor in the table corresponds to a socket data structure in the operating system buffer, so there may be several sockets descriptor points to the same socket data structure.

There is a special field in the socket data structure to store the number of references of the structure, that is, how many socket descriptors point to the structure. When the closesocket function is called, the operating system first checks the value of this field in the socket data structure.

If it is 1, it indicates that there is only one socket descriptor pointing to it, so the operating system first clears the entry corresponding to s in the socket descriptor table, and releases the socket data structure corresponding to s; If the field is greater than 1, then the operating system only clears the corresponding entry of s in the socket descriptor table, and subtracts 1 from the number of references to the socket data structure corresponding to s.

The closesocket function returns 0 if it executes successfully, otherwise it returns SOCKET_ERROR.

```c++
int WSACleanup (void);
```

After the application completes the use of the requested Socket library, it must call the WSACleanup function to unbind the Socket library and release the system resources occupied by the Socket library.