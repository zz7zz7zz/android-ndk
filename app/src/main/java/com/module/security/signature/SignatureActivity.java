package com.module.security.signature;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.module.security.R;

public class SignatureActivity extends AppCompatActivity {

    private Signature mSignature = new Signature();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.jni_signautre);

//        Toast.makeText(this,"",Toast.LENGTH_LONG).show();
        StringBuilder sb = new StringBuilder();
        sb.append("MD5:").append("\n").append(SignatureUtil.getMd5(this)).append("\n\n").
        append("SHA-1:").append("\n").append(SignatureUtil.getSha1(this)).append("\n\n").
        append("SHA-256:").append("\n").append(SignatureUtil.getSha256(this)).append("\n\n");

        TextView sign_from_java = findViewById(R.id.sign_from_java);
        sign_from_java.setText(sb.toString());

        TextView sign_verify = findViewById(R.id.sign_verify);
        sign_verify.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                JavaSignature.verifySignature(getApplicationContext());

                long start = System.currentTimeMillis();
                mSignature.verifySignature(getApplicationContext());
                Log.v("Testing","verifySignature cost " + (System.currentTimeMillis() -start));
            }
        });
    }

}
