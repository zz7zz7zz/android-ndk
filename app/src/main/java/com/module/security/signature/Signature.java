package com.module.security.signature;

import android.content.Context;

public class Signature {

    static {
        System.loadLibrary("signature");
    }

    public native void verifySignature(Context mContext);

}
