package com.module.net.socket;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;

import com.module.SharePreUtil;
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
		findViewById(R.id.reOpenPage).setOnClickListener(listener);
		
		ip=(EditText) findViewById(R.id.ip);
		port=(EditText) findViewById(R.id.port);
		sendContent=(EditText) findViewById(R.id.sendContent);
		recContent=(EditText) findViewById(R.id.recContent);

		String host = SharePreUtil.getString(getApplicationContext(),"SocketServerInfo","host");
		int iPort = SharePreUtil.getInt(getApplicationContext(),"SocketServerInfo","port");
		if(TextUtils.isEmpty(host)){
			host = "192.168.1.100";
		}
		if(iPort == 0){
			iPort = 9999;
		}

		ip.setText(host);
		port.setText(""+iPort);

		mClient = new CSocket();
	}

	private OnClickListener listener=new OnClickListener() {

		@Override
		public void onClick(View v) {
			String host = ip.getText().toString().trim();
			int iPort = Integer.valueOf(port.getText().toString());
			switch(v.getId())
			{
                case R.id.set_ip_port:
					SharePreUtil.asynPutString(getApplicationContext(),"SocketServerInfo","host",host);
					SharePreUtil.asynPutInt(getApplicationContext(),"SocketServerInfo","port",iPort);
                    break;

				case R.id.open:
					mClient.open(host, iPort, CSocketClientActivity.this);
//					for (int i = 0;i<1;i++){
//						mClient = new CSocket();
//						mClient.open(ip.getText().toString(), Integer.valueOf(port.getText().toString()), CSocketClientActivity.this);
//					}
					break;
					
				case R.id.close:
					mClient.close();
					mClient = new CSocket();
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

				case R.id.reOpenPage:
					Intent mIntent = new Intent(getApplicationContext(),CSocketClientActivity.class);
					startActivity(mIntent);
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
	public void onSocketResponse(final byte[] bytes) {

		StringBuilder sb = new StringBuilder();
		for (int i = 0;i<bytes.length;i++){
			sb.append(bytes[i]);
			if(i != bytes.length -1){
				sb.append(",");
			}
		}
		Log.v("Testing","onSocketResponse tid "+android.os.Process.myTid()+" data " + sb.toString());

		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				String old = recContent.getText().toString();
				recContent.setText(new String(bytes) + "\n" + old);
			}
		});

//		Log.v("Testing","onSocketResponse tid "+android.os.Process.myTid()+" count " + i);
		i++;
	}
	static int i;
}
