
package com.module.security.signature;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import com.module.utils.AppUtil;

import java.security.MessageDigest;

//https://www.cnblogs.com/shenchanghui/p/7910695.html
//https://blog.csdn.net/u011913612/article/details/52668422

public final class JavaSignature {

    private static final String TAG = "JavaSignature";

    public static final String[] pkgs= {
            "com.module.ndk",//ndk测试
            "com.douyu.yanhuan",//陌遇马甲包
            "cn.douyu.moyu",
            "cn.cstf.cpzd",
            "com.douyu.milian",
            "com.mengyu.hualiao",
            "com.douyu.myjy"
    };
//    原始值
//    public static String[] md5SignOriginal= {
//            "50:AD:25:9D:20:82:2C:0E:07:21:77:F8:4B:FB:2F:FF",//ndk测试
//            "29:00:D0:36:67:3C:D8:F1:C2:05:DF:A4:A7:F0:FE:33"//陌遇测试
//    };
//                /||\
//                 ||
//                 ||
//                \||/
//    加密后值
    public static final String[] md5SignEncrypt0xff = {
            "AF:52:DA:62:DF:7D:D3:F1:F8:DE:88:07:B4:04:D0:00",//ndk测试
            "D6:FF:2F:C9:98:C3:27:0E:3D:FA:20:5B:58:0F:01:CC"//陌遇测试
    };

    public static int verifySignature(Context mContext){
        if(null == mContext){
            throw new NullPointerException("param mContext is null");
        }

        //1.对比包名
        Context context = AppUtil.getApplication();//防止别人注入自定义Context
        if(null == context){
            context = mContext;
        }
        boolean isPkgMatch = false;
        String pkg = context.getPackageName();
        for (int i = 0; i < pkgs.length; i++) {
            if(pkgs[i].equals(pkg)){
                isPkgMatch = true;
                break;
            }
        }
        Log.v(TAG,"isPkgMatch " +isPkgMatch);
        if(!isPkgMatch){
            on_callback_verify_failed(mContext);
            return -1;
        }

        //2.签名对比
        byte[] signBytes;
        try {
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(pkg, PackageManager.GET_SIGNATURES);
            Signature[] signs = packageInfo.signatures;
            if (signs == null) {
                return -22;
            }else if(signs.length == 0){
                return -23;
            }
            signBytes = signs[0].toByteArray();
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return -21;
        }

        byte[] digestBytes;
        try {
            MessageDigest digest = MessageDigest.getInstance("MD5");//"MD5"/"SHA-1"/"SHA-256"
            if (digest != null) {
                digest.update(signBytes);
                digestBytes = digest.digest();
            }else{
                return -25;
            }
        } catch (Exception e) {
            e.printStackTrace();
            return -24;
        }
        if(null == digestBytes){
            return -26;
        }else if( digestBytes.length == 0){
            return -27;
        }

        StringBuilder sb = new StringBuilder(digestBytes.length *2 + (digestBytes.length-1));
        for (int i = 0;i<digestBytes.length;i++) {
//          String str = Integer.toString(0xFF & digestBytes[i], 16);//原始值
            String str = Integer.toString(0xFF & (~(0xFF & digestBytes[i])), 16);//加密后值
            if (str.length() == 1) {
                str = "0" + str;
            }
            sb.append(str);
            if(i != digestBytes.length -1){
                sb.append(":");
            }
        }
        String MD5Sign = sb.toString().toUpperCase();

//        String[] md5SignOriginal = encrypt0xff(md5SignEncrypt0xff);
        boolean isSignMatch = false;
        for (int i = 0; i < md5SignEncrypt0xff.length; i++) {
            if(md5SignEncrypt0xff[i].equals(MD5Sign)){
                isSignMatch = true;
                break;
            }
        }
        Log.v(TAG,"isSignMatch " +isSignMatch);
        if(!isSignMatch){
            on_callback_verify_failed(mContext);
            return -2;
        }

        return 0;
    }

    public static String[] encrypt0xff(String[] md5Signs){
        String[] ret = new String[md5Signs.length];
        for(int i = 0 ;i<md5Signs.length;i++){
            ret[i] = encrypt0xff(md5Signs[i]);
        }
        return ret;
    }

    public static String encrypt0xff(String md5Sign){
        String[] sByteValues =md5Sign.split(":");
        StringBuilder sb = new StringBuilder(sByteValues.length *2 + (sByteValues.length-1));
        for(int i = 0;i<sByteValues.length;i++){
            int hexValue = Integer.valueOf(sByteValues[i], 16);
            String str = Integer.toString(0xFF & ~hexValue, 16);
            if (str.length() == 1) {
                str = "0" + str;
            }
            sb.append(str);
            if(i != sByteValues.length -1){
                sb.append(":");
            }
        }
        return sb.toString().toUpperCase();
    }

    public static void on_callback_verify_failed(Context mContext){
        //1.弹出Toast
        Toast.makeText(mContext,"应用校验错误，将在 5s 后关闭应用 !",Toast.LENGTH_SHORT).show();

        //2.启动错误页面
        Intent mIntent = new Intent(mContext,SignErrorActivity.class);
        mIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_NEW_TASK);
        mContext.startActivity(mIntent);

        //3.延迟关闭应用
//            new Handler().postDelayed(new Runnable() {
//                @Override
//                public void run() {
//                    Process.killProcess(Process.myPid());
//                    System.exit(-1);
//                }
//            },5000);
        new Handler().postDelayed(new SignExitTask(),5000);
    }
}
