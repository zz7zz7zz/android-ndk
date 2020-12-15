package com.module;

import android.content.Context;
import android.content.SharedPreferences;
import android.text.TextUtils;

import java.util.Map;

public class SharePreUtil {

    public static Map<String,?> getAll(Context mContext , String mFileName){
        if (null == mContext || TextUtils.isEmpty(mFileName) ) {
            return null;
        }

        SharedPreferences spf = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        return spf.getAll();
    }

    //-------------------------------------------------------------------------------------
    public static String getString(Context mContext , String mFileName , String mKey){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return "";
        }

        SharedPreferences spf = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        return spf.getString(mKey, "");
    }

    public static void putString(Context mContext , String mFileName , String mKey , String mValue){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return ;
        }

        SharedPreferences shared = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = shared.edit();
        editor.putString(mKey, mValue);
        editor.commit();
    }

    public static void asynPutString(Context mContext , String mFileName , String mKey , String mValue){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return ;
        }

        SharedPreferences shared = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = shared.edit();
        editor.putString(mKey, mValue);
        editor.apply();
    }

    //-------------------------------------------------------------------------------------
    public static int getInt(Context mContext , String mFileName , String mKey){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return 0;
        }

        SharedPreferences spf = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        return spf.getInt(mKey, 0);
    }

    public static void putInt(Context mContext , String mFileName , String mKey , int mValue){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return ;
        }

        SharedPreferences shared = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = shared.edit();
        editor.putInt(mKey, mValue);
        editor.commit();
    }

    public static void asynPutInt(Context mContext , String mFileName , String mKey , int mValue){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return ;
        }

        SharedPreferences shared = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = shared.edit();
        editor.putInt(mKey, mValue);
        editor.apply();
    }

    //-------------------------------------------------------------------------------------
    public static long getLong(Context mContext , String mFileName , String mKey){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return 0;
        }

        SharedPreferences spf = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        return spf.getLong(mKey, 0);
    }

    public static void putLong(Context mContext , String mFileName , String mKey , long mValue){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return ;
        }

        SharedPreferences shared = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = shared.edit();
        editor.putLong(mKey, mValue);
        editor.commit();
    }

    public static void asynPutLong(Context mContext , String mFileName , String mKey , long mValue){
        if (null == mContext || TextUtils.isEmpty(mFileName) || TextUtils.isEmpty(mKey)) {
            return ;
        }

        SharedPreferences shared = mContext.getSharedPreferences(mFileName, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = shared.edit();
        editor.putLong(mKey, mValue);
        editor.apply();
    }
}
