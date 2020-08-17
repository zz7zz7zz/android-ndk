//
// Created by 杨龙辉 on 2020/5/3.
//


#ifndef TCP_NET_SOCKET_CLIENT_H
#define TCP_NET_SOCKET_CLIENT_H

#include "../ffmpeg_cmd/include/compat/avisynth/avs/types.h"

int tcp_net_socket_open(const char * ip, int port );

//    int tcp_net_socket_send(int sfd,char * msg);
    int tcp_net_socket_send(int sfd,BYTE * msg,int length);

    int tcp_net_socket_read(int sfd,BYTE * read_buf);

    int tcp_net_socket_close(int sfd);


#endif //TCP_NET_SOCKET_CLIENT_H
