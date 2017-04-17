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
//#define LOG_FILE "vhpi.log"                                                
//FILE *log_file = NULL;    
//FILE *log_sock_id = NULL;  
FILE *log_server = NULL;
FILE *log_server1 = NULL;
#define z32__ "00000000000000000000000000000000"     

char* Out_Port_Array[MAX_NUMBER_PORT];                                          
int out_port_num=0; 

int vhpi_cycle_count =0;
//server socket 
int server_socket_id=-1;
struct my_struct {
    char val[1024];                  
    char key[1024];       //Key
    UT_hash_handle hh;         /* makes this structure hashable */
};


struct my_struct *s, *users ,*tmp = NULL;

void parse_buffer(int sock_id,char* receive_buffer)
{
    /*Taking time information for log*/
    
    //time_t systime;                                                             
    //systime = time(NULL);  
    log_server=fopen("server.log","a");                                                     
    //fprintf(log_server,"Buffer came at %s \n",ctime(&systime));   
    
    fprintf(log_server,"Server-The recieved buffer is : %s \n",receive_buffer);
    fprintf(log_server,"Server-The socket id is : %d \n",sock_id);
    
    /*Parsing buffer to store in hash table */ 
    char *rest;
    char *token;
    char *ptr1=receive_buffer;
    
    char *var;
    char *value;

      
    fprintf(stderr,"Server-The recieved buffer is : %s \n",receive_buffer);
    fprintf(stderr,"Server-The socket id is : %d \n",sock_id);
    while(token = strtok_r(ptr1,",",&rest)) 
    {
        ptr1 = rest; // rest contains the left over part..assign it to ptr...and start tokenizing again.
        
        //Processing token again;

        while(var=strtok_r(token,":",&value))
        {

          s = (struct my_struct*)malloc(sizeof(struct my_struct));
          printf("Server-Variable is %s \n",var);
          printf("Server-Value is %s \n",value);
            
          strncpy(s->key, var,10);
          strncpy(s->val,value,10);
          HASH_ADD_STR( users, key, s );
          break;    
        }
    }
        
    s = (struct my_struct*)malloc(sizeof(struct my_struct));
    strncpy(s->key,"sock_id",10);
    snprintf(s->val,10,"%d",sock_id);
    HASH_ADD_STR(users,key,s);
    fflush(log_server);
    fclose(log_server);
}


void Vhpi_Set_Port_Value(char *port_name,char *port_value,int port_width)
{
  //char *lb; // you need to know maximum size of lb.
  //int I;
  //snprintf(lb,sizeof(char),"%s", port_value);
  
  printf("Server-Vhpi_Set_Port_value \n");
  printf("Server-The port name is %s \n",port_name);
   
    
  //printf("Port valu is %s",port_value);
    
  s = (struct my_struct*)malloc(sizeof(struct my_struct));
  strncpy(s->key, port_name,10);
  strncpy(s->val,port_value,10);
  HASH_ADD_STR( users, key, s );
    
   
  printf("Server-The out port value is %s \n ",port_value);
  
  log_server=fopen("server.log","a");
  fprintf(log_server,"Set Port Details \n");
  fprintf(log_server,"Port Name - %s And Port Value - %s \n",port_name,port_value);
  fflush(log_server);
  fclose(log_server);


}

void Vhpi_Get_Port_Value(char* port_name,char* port_value,int port_width)

{
  
  printf("Server-Vhpi_Get_Port_Value \n");
  //int I;
  //snprintf(port_value,1024,"1");
 
  log_server=fopen("server.log","a");
  fprintf(log_server,"Get Port Details \n");
  
  HASH_FIND_STR(users,port_name,s);
  if(s)
  {  
    printf("Server-The key is %s and value is  %s \n",s->key,s->val);

    snprintf(port_value,sizeof(port_value),"%s",s->val);
    fprintf(log_server,"Port Name - %s And Port Value - %s \n",port_name,port_value);
    HASH_DEL(users, s);
    free(s);
  }
  else
  {
    printf("Server-Port %s Not found \n",port_name);
    fprintf(log_server,"Port : %s not found \n",port_name);
  }
 
  fflush(log_server);
  fclose(log_server);
}
/*
int Copy_Value(char* dest, char* src, int width)                                
{                                                                               
  int ret_val = 0;                                                              
  char src_buf[4096];                                                           
  int src_width = 0;      
  
  // skip spaces                                                                
  while(*src == ' ')                                                            
    src++;                                                                                                                                                                                 
  while(1)                                                                      
  {                                                                           
    if(src[src_width] == '1' || src[src_width] == '0')                        
    {                                                                             
      src_buf[src_width] = src[src_width];                                        
      src_width++;                                                                
    }                                                                             
    else                                                                      
    break;                                                                        
  }                                                                           
  src_buf[src_width] = 0; // null-terminate                                                                                                                                                                              
  ret_val = src_width - width;                                                  
  dest[width] = 0;                                                              
  int i;                                                                        
  for(i = 1; i <= width; i++)                                                   
  {                                                                           
    if(i <= src_width)                                                        
      dest[width-i] = src[src_width-i];                                             
    else                                                                      
      dest[width-i] = '0'; // pad with 0.                                           
  }                                                                           
  
    return(ret_val);                                                                                                                                                  
}

*/
/*
int extract_payload(char* receive_buffer,char* payload, int max_n)
{ 
    int hash_pos = 0;
    int ret_val = 0;
    while(receive_buffer[hash_pos] != '#')
    {
        printf("Buffer is %c \n",receive_buffer[hash_pos]);
        if(receive_buffer[hash_pos] == 0) // end of string
        {
            hash_pos = -1;
            break;
        }
            hash_pos++;
    }
                                                     
    if(hash_pos >= 0)
    {
        receive_buffer[hash_pos] = 0;
        ret_val = max_n - (hash_pos+1);
        bcopy(receive_buffer+(hash_pos+1),payload,ret_val);
    }
    
    return(ret_val);
} 
*/


//
//Create Server to listen for message
//

int create_server(int port_number,int max_connections)
{
    
    
 int sockfd;
 struct sockaddr_in serv_addr;
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 fprintf(stderr, "Server- Info: opening socket for server on port %d with socket id %d \n",port_number,sockfd);
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
 // start listening on the server.
 listen(sockfd,max_connections);
 return sockfd;
 
}


//
// ask the server to wait for a client connection
// and accept one connection if possible
// 
// uses select to make this non-blocking.
// 
//

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
    FD_SET(server_fd,&c_set);                                                     
    select(server_fd+1, &c_set,NULL,NULL,&time_limit);                            
    if(FD_ISSET(server_fd,&c_set))                                                
    {                                                                           
        newsockfd = accept(server_fd,(struct sockaddr *) &cli_addr,&clilen);      
        if (newsockfd >= 0)                                                       
        {                                                                           
            fprintf(stderr,"Server- Info: new client connection %d \n",newsockfd);            
        }                                                                           
        else                                                                      
        {                                                                           
            fprintf(stderr,"Server- Info: failed in accept()\n");                             
        }                                                                           
    } 
    //else
    //{
    //  fprintf(stderr,"Server- failed to connect to client \n");  
    //}                                                                          
    
    return(newsockfd);                                                            
}   


//
//use select to check if we can write to                                       
// the socket..
//

int can_read_from_socket(int socket_id)                                         
{                                                                               
    struct timeval time_limit;                                                    
    time_limit.tv_sec = 0;                                                        
    time_limit.tv_usec = 1000;                                                    
    
    fd_set c_set;                                                                 
    FD_ZERO(&c_set);                                                              
    FD_SET(socket_id,&c_set);                                                     
    
    int npending = select(socket_id + 1, &c_set, NULL,NULL,&time_limit);          
    
    return(FD_ISSET(socket_id,&c_set));                                           
}   


//                                                                              
// use select to check if we can write to                                       
// the socket..                                                                 
//    

int can_write_to_socket(int socket_id)                                          
{                                                                               
    struct timeval time_limit;                                                    
    time_limit.tv_sec = 0;                                                        
    time_limit.tv_usec = 1000;                                                    
    
    fd_set c_set;                                                                 
    FD_ZERO(&c_set);                                                              
    FD_SET(socket_id,&c_set);                                                  
    int npending = select(socket_id + 1, NULL, &c_set,NULL,&time_limit);          
    
    return(FD_ISSET(socket_id,&c_set));                                           
}   


//receive string from socket and put it inside buffer.

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
    
    nbytes = recv(sock_id,buffer,MAX_BUF_SIZE,0);                                 
    return(nbytes);                                                               
}   

//                                                                              
// will establish a connection, send the                                        
// packet and block till a response is obtained.                                
// socket will be closed after the response                                     
// is obtained..                                                                
// the buffer is used for the sent as well                                      
// as the received data.        
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
    //log_file=fopen("vhpi.log","a");

    signal(SIGINT,Vhpi_Close);
    signal(SIGTERM,Vhpi_Close);

    int try_limit = 100;

    while(try_limit > 0)
    {
        server_socket_id = create_server(DEFAULT_SERVER_PORT,DEFAULT_MAX_CONNECTIONS);

        if(server_socket_id > 0)
        {
            fprintf(stderr,"Server- Info:Success: Started the server on port %d\n",DEFAULT_SERVER_PORT);
            fprintf(log_server,"Server -Started the server at port %d \n",DEFAULT_SERVER_PORT);
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
    
    //fprintf(log_server,"Setup completed on server side at %s ",ctime(&systime));
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
    if (strstr(line,"OUT") != NULL || strstr(line,"out") != NULL )                                             
    {                                                                           
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
    //#ifdef DEBUG
    //fprintf(log_file,"Server- Info: listening in cycle %d\n", vhpi_cycle_count);
    //fflush(log_file);
    //#endif
    int new_sock;
    while(1)
    {
        if((new_sock = connect_to_client(server_socket_id)) > 0) 
        {
            char receive_buffer[MAX_BUF_SIZE];
            fprintf(stderr,"Server- Info : waiting for client message \n");

        //if the client has connected "just now"
        // it must send something!
        //#ifdef DEBUG
        //fprintf(log_file,"Server- Info: waiting for message from client %d\n", new_sock);
        //fflush(log_file);
        //#endif
        int n = receive_string(new_sock,receive_buffer);
  
        
        if(n > 0)
            {
                  
                //payload_length = extract_payload(receive_buffer,payload,n);
                //#ifdef DEBUG
                //fprintf(log_file,"Info: received message from client %d: %s (payload-length=%d)\n",new_sock, receive_buffer,payload_length);
                //fprintf(log_file,"Server- Info: received message from client %d : %s \n",new_sock,receive_buffer);
                //fflush(log_file);
                //#endif

                
                if(strcmp(receive_buffer,"END")==0) 
                {
                  
                  log_server=fopen("server.log","a");
                  fprintf(log_server,"Accept Server closing request \n");  
                  printf("Accept server closing request \n");
                  fflush(log_server);
                  fclose(log_server);
                  Vhpi_Close();
                  exit(0);
                  sleep(1);
                  //close(server_socket_id);
                }  
                
                else 
                {
                  parse_buffer(new_sock,receive_buffer);
                  
                }
                
                //parse_buffer(new_sock,receive_buffer);
                break;
            }
        
        } 
    else
      {
        break;
      }
    }
    //#ifdef DEBUG
    //fprintf(log_file,"Server- Info: finished listening in cycle %d\n", vhpi_cycle_count); 
    //fflush(log_file);
    //#endif
}

// go down the list of finished jobs and send                                   
// out the resulting port values.. 
void  Vhpi_Send() 
{
    int sockid;
    char* out;
    //#ifdef DEBUG                                                                    
    //fprintf(log_file,"Server- Info: sending in cycle %d\n", vhpi_cycle_count);            
    //fflush(log_file);                                                             
    //#endif
    
    log_server=fopen("server.log","a");

    fprintf(stderr,"Server- Sending data to client \n");
    fprintf(log_server,"Sending data from server to client \n");  
    HASH_FIND_STR(users,"sock_id",s);
    if(s)
    {  
      printf("Server- The key is %s and value is  %s \n",s->key,s->val);
      sockid=atoi(s->val);
      //strncpy(sockid,10,atoi(s->val));
      //HASH_DEL(users, s);
      //free(s);
    }
    else
    {
      printf("Server- The socket id not found in table \n");
      fprintf(log_server,"The socket id is not present in table \n");
    }
    
    //snprintf(sockid,sizeof(sockid),"%d",s->val);
  
      Data_Send(sockid);                                      

    fflush(log_server);
    fclose(log_server);
 
}

void Data_Send(int sockid)                                       
{                                                                               
  char* out;
  out = (char *) malloc(sizeof(char));
  *out = '\0';
  char send_data_buf[BUFSIZ];
  int i;
  char colon = ':';
  char semicolon = ';'; 
  for (i=0;i<out_port_num;i++)
  {  
  HASH_FIND_STR(users,Out_Port_Array[i],s);                                            
  if(s)                                                                       
  {                                                                           
    printf("Server-Sending data has key:%s and value:%s \n",s->key,s->val);        
    fprintf(log_server,"Sending data has key:%s and value:%s \n",s->key,s->val);
    
    strncat(out, s->key, strlen(s->key));
    strncat(out, &colon, 1);
    strncat(out, s->val, strlen(s->val));
    strncat(out, &semicolon, 1);

    
    //HASH_DEL(users, s);                                                     
    //free(s);                                                                
    while(1)                                                                  
    {                                                                         
    if(can_write_to_socket(sockid))                                         
      break;                                                                
    usleep(1000);                                                           
    }                                                                         
  
  }                                                                           
  else                                                                        
  {                                                                           
    printf("Server- The output port's %s value Not found \n",Out_Port_Array[i]);
    fprintf(log_server,"The %s's value not found in the table \n",Out_Port_Array[i]);                         
  }
  }

        strcpy(send_data_buf, out);

        if ((send(sockid, send_data_buf, sizeof(send_data_buf), 0)) == -1)
        {
                perror("Server- Failure Sending Message\n");
                exit(1);
        }
        fprintf(log_server,"Val of output buffer %s\n",send_data_buf);  
} 

void  Vhpi_Close()                                                         
{  
 fprintf(stderr,"Server- Info: closing VHPI link\n");
 //fclose(log_file); 
 close(server_socket_id);
    
}

static void Vhpi_Exit(int sig)                                                  
{                                                                               
    fprintf(stderr, "Server- *** Break! ***\n");                                          
    fprintf(stderr,"Server- Info: Stopping the simulation \n");                           
    Vhpi_Close();                                                                 
    exit(0);                                                                      
}    
