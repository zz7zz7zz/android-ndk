package com.module.net.socket;

public final class CSocket {

    static {
        System.loadLibrary("nio_net_socket");
    }

    public native void open(String host,int port, CSocketHandler handler);

    public native void reconnect();

    public native void write(byte[] bytes);

    public native byte[] read();

    public native void close();

}
