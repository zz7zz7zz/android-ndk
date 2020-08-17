package com.module.study.type;

import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;

import com.module.security.R;

public class StudyActivity extends AppCompatActivity implements View.OnClickListener {

    private Type mType = new Type();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.jni_study);

        findViewById(R.id.type_boolean).setOnClickListener(this);
        findViewById(R.id.type_byte).setOnClickListener(this);
        findViewById(R.id.type_char).setOnClickListener(this);
        findViewById(R.id.type_short).setOnClickListener(this);
        findViewById(R.id.type_int).setOnClickListener(this);
        findViewById(R.id.type_long).setOnClickListener(this);
        findViewById(R.id.type_float).setOnClickListener(this);
        findViewById(R.id.type_double).setOnClickListener(this);
        findViewById(R.id.log_1).setOnClickListener(this);
        findViewById(R.id.clear_1).setOnClickListener(this);

        findViewById(R.id.type_object).setOnClickListener(this);
        findViewById(R.id.type_string).setOnClickListener(this);
        findViewById(R.id.type_booleanArray).setOnClickListener(this);
        findViewById(R.id.type_byteArray).setOnClickListener(this);
        findViewById(R.id.type_charArray).setOnClickListener(this);
        findViewById(R.id.type_shortArray).setOnClickListener(this);
        findViewById(R.id.type_intArray).setOnClickListener(this);
        findViewById(R.id.type_longArray).setOnClickListener(this);
        findViewById(R.id.type_floatArray).setOnClickListener(this);
        findViewById(R.id.type_doubleArray).setOnClickListener(this);
        findViewById(R.id.type_maxLocalRef).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {

        switch (v.getId()){

            case R.id.type_boolean:
                Object ret  = mType.paramBoolean(true);
                appendLog(v,ret);
                break;

            case R.id.type_byte:
                ret = mType.paramByte((byte) 1);
                appendLog(v,ret);
                break;

            case R.id.type_char:
                ret = mType.paramChar('A');
                appendLog(v,ret);
                break;

            case R.id.type_short:
                ret = mType.paramShort( (short) 3);
                appendLog(v,ret);
                break;

            case R.id.type_int:
                ret = mType.paramInt( 4);
                appendLog(v,ret);
                break;

            case R.id.type_long:
                ret = mType.paramLong( 5L);
                appendLog(v,ret);
                break;

            case R.id.type_float:
                ret = mType.paramFloat( 6.66f);
                appendLog(v,ret);
                break;

            case R.id.type_double:
                ret = mType.paramDouble( 7.77d);
                appendLog(v,ret);
                break;



            case R.id.type_object:
                ret = mType.paramObject(new Hello("Yang",30));
                appendLog(v,ret);
                break;

            case R.id.type_string:
                ret = mType.paramString("I am string");
                appendLog(v,ret);
                break;

            case R.id.type_booleanArray:
                ret = mType.paramBooleanArray(new boolean[]{true,false,false,true});
                appendLog(v,ret);
                break;

            case R.id.type_byteArray:
                ret = mType.paramByteArray(new byte[]{1,2,3,4});
                appendLog(v,ret);
                break;

            case R.id.type_charArray:
                ret = mType.paramCharArray(new char[]{'A','B','C','D'});
                appendLog(v,ret);
                break;

            case R.id.type_shortArray:
                ret = mType.paramShortArray(new short[]{11,12,13,14});
                appendLog(v,ret);
                break;

            case R.id.type_intArray:
                ret = mType.paramIntArray(new int[]{21,22,23,24});
                appendLog(v,ret);
                break;

            case R.id.type_longArray:
                ret = mType.paramLongArray(new long[]{31L,32L,33L,34L});
                appendLog(v,ret);
                break;

            case R.id.type_floatArray:
                ret = mType.paramFloatArray(new float[]{41f,42f,43f,44f});
                appendLog(v,ret);
                break;

            case R.id.type_doubleArray:
                ret = mType.paramDoubleArray(new double[]{51d,52d,53d,54d});
                appendLog(v,ret);
                break;

            case R.id.type_maxLocalRef:
                mType.maxLocalCapacity(512);
//                appendLog(v,ret);
                break;

            case R.id.clear_1:
                ((EditText)(findViewById(R.id.log_1))).setText("");
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

        EditText log1=findViewById(R.id.log_1);
        log1.append("\n" + value);
    }


}
