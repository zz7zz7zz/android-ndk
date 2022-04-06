package com.lib.security;

import android.content.Context;

import com.lib.security.signature.Signature;

public class Security {

    public static int doSecurityCheck(Context mContext){
        return new Signature().verifySignature(mContext);
    }

}
