package com.lib.utils;

import android.app.Application;
import android.util.Log;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class AppUtil {

    //获取Application
    public static Application getApplication() {

        Object currentActivityThread = null;
        Class activityThreadClass = null;

        //从属性里获取
        try {
            activityThreadClass = Class.forName("android.app.ActivityThread");
            Field sCurrentActivityThreadField = activityThreadClass.getDeclaredField("sCurrentActivityThread");
            sCurrentActivityThreadField.setAccessible(true);
            currentActivityThread = sCurrentActivityThreadField.get(null);
        } catch (Exception e) {
            Log.e("AppUtil", "getActivityThreadInActivityThreadStaticField: " + e.getMessage());
        }

        if(null == currentActivityThread){
            //从方法里获取
            try {
                Method getMainThread = activityThreadClass.getMethod("currentActivityThread");
                getMainThread.setAccessible(true);
                currentActivityThread = getMainThread.invoke(null);
            } catch (Exception e) {
                Log.e("AppUtil", "getActivityThreadInActivityThreadStaticMethod: " + e.getMessage());
                return null;
            }
        }

        try {
            Object app = activityThreadClass.getMethod("getApplication").invoke(currentActivityThread);
            if (null != app) {
                return (Application) app;
            }
        } catch (Exception e) {
            Log.e("AppUtil", "getApplicationInActivityThreadStaticMethod: " + e.getMessage());
            e.printStackTrace();
        }
        return null;
    }

    //获取摘要
    public static byte[] getDigest(byte[] hexBytes){
        try {
            MessageDigest digest = MessageDigest.getInstance("MD5");//"MD5"/"SHA-1"/"SHA-256"
            if (digest != null) {
                digest.update(hexBytes);
                return digest.digest();
            }
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return null;
    }
}
