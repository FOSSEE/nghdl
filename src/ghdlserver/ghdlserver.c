/*************************************************************************
 * <ghdlserver.c>  FOSSEE, IIT-Mumbai
 * 10.Feb.2017 - Raj Mohan - Log messages with timestamp/code clean up
 *************************************************************************
 */
#include <string.h>
#include "ghdlserver.h"
#include "uthash.h"
#include <fcntl.h>
#include <stdio.h>                                                              
#include <stdlib.h>                                                             
#include <assert.h>                                                             
#include <signal.h>                                                             
#include <sys/types.h>                                                          
#include <sys/socket.h>                                                         
#include <netinet/in.h>                                                         
#include <netdb.h>
#include <limits.h>
#include <time.h>
    
#define _XOPEN_SOURCE 500
#define MAX_NUMBER_PORT 100

FILE *log_server = NULL;

char* Out_Port_Array[MAX_NUMBER_PORT];                                          
int out_port_num = 0; 
int vhpi_cycle_count =0;
int server_socket_id = -1;

struct my_struct {
    char val[1024];                  
    char key[1024];       
    UT_hash_handle hh;    //Makes this structure hashable.
};

struct my_struct *s, *users, *tmp = NULL;

static char* curtim(void)
{
    static char ct[50];
    struct timeval tv;
    struct tm* ptm;
    long milliseconds;
    char time_string[40];

    gettimeofday (&tv, NULL);
    ptm = localtime (&tv.tv_sec);
    strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
    milliseconds = tv.tv_usec / 1000;
    sprintf (ct, "%s.%03ld", time_string, milliseconds);
    return(ct);
}

void parse_buffer(int sock_id, char* receive_buffer)
{
    static int rcvnum;
    log_server=fopen("server.log","a");                                                     
    fprintf(log_server,"\n%s RCVD RCVN:%d from CLT:%d buffer : %s\n",
	    curtim(), rcvnum++, sock_id,receive_buffer);
    
    /*Parsing buffer to store in hash table */ 
    char *rest;
    char *token;
    char *ptr1=receive_buffer;
    char *var;
    char *value;

    // Processing tokens.
    while(token = strtok_r(ptr1, ",", &rest)) 
    {
        ptr1 = rest;
        while(var=strtok_r(token, ":", &value))
        {
          s = (struct my_struct*)malloc(sizeof(struct my_struct));
          strncpy(s->key, var, 10);
          strncpy(s->val, value, 10);
          HASH_ADD_STR(users, key, s );
          break;    
        }
    }
        
    s = (struct my_struct*)malloc(sizeof(struct my_struct));
    strncpy(s->key, "sock_id", 10);
    snprintf(s->val,10, "%d", sock_id);
    HASH_ADD_STR(users, key, s);
    fflush(log_server);
    fclose(log_server);
}

void Vhpi_Set_Port_Value(char *port_name,char *port_value,int port_width)
{
  printf("Server-The port name is %s \n",port_name);
    
  s = (struct my_struct*)malloc(sizeof(struct my_struct));
  strncpy(s->key, port_name,10);
  strncpy(s->val,port_value,10);
  HASH_ADD_STR( users, key, s );
   
  printf("Server-The out port value is %s \n ",port_value);
  
  log_server=fopen("server.log","a");

  fflush(log_server);
  fclose(log_server);
}

void Vhpi_Get_Port_Value(char* port_name,char* port_value,int port_width)
{

  log_server=fopen("server.log","a");
  
  HASH_FIND_STR(users,port_name,s);
  if(s)
  {  

    snprintf(port_value,sizeof(port_value),"%s",s->val);
//    fprintf(log_server,"Port Name - %s And Port Value - %s \n",port_name,port_value);
    HASH_DEL(users, s);
    free(s);
  }
  else
  {
//    fprintf(log_server,"Port : %s not found \n",port_name);
  }
 
  fflush(log_server);
  fclose(log_server);
}

//
//Create Server to listen for message
//

int create_server(int port_number,int max_connections)
{
 int sockfd;
 struct sockaddr_in serv_addr;

 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 fprintf(stderr, "%s Server- Info: opening socket for server on port %d with socket:%d \n",curtim(), port_number,sockfd);
 if (sockfd < 0)
    fprintf(stderr, "Server- Error: in opening socket on port %d\n", port_number);

 bzero((char *) &serv_addr, sizeof(serv_addr));
 serv_addr.sin_family = AF_INET;
 serv_addr.sin_addr.s_addr = INADDR_ANY;
 serv_addr.sin_port = htons(port_number);

 if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
 {
    fprintf(stderr,"Server- Error: could not bind socket to port %d\n",port_number);
    close(sockfd);
    sockfd= -1;
 }
 else
 fprintf(stderr,"Server- Info: finished binding socket to port %d\n",port_number);
 // Start listening on the server.
 listen(sockfd,max_connections);

 return sockfd;
}

// The server to wait (non-blocking) for a client connection.
int connect_to_client(int server_fd)                                            
{                                                                               
    int ret_val = 1;
    int newsockfd = -1;
    socklen_t clilen;
    struct sockaddr_in  cli_addr;
    fd_set c_set;
    struct timeval time_limit;                                                
  
    time_limit.tv_sec = 0;
    time_limit.tv_usec = 1000;
    
    clilen = sizeof(cli_addr); 

    FD_ZERO(&c_set);
    FD_SET(server_fd, &c_set);


    select(server_fd + 1, &c_set, NULL, NULL, &time_limit);
    if(FD_ISSET(server_fd, &c_set))
    {                                                                           
        newsockfd = accept(server_fd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd >= 0)
        { 
        fprintf(stderr,"%s Server-SRV:%d New Client Connection CLT:%d\n",
                curtim(), server_fd, newsockfd);  
        }        
        else
        { 
            fprintf(stderr,"Server- Info: failed in accept()\n");    
        }                   
    } 
    return(newsockfd);
}   

//                                                                              
// Check if we can read from the socket..
//    
int can_read_from_socket(int socket_id)                                         
{                                                                               
    struct timeval time_limit;  
    time_limit.tv_sec = 0;  
    time_limit.tv_usec = 1000;
    
    fd_set c_set; 
    FD_ZERO(&c_set); 
    FD_SET(socket_id, &c_set);
    
    int npending = select(socket_id + 1, &c_set, NULL, NULL, &time_limit);

    return(FD_ISSET(socket_id, &c_set));
}   

//                                                                              
// Check if we can write to the socket..
//    
int can_write_to_socket(int socket_id)                                          
{                                                                               
    struct timeval time_limit;  
    time_limit.tv_sec = 0;
    time_limit.tv_usec = 1000;
    
    fd_set c_set;
    FD_ZERO(&c_set);
    FD_SET(socket_id, &c_set);                                                  

    int npending = select(socket_id + 1, NULL, &c_set, NULL, &time_limit);
    
    return(FD_ISSET(socket_id,&c_set));
}   

//Receive string from socket and put it inside buffer.
int receive_string(int sock_id, char* buffer)                                   
{                                                                               
  int nbytes = 0;  
    
    while(1)
    {                                                                           
        if(can_read_from_socket(sock_id))
            break;
        else   
            usleep(1000);
    }                                                                           
    
    nbytes = recv(sock_id, buffer, MAX_BUF_SIZE, 0);
    return(nbytes); 
}   
//                                                                              
// Establish connection, send the packet and block till a response is obtained. 
// Socket will be closed after the response is obtained..
// The buffer is used for the sent as well as the received data.        
//
void set_non_blocking(int sock_id)                                              
{                                                                               
    int x;                                                                    
    x=fcntl(sock_id,F_GETFL,0); 
    fcntl(sock_id,F_SETFL,x | O_NONBLOCK); 
    fprintf(stderr,"Server- Setting server to non blocking state"); 
} 

void Vhpi_Initialize(int sock_port)
{
    DEFAULT_SERVER_PORT = sock_port;
 
   /*Taking time info for log*/ 
    time_t systime;                                                             
    systime = time(NULL);  
    
    log_server=fopen("server.log","a");

    signal(SIGINT,Vhpi_Close);
    signal(SIGTERM,Vhpi_Close);

    int try_limit = 100;

    while(try_limit > 0)
    {
      server_socket_id = create_server(DEFAULT_SERVER_PORT,DEFAULT_MAX_CONNECTIONS);
      if(server_socket_id > 0)
        {
            fprintf(log_server,"%s -Started the server at port %d  SRV:%d\n",
		    curtim(), DEFAULT_SERVER_PORT, server_socket_id);
            set_non_blocking(server_socket_id);
            break;
            
        }
        else
            fprintf(stderr,"Server- Info:Could not start server on port %d,will try again\n",DEFAULT_SERVER_PORT);

        usleep(1000);
        try_limit--;
        
        if(try_limit==0)
        {
            fprintf(stderr,"Server- Error:Tried to start server on port %d, failed..giving up \n",DEFAULT_SERVER_PORT);
            exit(1);
            }

    }

    fflush(log_server);
    fclose(log_server);

  //                                                                            
  //Reading Output Port name and storing in Out_Port_Array;                     
  //                                                                            
  char *line = NULL;                                                            
  size_t len = 0;                                                               
  ssize_t read;                                                                 
  char *token;                                                                  
  FILE *fp;                                                                     

  fp=fopen("connection_info.txt","r");                                          
  
  while ((read = getline(&line, &len, fp)) != -1)                               
  {                                                                             
    if (strstr(line,"OUT") != NULL || strstr(line,"out") != NULL )                  {                                                                           
      strtok_r(line, " ",&token);                                               
      Out_Port_Array[out_port_num] = line;                                      
      out_port_num++;                                                           
    }                                                                           
    line = (char *)malloc(sizeof(char));                                        
  }                     	

  fprintf(stderr,"\n Server- Vhpi_Initialize finished \n"); 
  sleep(2);
  fclose(fp);
}

void Vhpi_Listen()
{
    char payload[4096];
    int payload_length;
    vhpi_cycle_count++;

    int new_sock;

    while(1)
    {
        if((new_sock = connect_to_client(server_socket_id)) > 0) 
        {
            char receive_buffer[MAX_BUF_SIZE];
	    int n = receive_string(new_sock, receive_buffer);
	    if(n > 0)
            {
                  log_server=fopen("server.log","a");
		fprintf(log_server,"%s New socket connection CLT:%d\n",
                        curtim(), new_sock);
                  fflush(log_server);
                  fclose(log_server);

	      if(strcmp(receive_buffer,"END")==0) 
                {
                  log_server=fopen("server.log","a");
                  fprintf(log_server,
			  "%s Received Server closing request - CLT:%d\n", 
                          curtim(), new_sock);  
                  fflush(log_server);
                  fclose(log_server);
                  Vhpi_Close();
                  exit(0);
                  sleep(1);
                }  
	      else 
                {
                  parse_buffer(new_sock,receive_buffer);
                }
                break;
            }
        
        } 
    else
      {
        break;
      }
    }
}

// Traverse the list of finished jobs and send out the resulting port values.. 
void  Vhpi_Send() 
{
    int sockid;
    char* out;
    log_server=fopen("server.log","a");
    fprintf(log_server, "%s Vhpi_Send() called\n", curtim());
    HASH_FIND_STR(users,"sock_id",s);
    if(s)
    {  
      sockid=atoi(s->val);
    }
    else
    {
      fprintf(log_server,"%s Socket id not in table - key=%s val=%s\n",
              curtim(),
	      users->key, users->val);  
    }

    Data_Send(sockid);                                      
    
    fflush(log_server);
    fclose(log_server);
 
}

void Data_Send(int sockid)                                       
{                                                                               
  static int trnum;
  char* out;
  out = (char *) malloc(sizeof(char));
  *out = '\0';
  char send_data_buf[BUFSIZ];
  int i;
  char colon = ':';
  char semicolon = ';'; 
  int wrt_retries = 0;

  for (i=0;i<out_port_num;i++)
  {  
     HASH_FIND_STR(users,Out_Port_Array[i],s);
     if(s) 
     { 
       strncat(out, s->key, strlen(s->key));
       strncat(out, &colon, 1);
       strncat(out, s->val, strlen(s->val));
       strncat(out, &semicolon, 1);

       while(1)
       {
         if (wrt_retries > 2) return;
         if(can_write_to_socket(sockid)) break;
	 fprintf(log_server, "%s CLT:%d SOCKET NOT WRITABLE - Re-trying\n", 
                 curtim(), sockid);
	 usleep(1000);
         wrt_retries++;
      } 
    }         
    else                                                                        
    {                                                                       
      fprintf(log_server,"%s The %s's value not found in the table \n",
                curtim(), Out_Port_Array[i]);
      return;
    }
  }
  // RM - AVOID UNNECCESSARY COPY.
  strcpy(send_data_buf, out);
  if ((send(sockid, send_data_buf, sizeof(send_data_buf), 0)) == -1)
    {
      perror("Server- Failure Sending Message\n");
      fprintf(log_server,"%s Failure sending to CLT:%d buffer:%s\n",
              curtim(), sockid, send_data_buf);
      exit(1);
    }
  fprintf(log_server,"%s SNT:TRNUM:%d to CLT:%d buffer: %s\n",
          curtim(),
          trnum++, sockid,send_data_buf);  
} 

void  Vhpi_Close()                                                         
{  
    fprintf(stderr,"%s Server- Info: closing VHPI link\n", curtim());

 close(server_socket_id);
}

static void Vhpi_Exit(int sig)                                                  
{                                                                               
  fprintf(stderr, "Server- *** Break! ***\n");          
  fprintf(stderr,"%s Info: Stopping the simulation \n", curtim()); 
  Vhpi_Close(); 
  exit(0);
}    
