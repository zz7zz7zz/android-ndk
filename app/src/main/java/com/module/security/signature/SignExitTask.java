package com.module.security.signature;

import android.os.Process;

public class SignExitTask implements Runnable{
    @Override
    public void run() {
        Process.killProcess(Process.myPid());
        System.exit(-1);
    }
}
