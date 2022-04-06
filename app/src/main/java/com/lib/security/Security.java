package com.lib.security;

import android.content.Context;
import android.util.Log;

import com.lib.security.signature.Signature;

public class Security {

    public static int doSecurityCheck(Context mContext){
        long start = System.currentTimeMillis();
        int errCodeNative = new Signature().verifySignature(mContext);
        Log.v("Testing","verifySignature errCodeNative "+ errCodeNative+" cost " + (System.currentTimeMillis() -start));
        return new Signature().verifySignature(mContext);
    }

}
