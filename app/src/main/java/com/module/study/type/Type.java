package com.module.study.type;

public final class Type {

    static {
        System.loadLibrary("study");
    }

    //------------------基础数据类型--------------------
    public native boolean paramBoolean(boolean input);
    public native byte paramByte(byte input);
    public native char paramChar(char input);
    public native short paramShort(short input);
    public native int paramInt(int input);
    public native long paramLong(long input);
    public native float paramFloat(float input);
    public native double paramDouble(double input);

    //------------------引用数据类型--------------------
    //jobject
    public native Hello paramObject(Hello input);

    //jstring
    public native String paramString(String input);

    //jarray
    public native boolean[] paramBooleanArray(boolean[] input);
    public native byte[] paramByteArray(byte[] input);
    public native char[] paramCharArray(char[] input);
    public native short[] paramShortArray(short[] input);
    public native int[] paramIntArray(int[] input);
    public native long[] paramLongArray(long[] input);
    public native float[] paramFloatArray(float[] input);
    public native double[] paramDoubleArray(double[] input);

    //jthrowable

    //-----------------------------------
    public native void maxLocalCapacity(int input);

}
