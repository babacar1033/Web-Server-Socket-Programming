#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"


// Maximum number of pending requests is 20
#define BACKLOG 20
// Maximum size of GET request and the maximum size of the GET response header if an array is used (so 2048 = 2047 + Null termination)
#define READ_BUF_SIZE 2048
#define ERROR -1
// Size of the string for the error response 
#define MAX_ERROR_SIZE 114

/**********************************************
 * init
   - port is the number of the port you want the server to be
     started on
   - initializes the connection acception/handling system
   - YOU MUST CALL THIS EXACTLY ONCE (not once per thread,
     but exactly one time, in the main thread of your program)
     BEFORE USING ANY OF THE FUNCTIONS BELOW
   - if init encounters any errors, it will call exit().
************************************************/

// Socket descriptor that will be returned after the server socket creation. monitoring will be done in socket server
int FD; 
// Client connection will be done using this file descriptor.
int CLIENT_FD; 
// Server address, internet domain --> used for init() and accept_connection()
struct sockaddr_in addr; 

void init(int port)
{
  // Socket creation. TCP used as transport protocol
  // Error handling for socket()
  if ((FD = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
  {
    perror("socket creation failed !");
    // Close the socket fd because the program is exiting
    if (close(FD) == -1)
    {
      // Error handling for close()
      perror("Failed to close FD.\n");
    }
    exit(1);
  }
  // Ensure that address for binding is reusable
  int enable = 1;
  // Error handling for setsockopt()
  if (setsockopt(FD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
  {
    perror("Socket option cannot be set.\n");
    // Close the socket fd, because the program is exiting
    if (close(FD) == -1)
    {
      // Error handling for close()
      perror("Failed to close FD.\n");
    }
    exit(1);
  }
  // Binding process
  addr.sin_family = AF_INET;
  // Server picks the port
  addr.sin_port = htons(port);
  // Network byte order
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // Error handling for bind()
  if (bind(FD, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    perror("Binding process failed.\n");
    // Close the socket fd, because the program is exiting
    if (close(FD) == -1)
    {
      // Error handling for close()
      perror("Failed to close FD.\n");
    }
    exit(1);
  }

  // Set up the server socket to monitor incoming request
  // Error handling for listen()
  if (listen(FD, BACKLOG) == -1)
  {
    perror("Monitoring failed!\n");
    // Close the socket fd, because the program is exiting
    if (close(FD) == -1)
    {
      // Error handling for close()
      perror("Failed to close FD.\n");
    }
    exit(1);
  }    
}
/**********************************************
 * accept_connection - takes no parameters
   - returns a file descriptor for further request processing.
     DO NOT use the file descriptor on your own -- use
     get_request() instead.
   - if the return value is negative, the request should be ignored.
***********************************************/
int accept_connection(void)
{
  // Client address, internet domain
  struct sockaddr_in client_addr;
  int addr_size = sizeof(client_addr);
  // Accept connection from client
  // Error handling for accept()
  if ((CLIENT_FD = accept(FD, (struct sockaddr *) &client_addr, (socklen_t*)&addr_size)) == -1) 
  {
    perror("Failed to accept connection.\n");
  }
  return CLIENT_FD;
}

/**********************************************
 * get_request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        from where you wish to get a request
      - filename is the location of a character buffer in which
        this function should store the requested filename. (Buffer
        should be of size 1024 bytes.)
   - returns 0 on success, nonzero on failure. You must account
     for failures because some connections might send faulty
     requests. This is a recoverable error - you must not exit
     inside the thread that called get_request. After an error, you
     must NOT use a return_request or return_error function for that
     specific 'connection'.
************************************************/
int get_request(int fd, char *filename)
{
	// buf[] holds the first line of HTTP GET request
	char buf[READ_BUF_SIZE];
	// Read the first line of HTTP GET request
	// Error handling for read()
	if ((read(fd, buf, READ_BUF_SIZE)) == -1)
	{
		perror("Could not read from the file specified.\n");
	}
	
	// str1 = "GET", str2 = filename, str3 = HTTP/1.1, each string initialized to READ_BUF_SIZE
	char str1[READ_BUF_SIZE], str2[READ_BUF_SIZE], str3[READ_BUF_SIZE];
	
  // The next 3 if-statements regard request error handling
  // Error handling for sscanf()
  if ((sscanf(buf, "%s %s %s", str1, str2, str3)) < 2)
  {
    perror("Incorrect number of strings in request.\n");
    return ERROR;
  }
  // strcmp() is thread-safe 
  // Error handling for strcmp()
  if (strcmp(str1,"GET") != 0)
  {
    perror("Incorrect request type.\n");
    return ERROR;
  }
  // strlen() is thread-safe 
  // Error handling for strlen() and strstr()
  if ((strlen(str2) > 1023) || (strstr(str2, "..") != NULL) || (strstr(str2, "//") != NULL))
  {
    perror("Incorrect file path.\n");
    return ERROR;
  }
	// Error handling for sprintf()
  if ((sprintf(filename,"%s",str2)) == -1)
  {
  	perror("Could not send str2 formatted output to filename.\n");
  }
	// Success case
	return 0; 
}
/**********************************************
 * return_result
   - returns the contents of a file to the requesting client
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the result of a request
      - content_type is a pointer to a string that indicates the
        type of content being returned. possible types include
        "text/html", "text/plain", "image/gif", "image/jpeg" cor-
        responding to .html, .txt, .gif, .jpg files.
      - buf is a pointer to a memory location where the requested
        file has been read into memory (the heap). return_result
        will use this memory location to return the result to the
        user. (remember to use -D_REENTRANT for CFLAGS.) you may
        safely deallocate the memory after the call to
        return_result (if it will not be cached).
      - numbytes is the number of bytes the file takes up in buf
   - returns 0 on success, nonzero on failure.
************************************************/
int return_result(int fd, char *content_type, char *buf, int numbytes) {
	// 81 is the sizes of the constant strings that will be returned in the response
  char result[81+sizeof(content_type)+numbytes];
  // Set every byte of result[] to the null termination
  memset(result,'\0',sizeof(result));

  // Send formatted output to char array that is returned
  if (sprintf(result,"HTTP/1.1 200 OK\n"
  "Content-Type: %s\n"
  "Content-Length: %d\n"
  "Connection: Close\n\n"
  "%s",content_type,numbytes,buf) < 0)
  {
    // Error handling for sprintf()
    perror("sprintf failed.\n");
    return ERROR;
  }
  // Writing the result to the client file descriptor
  if (write(fd, result, sizeof(result)) == -1)
  {
    // Error handling for write()
    perror("write failed.\n");
    return ERROR;
  }

  // Close the client fd
  if (close(fd) == -1)
  {
    // Error handling for close()
    perror("Failed to close fd.\n");
    return ERROR;
  }
  // Success case
  return 0;
}
/**********************************************
 * return_error
   - returns an error message in response to a bad request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the error
      - buf is a pointer to the location of the error text
   - returns 0 on success, nonzero on failure.
************************************************/
int return_error(int fd, char *buf) {
  char error[MAX_ERROR_SIZE];
  // Set every byte of error[] to the null termination
  memset(error,'\0',sizeof(error));
  // Send formatted output to char array that is returned
  if (sprintf(error,"HTTP/1.1 404 Not Found\n"
  "Content-Type: text/html\n"
  "Content-Length: 25\n"
  "Connection: Close\n\n"
  "%s",buf) < 0)
  {
    // Error handling for sprintf()
    perror("sprintf failed.\n");
    return ERROR;
  }
  // Writing the error to the client file descriptor
  if (write(fd, error, sizeof(error)) == -1)
  {
    // Error handling for write()
    perror("write failed.\n");
    return ERROR;
  }

  // Close the client fd
  if (close(fd) == -1)
  {
    // Error handling for close()
    perror("Failed to close fd.\n");
    return ERROR;
  }
	// Success case
	return 0;
}
