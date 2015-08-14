
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


// Should be enough..
#define MAX_BUF_SIZE 4096

//Defualt port number

//unlikely to have more than 16 active
//threads talking to the TB?
#define DEFAULT_MAX_CONNECTIONS 65535




int DEFAULT_SERVER_PORT;

//Payload Handling
int extract_payload(char* receive_buffer,char* payload, int max_n);
//void print_payload(FILE* log_file,char* send_buffer, int wlength, int nwords);
//void pack_value(char* payload,int wlength,int offset, char* port_value);
//void unpack_value(char* payload,int wlength,int offset, char* port_value);


//Create Server to listen for message
int create_server(int port_number, int max_connections); 


int connect_to_client(int server_fd);

int can_read_from_socket(int socket_id);

int can_write_to_socket(int socket_id); 

int receive_string(int n, char* buffer);

void  send_packet_and_wait_for_response(char* buffer, int send_length, char* server_host_name, int server_port_number);

void set_non_blocking(int sock_id);
void Data_Send(int sockid);

//Vhpi Function
void   Vhpi_Initialize(int sock_port);                                                     
void   Vhpi_Close(); 
void   Vhpi_Listen();
void   Vhpi_Send();
void   Vhpi_Set_Port_Value(char* reg_name, char* reg_value, int port_width);
void   Vhpi_Get_Port_Value(char* reg_name, char* reg_value, int port_width);
void   Vhpi_Log(char* message_string);

 
