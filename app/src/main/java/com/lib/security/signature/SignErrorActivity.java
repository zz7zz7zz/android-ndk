package com.lib.security.signature;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.module.security.R;

public class SignErrorActivity extends AppCompatActivity {

    private Handler mHandler = new Handler();
    private int sign_error_ttl;
    private TextView security_sign_error_prompt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        Intent mIntent = getIntent();
        if(null != mIntent){
            sign_error_ttl = mIntent.getIntExtra("sign_error_ttl",10);
        }
        setContentView(R.layout.security_signautre_error);

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

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return true;
    }
}
