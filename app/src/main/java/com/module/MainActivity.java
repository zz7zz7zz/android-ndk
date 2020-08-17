package com.module;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.module.media.FFmpegActivity;
import com.module.net.socket.CSocketClientActivity;
import com.module.security.R;
import com.module.security.signature.SignatureActivity;
import com.module.study.type.StudyActivity;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.jni_main);

        findViewById(R.id.jni_study).setOnClickListener(this);
        findViewById(R.id.jni_signature).setOnClickListener(this);
        findViewById(R.id.jni_ffmepg).setOnClickListener(this);
        findViewById(R.id.jni_csocket).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.jni_study:
                startActivity(StudyActivity.class);
                break;

            case R.id.jni_signature:
                startActivity(SignatureActivity.class);
                break;

            case R.id.jni_ffmepg:
                startActivity(FFmpegActivity.class);
                break;

            case R.id.jni_csocket:
                startActivity(CSocketClientActivity.class);
                break;
        }
    }

    private void startActivity(Class cls){
        Intent mIntent = new Intent(this,cls);
        startActivity(mIntent);
    }
}
