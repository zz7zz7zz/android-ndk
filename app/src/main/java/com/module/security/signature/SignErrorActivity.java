package com.module.security.signature;

import android.os.Bundle;
import android.os.Handler;
import android.os.Process;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.module.security.R;

public class SignErrorActivity extends AppCompatActivity {


    private Handler mHandler = new Handler();
    private int max_count_down_ms = 5+1;

    private TextView sign_error_prompt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.jni_signautre_error);

        sign_error_prompt = findViewById(R.id.sign_error_prompt);
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {

                max_count_down_ms --;
                if(max_count_down_ms < 0){
                    Process.killProcess(Process.myPid());
                    System.exit(0);
                    return;
                }

                String str = "⚠️ sign error , The application will exit after " + max_count_down_ms + (max_count_down_ms >1 ?  " seconds ":" second ");
                sign_error_prompt.setText(str);
                mHandler.postDelayed(this,1000);
            }
        },1000);
    }

}
