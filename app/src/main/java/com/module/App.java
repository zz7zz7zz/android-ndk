package com.module;

import android.app.Application;

public class App extends Application {

    private static App INS;

    @Override
    public void onCreate() {
        super.onCreate();
        INS = this;
    }

    public static App getInstance() {
        return INS;
    }
}
