package com.lib.security.signature;

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
    private int max_count_down_ms = 10;
    private TextView security_sign_error_prompt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.security_signautre_error);

        security_sign_error_prompt = findViewById(R.id.security_sign_error_prompt);
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {

                max_count_down_ms --;
                if(max_count_down_ms < 0){
//                    Process.killProcess(Process.myPid());
//                    System.exit(-1);
                    return;
                }

                String str = "⚠ 签名校验失败， 应用将在 " + max_count_down_ms + " 秒后将关闭应用，请前往官方下载正版应用";
                security_sign_error_prompt.setText(str);
                security_sign_error_prompt.setVisibility(View.VISIBLE);
                mHandler.postDelayed(this,1000);
            }
        },1000);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return true;
    }
}
