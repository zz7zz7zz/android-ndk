package com.lib.security.signature;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.util.Log;


import com.module.security.BuildConfig;

import java.security.MessageDigest;


public final class SignatureUtil {

    public final static String MD5 = "MD5";
    public final static String SHA1 = "SHA-1";
    public final static String SHA256 = "SHA-256";

    public static String getMd5(Context mContext){
        return getSingInfo(mContext,mContext.getPackageName(),MD5);
    }

    public static String getSha1(Context mContext){
        return getSingInfo(mContext,mContext.getPackageName(),SHA1);
    }

    public static String getSha256(Context mContext){
        return getSingInfo(mContext,mContext.getPackageName(),SHA256);
    }

    //返回一个签名的对应类型的字符串
    public static String getSingInfo(Context context, String packageName, String algorithm) {
        String ret = null;
        switch (algorithm){
            case MD5:
            case SHA1:
            case SHA256:
                Signature[] signs = getSignatures(context, packageName);
                if ((signs == null) || (signs.length == 0)) {
                    break;
                }
                ret = getSignatureString(signs[0], algorithm);
        }
        return ret;
    }

    //返回对应包的签名信息
    private static Signature[] getSignatures(Context context, String packageName) {
        try {
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(packageName, PackageManager.GET_SIGNATURES);
            return packageInfo.signatures;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    /*
    //获取相应的类型的字符串（把签名的byte[]信息转换成16进制）
    private static String getSignatureString(Signature sig, String type) {
        byte[] hexBytes = sig.toByteArray();
        String ret = "";
        try {
            MessageDigest digest = MessageDigest.getInstance(type);
            if (digest != null) {
                byte[] digestBytes = digest.digest(hexBytes);

                StringBuilder sb = new StringBuilder();
//--------------------------old--------------------------
//                for (byte digestByte : digestBytes) {
//                    sb.append((Integer.toHexString((digestByte & 0xFF) | 0x100)).substring(1, 3));
//                }
//                ret = sb.toString()
//--------------------------new--------------------------
                for (int i = 0;i<digestBytes.length;i++) {
                    sb.append((Integer.toHexString((digestBytes[i] & 0xFF) | 0x100)).substring(1, 3));
                    if(i != digestBytes.length -1){
                        sb.append(":");
                    }
                }
                ret = sb.toString().toUpperCase();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return ret;
    }*/

    //获取相应的类型的字符串（把签名的byte[]信息转换成16进制）
    private static String getSignatureString(Signature sig, String algorithm) {
        byte[] hexBytes = sig.toByteArray();

        printByteData("sign " + algorithm ,hexBytes);

        String ret = "";
        try {
            MessageDigest digest = MessageDigest.getInstance(algorithm);
            if (digest != null) {
                digest.update(hexBytes);
                byte[] digestBytes = digest.digest();

                printByteData("digest "+ algorithm,digestBytes);

                StringBuilder sb = new StringBuilder(digestBytes.length *2 + (digestBytes.length-1));
                for (int i = 0;i<digestBytes.length;i++) {
//--------------------------old--------------------------
//                    sb.append((Integer.toHexString((digestBytes[i] & 0xFF) | 0x100)).substring(1, 3));
//--------------------------new--------------------------
                    String str = Integer.toString(0xFF & digestBytes[i], 16);
                    if (str.length() == 1) {
                        str = "0" + str;
                    }
                    sb.append(str);

                    if(i != digestBytes.length -1){
                        sb.append(":");
                    }
                }
                ret = sb.toString().toUpperCase();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return ret;
    }

    private static void printByteData(String prefix,byte[] hexBytes ){
        if(!BuildConfig.DEBUG){
            return;
        }
        //打印byte值
        StringBuilder sb1 = new StringBuilder(hexBytes.length);
        sb1.append(prefix+"[] = {");
        for (int i = 0;i<hexBytes.length;i++) {
            String str = ""+hexBytes[i];
            sb1.append(str);
            if(i != hexBytes.length -1){
                sb1.append(",");
            }
            if(i % 100 == 0){
                sb1.append("\n");
            }
        }
        sb1.append("};");

        //打印int价值
//        StringBuilder sb1 = new StringBuilder(hexBytes.length);
//        sb1.append("sign[] = {");
//        for (int i = 0;i<hexBytes.length;i++) {
//            sb1.append("0x");
//            String str = Integer.toString(0xFF & hexBytes[i], 16);
//            if (str.length() == 1) {
//                str = "0" + str;
//            }
//            sb1.append(str);
//            if(i != hexBytes.length -1){
//                sb1.append(",");
//            }
//            if(i % 100 == 0){
//                sb1.append("\n");
//            }
//        }
//        sb1.append("};");

        Log.v("Testing",prefix+" printByteData length "+hexBytes.length  + " sb1 " + sb1.length());
        if(sb1.length() > 500){
            Log.v("Testing ","first half : " + sb1.substring(0,sb1.length()/2));
            Log.v("Testing","the second half:  "+sb1.substring(sb1.length()/2));
        }else{
            Log.v("Testing ","all : " + sb1.toString());
        }
    }
}
