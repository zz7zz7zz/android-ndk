package com.lib.security.signature;

import android.content.Context;

public class Signature {

    static {
        System.loadLibrary("signature");
    }

    public native int verifySignature(Context mContext);

}
