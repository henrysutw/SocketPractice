#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in6 serv_addr;
    struct hostent *server;

    char buffer[256];
    char endkey[] = "-1";
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname2(argv[1], AF_INET6);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin6_flowinfo = 0;
    serv_addr.sin6_family = AF_INET6;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin6_addr,
         server->h_length);
    serv_addr.sin6_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    n = write(sockfd,argv[3],strlen(argv[3]));
    if (n < 0) 
        error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
        error("ERROR reading from socket");
    printf("%s\n", buffer);

    while(1){
       printf("Please enter the message: ");
       bzero(buffer,256);
       fgets(buffer,255,stdin);
       n = write(sockfd,buffer,strlen(buffer));
       if (n < 0) 
            error("ERROR writing to socket");
       if(strncmp(buffer, endkey, 2)==0)
          break;
       printf("Server resent:\n");
       bzero(buffer,256);
       n = read(sockfd,buffer,255);
       if (n < 0) 
            error("ERROR reading from socket");
       printf("%s\n",buffer);
    }
    close(sockfd);
    return 0;
}
