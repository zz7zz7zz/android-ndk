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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;


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

	public static void main(String[] args) {

// 1.Android is always little-endian.
//		static ByteOrder byteOrder() {
//			// Android-removed: Android is always little-endian.
//        /*
//        if (byteOrder == null)
//            throw new Error("Unknown byte order");
//        */
//			return byteOrder;
//		}
//
//		static {
//			// BEGIN Android-changed: Android is always little-endian.
//        /*
//        long a = unsafe.allocateMemory(8);
//        try {
//            unsafe.putLong(a, 0x0102030405060708L);
//            byte b = unsafe.getByte(a);
//            switch (b) {
//            case 0x01: byteOrder = ByteOrder.BIG_ENDIAN;     break;
//            case 0x08: byteOrder = ByteOrder.LITTLE_ENDIAN;  break;
//            default:
//                assert false;
//                byteOrder = null;
//            }
//        } finally {
//            unsafe.freeMemory(a);
//        }
//        */
//			byteOrder = ByteOrder.LITTLE_ENDIAN;
//			// END Android-changed: Android is always little-endian.
//		}



		if (ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN) {
			System.out.println("BIG_ENDIAN   ");
		} else {
			System.out.println("LITTLE_ENDIAN");
		}

		byte [] fourBytes = new byte[]{01,02,03,04};
//		1.把小端数据转换成int

		int i = ByteBuffer.wrap(fourBytes).order(ByteOrder.LITTLE_ENDIAN).getInt();

//		2.把大端数据转换成int

		int j = ByteBuffer.wrap(fourBytes).order(ByteOrder.BIG_ENDIAN).getInt();

		System.out.println("LITTLE_ENDIAN value " + Integer.toHexString(i));
		System.out.println("BIG_ENDIAN    value " + Integer.toHexString(j));

		int x = 0x01020304;

		ByteBuffer byteBuffer = ByteBuffer.wrap(new byte[4]);
		byteBuffer.asIntBuffer().put(x);
		String before = Arrays.toString(byteBuffer.array());
		System.out.println("默认字节序："+byteBuffer.order().toString() + "," + "内存数据："+before);

		byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
		byteBuffer.asIntBuffer().put(x);
		String after = Arrays.toString(byteBuffer.array());
		System.out.println("修改字节序："+byteBuffer.order().toString()+","+"内存数据："+after);
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
