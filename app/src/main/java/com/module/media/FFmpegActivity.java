package com.module.media;


import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.module.security.R;

public class FFmpegActivity extends AppCompatActivity implements View.OnClickListener {

    private String TAG = "FFmpegActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.jni_ffmpeg);
        initView();
    }

    private void initView(){

        findViewById(R.id.clear_1).setOnClickListener(this);

        findViewById(R.id.ffmpeg_getUrlProtocol).setOnClickListener(this);
        findViewById(R.id.ffmpeg_getAvformat).setOnClickListener(this);
        findViewById(R.id.ffmpeg_getAvCodec).setOnClickListener(this);
        findViewById(R.id.ffmpeg_getAvFilter).setOnClickListener(this);
        findViewById(R.id.ffmpeg_configuration).setOnClickListener(this);

        findViewById(R.id.ffmpeg_thumbnail).setOnClickListener(this);
        findViewById(R.id.ffmpeg_preview).setOnClickListener(this);
        findViewById(R.id.ffmpeg_cut).setOnClickListener(this);

        findViewById(R.id.ffmpeg_thumbnail_batch).setOnClickListener(this);
        findViewById(R.id.ffmpeg_preview_batch).setOnClickListener(this);
        findViewById(R.id.ffmpeg_cut_batch).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {

        String ret;
        switch (v.getId()){
            case R.id.ffmpeg_getUrlProtocol:
                ret = FFmpeg.getUrlProtocol();
                appendLog(v,ret);
                break;
            case R.id.ffmpeg_getAvformat:
                ret = FFmpeg.getAvformat();
                appendLog(v,ret);
                break;
            case R.id.ffmpeg_getAvCodec:
                ret = FFmpeg.getAvCodec();
                appendLog(v,ret);
                break;
            case R.id.ffmpeg_getAvFilter:
                ret = FFmpeg.getAvFilter();
                appendLog(v,ret);
                break;

            case R.id.ffmpeg_configuration:
                ret = FFmpeg.getConfiguration();
                appendLog(v,ret);
                break;

            case R.id.ffmpeg_thumbnail:
                //压缩分辨率
                String [] param = "ffmpeg -i /sdcard/input.mp4 -strict -2 -vf scale=-1:1920 /sdcard/2020.05.17.out.mp4".split(" ");
                int iRet = FFmpeg.execute(param);

//                int iRet = FFmpeg.execute(new String[]{"ffmpeg"});
                appendLog(v,iRet);
                break;

            case R.id.ffmpeg_preview:
                break;

            case R.id.ffmpeg_cut:
                break;

            case R.id.ffmpeg_thumbnail_batch:
                break;

            case R.id.ffmpeg_preview_batch:
                break;

            case R.id.ffmpeg_cut_batch:
                break;

            case R.id.clear_1:
                ((TextView)(findViewById(R.id.log_1))).setText("");
                break;
        }
    }

    private void appendLog(View view,Object value){
        if(null == value){
            return;
        }

        ViewGroup viewGroup = ((ViewGroup)view.getParent());
        View logViewGroup = viewGroup.findViewById(R.id.log_parentid_1);
        viewGroup.removeView(logViewGroup);

        int index = viewGroup.indexOfChild(view);
        viewGroup.addView(logViewGroup,index+1);

        TextView log1=findViewById(R.id.log_1);
        log1.setText(""+value);
    }
}
