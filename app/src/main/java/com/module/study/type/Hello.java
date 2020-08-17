package com.module.study.type;

import android.util.Log;

public class Hello {

    static final String TAG = "Hello";

    public String name;
    public int    age;

    public Hello() {
    }

    public Hello(String name, int age) {
        this.name = name;
        this.age = age;
    }

    public void sayHello(){
        Log.v(TAG," hello , good morning !");
    }
}
