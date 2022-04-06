package com.lib.security.signature;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.module.security.R;

import java.text.SimpleDateFormat;
import java.util.Date;

public class SignErrorActivity extends AppCompatActivity {

    private Handler mHandler = new Handler();
    private int sign_error_ttl = 15;

    private TextView security_sign_result;
    private TextView security_sign_error_prompt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
//        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            getWindow().setStatusBarColor(Color.TRANSPARENT);
            getWindow()
                    .getDecorView()
                    .setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            getWindow()
                    .setFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS, WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        }

        Intent mIntent = getIntent();
        if(null != mIntent){
            sign_error_ttl = mIntent.getIntExtra("sign_error_ttl",sign_error_ttl);
        }
        setContentView(R.layout.security_signautre_error);

        //应用信息
        security_sign_result = findViewById(R.id.security_sign_result);
        StringBuilder sb = new StringBuilder(512);
        sb.append("时间:").append("\n")
                .append(new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date(System.currentTimeMillis()))).append("\n\n");

        sb.append("包名:").append("\n")
            .append(getApplication().getPackageName()).append("\n\n");

        sb.append("应用信息:").append("\n")
                .append(getAppSummaryInfo()).append("\n\n");

        sb.append("签名信息:").append("\n")
            .append("MD5: ").append("\n").append(SignatureUtil.getMd5(this)).append("\n")
            .append("SHA-1: ").append("\n").append(SignatureUtil.getSha1(this)).append("\n")
            .append("SHA-256: ").append("\n").append(SignatureUtil.getSha256(this)).append("\n");
        final String result = sb.toString();
        security_sign_result.setText(result);

        View.OnClickListener listener = new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (android.os.Build.VERSION.SDK_INT > 11) {
                    android.content.ClipboardManager c = (android.content.ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
                    c.setText(result);
                } else {
                    android.text.ClipboardManager c = (android.text.ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
                    c.setText(result);
                }
                Toast.makeText(getApplicationContext(),"信息已复制",Toast.LENGTH_SHORT).show();
            }
        };
        security_sign_result.setOnClickListener(listener);
        findViewById(R.id.security_sign_result_click).setOnClickListener(listener);


        //倒计时信息
        security_sign_error_prompt = findViewById(R.id.security_sign_error_prompt);
        mHandler.post(new Runnable() {
            @Override
            public void run() {

                sign_error_ttl--;
                if(sign_error_ttl < 0){
//                    Process.killProcess(Process.myPid());
//                    System.exit(-1);
                    return;
                }

                String str = "⚠ 签名校验失败， 应用将在 " + sign_error_ttl + " 秒后关闭应用，请前往官方下载正版应用";
                security_sign_error_prompt.setText(str);
                security_sign_error_prompt.setVisibility(View.VISIBLE);
                mHandler.postDelayed(this,1000);
            }
        });
    }

    public String getAppSummaryInfo(){
//        String phoneInfo = "Product: " + android.os.Build.PRODUCT +"\n";
//        phoneInfo += ", CPU_ABI: " + android.os.Build.CPU_ABI +"\n";
//        phoneInfo += ", TAGS: " + android.os.Build.TAGS +"\n";
//        phoneInfo += ", VERSION_CODES.BASE: " + android.os.Build.VERSION_CODES.BASE +"\n";
//        phoneInfo += ", MODEL: " + android.os.Build.MODEL +"\n";
//        phoneInfo += ", SDK: " + android.os.Build.VERSION.SDK +"\n";
//        phoneInfo += ", VERSION.RELEASE: " + android.os.Build.VERSION.RELEASE +"\n";
//        phoneInfo += ", DEVICE: " + android.os.Build.DEVICE +"\n";
//        phoneInfo += ", DISPLAY: " + android.os.Build.DISPLAY +"\n";
//        phoneInfo += ", BRAND: " + android.os.Build.BRAND +"\n";
//        phoneInfo += ", BOARD: " + android.os.Build.BOARD +"\n";
//        phoneInfo += ", FINGERPRINT: " + android.os.Build.FINGERPRINT +"\n";
//        phoneInfo += ", ID: " + android.os.Build.ID +"\n";
//        phoneInfo += ", MANUFACTURER: " + android.os.Build.MANUFACTURER +"\n";
//        phoneInfo += ", USER: " + android.os.Build.USER;

        String versionName = null;
        int    versionCode = 0;
        String brand = android.os.Build.BRAND;
        String model = android.os.Build.MODEL;
        String cpu_abi = android.os.Build.CPU_ABI;
        int    sdk_int =  Build.VERSION.SDK_INT;
        String systemVersion =  android.os.Build.VERSION.RELEASE;

        try {
            PackageManager pm = getPackageManager();
            PackageInfo pi  = pm.getPackageInfo(getPackageName(), 0);
            versionName = pi.versionName;
            versionCode = pi.versionCode;
        } catch (Exception e) {
            e.printStackTrace();
        }

        StringBuilder sb = new StringBuilder(256);
        sb.append("uid: ").append("").append("\n");
        sb.append("channel: ").append("gp").append("\n");
        sb.append("versionName: ").append(versionName).append("\n");
        sb.append("versionCode: ").append(versionCode).append("\n");

        sb.append("systemVersion: ").append("Android ").append(systemVersion).append(" / level ").append(sdk_int).append("\n");
        sb.append("brand: ").append(brand).append("\n");
        sb.append("model: ").append(model).append("\n");
        sb.append("cpu_abi: ").append(cpu_abi);

        return sb.toString();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return true;
    }
}
