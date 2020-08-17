#include "bio_tcp_net_socket_client.h"



#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "../base/util_log.h"
#include "socket_config.h"

int tcp_net_socket_open(const char * ip, int port)
{
    struct sockaddr_in server_addr;
    int iret_socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(iret_socket_fd == -1){
        LOGV("net_socket","socket error %d",iret_socket_fd);
        return -1;
    }

    memset(&server_addr,0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if(connect(iret_socket_fd,(struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        LOGV("net_socket","connect error ");
        close(iret_socket_fd);
        return -2;
    }

    return iret_socket_fd;
}

//int tcp_net_socket_send(int sfd,char * msg)
//{
//    char send_buf[MAX_MSG_LENGTH];
//    memset(send_buf,0, sizeof(send_buf));
//
//    int length = strlen(msg);
//    memcpy(send_buf,msg,length);
//    send_buf[length] = '\0';
//    if(write(sfd,send_buf, sizeof(send_buf)) == -1){
//        close(sfd);
//        LOGV("Testing","connect error %s",msg);
//        return  -1;
//    }
//
////    send(sfd,msg,length,0);
//    return 0;
//}

int tcp_net_socket_send(int sfd,BYTE * msg,int length)
{
    char send_buf[MAX_MSG_LENGTH];
    memset(send_buf,0, sizeof(send_buf));

    memcpy(send_buf,msg,length);
    send_buf[length] = '\0';
    if(write(sfd,send_buf, sizeof(send_buf)) == -1){
        close(sfd);
        LOGV("net_socket","connect error %s",msg);
        return  -1;
    }

//    send(sfd,msg,length,0);
    return 0;
}

int tcp_net_socket_read(int sfd,BYTE read_buf[])
{
    memset(read_buf,0, MAX_MSG_LENGTH);
    read(sfd,read_buf, MAX_MSG_LENGTH);
    LOGV("net_socket","%s",read_buf);
    return 0;

}

int tcp_net_socket_close(int sfd)
{
    close(sfd);
    return 0;
}
