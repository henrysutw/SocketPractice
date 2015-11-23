/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define NUM_THREADS 10

void *dostuff(void *); /* function prototype */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in6 serv_addr, cli_addr;
     char client_addr_ipv6[100];
     pthread_t threads[NUM_THREADS];
     int rc;
     int count = 0;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET6, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin6_flowinfo = 0;
     serv_addr.sin6_family = AF_INET6;
     serv_addr.sin6_addr = in6addr_any;
     serv_addr.sin6_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");

         if(count==NUM_THREADS)
             count = 0;

         rc = pthread_create(&threads[count], NULL, dostuff, (void *)newsockfd);
         if(rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
         }
/*
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             //close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);*/
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void *dostuff (void *sock)
{
   int n;
   char buffer[256];
   char endkey[] = "-1";
   char username[25];
      
   bzero(username,25);
   n = read((int)sock,username,24);
   if (n < 0) error("ERROR reading from socket");
   printf("%s has connected to this server\n", username);
   n = write((int)sock,"Connection accepted",19);
   if (n < 0) error("ERROR writing to socket");

   do {
      bzero(buffer,256);
      n = read((int)sock,buffer,255);
      if (n < 0) error("ERROR reading from socket");
//      printf("Here is the message: %s\n",buffer);
      n = write((int)sock,buffer,strlen(buffer));
      if (n < 0) error("ERROR writing to socket");
   }while(strncmp(buffer, endkey, 2)!=0);

   printf("%s closes the connection\n", username);
   close((int)sock);
   pthread_exit(NULL);
}
