//
// Created by 杨龙辉 on 2020/5/3.
//


#ifndef NIO_TCP_NET_SOCKET_CLIENT_H
#define NIO_TCP_NET_SOCKET_CLIENT_H

#include "../ffmpeg_cmd/include/compat/avisynth/avs/types.h"

    int nio_tcp_net_socket_open(const char * ip, int port, int * openErrorCode);

//    int tcp_net_socket_send(int sfd,char * msg);
    int nio_tcp_net_socket_send(int sfd,BYTE * msg,int length);

    int nio_tcp_net_socket_read(int sfd,BYTE * read_buf);

    int nio_tcp_net_socket_close(int sfd);


#endif //NIO_TCP_NET_SOCKET_CLIENT_H
