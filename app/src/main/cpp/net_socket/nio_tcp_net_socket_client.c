#include "bio_tcp_net_socket_client.h"



#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "../base/util_log.h"
#include "socket_config.h"

//https://www.cnblogs.com/GameDeveloper/p/3406565.html
static const char* kTAG = "NIO";
int nio_tcp_net_socket_open(const char * ip, int port, int * openErrorCode)
{
    int iret_socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(iret_socket_fd == -1){
        LOGV(kTAG,"socket error %d",iret_socket_fd);
        *openErrorCode = -1;
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr,0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    //https://www.cnblogs.com/GameDeveloper/p/3406565.html
    int flags = fcntl(iret_socket_fd,F_GETFL,0);//获取建立的sockfd的当前状态（非阻塞）
    if(flags < 0){
        close(iret_socket_fd);
        *openErrorCode = -2;
        return -1;
    }

    LOGV(kTAG,"socket flags %d",flags);
    int newFlags = fcntl(iret_socket_fd,F_SETFL,flags|O_NONBLOCK);//将当前sockfd设置为非阻塞
    if(newFlags < 0){
        close(iret_socket_fd);
        *openErrorCode = -3;
        return -1;
    }

    int connectRet = connect(iret_socket_fd,(struct sockaddr *)&server_addr, sizeof(server_addr));
    if( connectRet == -1){//无论连接是否建立立即返回-1，同时将errno（包含errno.h就可以直接使用）设置为EINPROGRESS, 表示此时tcp三次握手仍旧进行，如果errno不是EINPROGRESS，则说明连接错误，程序结束
        LOGV(kTAG,"connect() errno = %d " ,errno);
        if(errno == EINPROGRESS){

        }else{//说明连接错误，程序结束
            close(iret_socket_fd);
            *openErrorCode = -4;
            return -1;
        }
    } else if(connectRet == 0){//连接成功
        return iret_socket_fd;
    }

    fd_set fd_rds;
    fd_set fd_wrs;
    fd_set fd_exs;
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec= 0;

    FD_ZERO(&fd_rds);
    FD_SET(iret_socket_fd,&fd_rds);

    FD_ZERO(&fd_wrs);
    FD_SET(iret_socket_fd,&fd_wrs);

    FD_ZERO(&fd_exs);
    FD_SET(iret_socket_fd,&fd_exs);

    int ret = select(iret_socket_fd + 1,&fd_rds,&fd_wrs,&fd_exs,&timeout);
    if(ret < 0){
        //an error occurred.
        LOGV(kTAG,"select an error occurred errno = %d " ,errno);
        close(iret_socket_fd);
        *openErrorCode = -5;
        return -1;
    }else if(ret == 0){
        //the time limitexpires
        LOGV(kTAG,"select the time limitexpires");
        close(iret_socket_fd);
        *openErrorCode = -6;
        return -1;
    }else{

        LOGV(kTAG,"select Success errno = %d",errno);

        int vr =    FD_ISSET(iret_socket_fd,&fd_rds);
        int vw =    FD_ISSET(iret_socket_fd,&fd_wrs);
        int ve =    FD_ISSET(iret_socket_fd,&fd_exs);

        LOGV(kTAG,"select FD_ISSET fd_rds = %d ; FD_ISSET fd_wrs %d ; FD_ISSET fd_exs %d " ,vr ,vw ,ve);

        if(vr || vw){
            int error = 0;
            socklen_t socklen = sizeof(error);
           int ret_getsocketopt = getsockopt(iret_socket_fd,SOL_SOCKET,SO_ERROR,&error,&socklen);


            LOGV(kTAG,"select (getsocketopt = %d), (errno = %d errnostr = %s),(error = %d errorstr = %s) " ,ret_getsocketopt ,errno ,strerror(errno) ,error,strerror(error));

            if(ret_getsocketopt < 0){
               close(iret_socket_fd);
               *openErrorCode = -7;
               return -1;
           }

            if(error != 0){
                close(iret_socket_fd);
                *openErrorCode = -8;
                return -1;
            }
        }
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

int nio_tcp_net_socket_send(int sfd,BYTE * msg,int length)
{
    char send_buf[MAX_MSG_LENGTH];
    memset(send_buf,0, sizeof(send_buf));

    memcpy(send_buf,msg,length);
    send_buf[length] = '\0';

    ssize_t susLen= write(sfd,send_buf, length);

    LOGV(kTAG,"nio_tcp_net_socket_send A (error = %d errorstr = %s) susLen %d length %d message %s ",errno ,strerror(errno) ,susLen,length ,send_buf);

    if( susLen == -1){
        close(sfd);
        return  -1;
    }

    LOGV(kTAG,"nio_tcp_net_socket_send B (error = %d errorstr = %s) susLen %d length %d message %s ",errno ,strerror(errno) ,susLen,length ,send_buf);
//    send(sfd,msg,length,0);
    return 0;
}

int nio_tcp_net_socket_read(int iret_socket_fd,BYTE read_buf[])
{
//    memset(read_buf,0, MAX_MSG_LENGTH);
//    read(sfd,read_buf, MAX_MSG_LENGTH);
//    LOGV(kTAG,"nio_tcp_net_socket_read %s",read_buf);
//    return 0;

    LOGV(kTAG,"nio_tcp_net_socket_read ------------------------------");
    fd_set fd_rds;
    fd_set fd_wrs;
    fd_set fd_exs;
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec= 0;

//    while(1){
        FD_ZERO(&fd_rds);
        FD_SET(iret_socket_fd,&fd_rds);

        FD_ZERO(&fd_wrs);
        FD_SET(iret_socket_fd,&fd_wrs);

        FD_ZERO(&fd_exs);
        FD_SET(iret_socket_fd,&fd_exs);

        int ret = select(iret_socket_fd + 1,&fd_rds,&fd_wrs,&fd_exs,&timeout);
        LOGV(kTAG,"nio_tcp_net_socket_read select errno = %d selectCode = %d ",errno ,ret);
        if(ret >0){

            int vr =    FD_ISSET(iret_socket_fd,&fd_rds);
            int vw =    FD_ISSET(iret_socket_fd,&fd_wrs);
            int ve =    FD_ISSET(iret_socket_fd,&fd_exs);

            LOGV(kTAG,"nio_tcp_net_socket_read select FD_ISSET fd_rds = %d ; FD_ISSET fd_wrs %d ; FD_ISSET fd_exs %d " ,vr ,vw ,ve);

            int error = 0;
            socklen_t socklen = sizeof(error);
            int ret_getsocketopt = getsockopt(iret_socket_fd,SOL_SOCKET,SO_ERROR,&error,&socklen);
            LOGV(kTAG,"select (getsocketopt = %d), (errno = %d errnostr = %s),(error = %d errorstr = %s) " ,ret_getsocketopt ,errno ,strerror(errno) ,error,strerror(error));

            if(ret_getsocketopt < 0){

            }
            if(vr && vw){
//                close(iret_socket_fd);
//                return -1;
            }

            if(vr){
                    memset(read_buf,0, MAX_MSG_LENGTH);
                    socklen_t  socklen1 = read(iret_socket_fd,read_buf, MAX_MSG_LENGTH);
                    LOGV(kTAG,"nio_tcp_net_socket_read (errno = %d errnostr = %s),(len = %d value = %s )",errno ,strerror(errno),socklen1,read_buf);
                    return socklen1;
            }

            if(vw){

            }
        }
//    }
    return 0;
}

int nio_tcp_net_socket_close(int sfd)
{
    close(sfd);
    return 0;
}
