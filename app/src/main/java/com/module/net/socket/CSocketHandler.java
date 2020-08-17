package com.module.net.socket;

public interface CSocketHandler {


    public void onSocketConnectResult(boolean isSuccess);


    public void onSocketResponse(byte[] bytes);

}
