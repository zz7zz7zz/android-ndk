package com.module.net.socket;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;

import com.module.security.R;


public class CSocketClientActivity extends Activity implements CSocketHandler{

	private CSocket mClient =null;
	private EditText ip,port,sendContent,recContent;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.jni_net_socket);
		initView();
		Log.v("Testing","onCreate tid "+android.os.Process.myTid());
	}
	 
	
	private void initView()
	{
        findViewById(R.id.set_ip_port).setOnClickListener(listener);
		findViewById(R.id.open).setOnClickListener(listener);
		findViewById(R.id.close).setOnClickListener(listener);
		findViewById(R.id.reconn).setOnClickListener(listener);
		findViewById(R.id.send).setOnClickListener(listener);
		findViewById(R.id.clear).setOnClickListener(listener);
		
		ip=(EditText) findViewById(R.id.ip);
		port=(EditText) findViewById(R.id.port);
		sendContent=(EditText) findViewById(R.id.sendContent);
		recContent=(EditText) findViewById(R.id.recContent);

		ip.setText("192.168.1.56");
		port.setText("9999");

		mClient = new CSocket();
	}

	private OnClickListener listener=new OnClickListener() {

		@Override
		public void onClick(View v) {
			switch(v.getId())
			{
                case R.id.set_ip_port:
                    break;

				case R.id.open:

					mClient.open(ip.getText().toString(), Integer.valueOf(port.getText().toString()), CSocketClientActivity.this);

//					for (int i = 0;i<1;i++){
//						mClient = new CSocket();
//						mClient.open(ip.getText().toString(), Integer.valueOf(port.getText().toString()), CSocketClientActivity.this);
//					}
					break;
					
				case R.id.close:
					mClient.close();
					break;
					
				case R.id.reconn:
					mClient.reconnect();
					break;
					
				case R.id.send:
					mClient.write(sendContent.getText().toString().getBytes());
					sendContent.setText("");
					break;
					
				case R.id.clear:
					recContent.setText("");
					break;
			}
		}
	};

	@Override
	public void onBackPressed() {
		super.onBackPressed();
		mClient.close();
		i = 0;
	}

	@Override
	public void onSocketConnectResult(boolean isSuccess) {

		Log.v("Testing","onSocketConnectResult tid "+android.os.Process.myTid()+" isSuccess " + isSuccess);
	}

	@Override
	public void onSocketResponse(byte[] bytes) {
		Log.v("Testing","onSocketResponse tid "+android.os.Process.myTid()+" count " + i);
		i++;
	}
	static int i;
}
