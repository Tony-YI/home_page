package com.boyaa.chat.demo;
	import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.HashMap;
import java.util.Timer;
import java.util.TimerTask;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff.Mode;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.WebSettings;
import android.webkit.WebSettings.LayoutAlgorithm;
import android.webkit.WebSettings.PluginState;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.android.test.ClientSocket;
import com.boyaa.chat.R;
import com.boyaa.push.lib.service.Client;
import com.boyaa.push.lib.service.ISocketResponse;
import com.boyaa.push.lib.service.Packet;
import com.view.MyView;



public  class MainActivity extends Activity  implements OnTouchListener{
	
	
	private SensorManager sensorManager;
	private MySensorEventListener mySensorEventListener;


	  private RadioGroup styleGroup=null; 
	    private RadioButton s1RadioButton=null; 
	    private RadioButton s2RadioButton=null; 
	    private RadioButton s3RadioButton=null; 
	    
	    private RadioGroup handGroup=null; 
	    private RadioButton h1RadioButton=null; 
	    private RadioButton h2RadioButton=null; 
	    
	    
	    private int style_choice=1; 
	    private int hand_choice=1; 


	
	private int x=0;//绘画开始的横坐标
	private int y=0;//绘画开始的纵坐标
	private int m=0;//绘画结束的横坐标
	private int n=0;//绘画结束的纵坐标
	private int width=0;//绘画的宽度
	private int height=0;//绘画的高度
	private Bitmap bitmap;//生成的位图
	private MyView myView;//绘画选择区域
	private Button button;
	
	
	private int origin_x=0;//绘画结束的横坐标
	private int origin_y=0;//绘画结束的纵坐标
	
	private int ratiox=0;//绘画开始的横坐标
	private int ratioy=0;//绘画开始的纵坐标
	private int ratioheight=0;//绘画开始的横坐标
	private int ratiowidth=0;//绘画开始的纵坐标
	
	private int image_x=0;//绘画开始的横坐标
	private int image_y=0;//绘画开始的纵坐标
	private int image_width=0;//绘画结束的横坐标
	private int image_height=0;//绘画结束的纵坐标

	private int cuhk_logo=1;
	
	private int viewcount=1;


	private int degree=0;
	private int connected=0;
	
	private int file_transfer_port=12000;
	
	private Client user=null;
//	private EditText ip;
	private TextView status;
	private WebView myWebView;
	private WebView myWebView2;
	
	 private String savePath = "/mnt/sdcard/intelcup.jpg";  
	
	private String camera_ip="192.168.1.101";
	private String board_ip="192.168.43.186";
	SoundPool player,play;
	HashMap<Integer,Integer> soundMap;
	
	private SensorManager sm;
	//需要两个Sensor
	private Sensor aSensor;
	private Sensor mSensor;
	float[] accelerometerValues = new float[3];
	float[] magneticFieldValues = new float[3];
	private static final String TAG = "sensor";

	
	 Packet packet=new Packet();
	 
	ImageView imageView;
   
	   private ClientSocket cs = null;  
	   
	   private SeekBar rollBar;
	   	private SeekBar pitchBar;
	   	
	   	private TextView roll;
	   	private TextView pitch;
	   	
         private int roll_lastProgress = 45;
         private int roll_newProgress = 45;
         private int pitch_lastProgress = 45;
         private int pitch_newProgress = 45;
         
		private final int STATE_NOT_CONNECTED=1;
		private final int STATE_CONNECTED=2;
		private final int STATE_COMPASS=3;
		private final int STATE_MODE=4;
		private final int STATE_SINGLE=5;
		private final int STATE_DOUBLE=6;
		private final int STATE_MULTIPLE=7;
		private final int STATE_VIDEO=8;
		private final int STATE_START=9;
		private final int STATE_WAIT_CONFIRM=10;
		private final int STATE_CONFIRM=11;
		private final int STATE_WAIT_FINISH=10;
		private final int STATE_FINISH=13;
	
		private int state=STATE_NOT_CONNECTED;
		private float currentDegree = 0f;

			
   
        
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		
		
		
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_main);
		

		  mySensorEventListener= new MySensorEventListener();
	      
	        //��ȡ��Ӧ��������
	        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
		  
		
		
//		ip=(EditText) findViewById(R.id.ip);
		status=(TextView) findViewById(R.id.status);
		
//		ip.setText(board_ip);
		status.setText("off");
		
	
	
		initView();
		


	
		
		 soundMap=new HashMap<Integer,Integer>();
	        player=new SoundPool(1,AudioManager.STREAM_MUSIC,10);
	        play=new SoundPool(1,AudioManager.STREAM_MUSIC,10);
	        play.load(this, R.raw.test, 10);
	        soundMap.put(1,player.load(this, R.raw.test, 10));
	        play.load(this, R.raw.sitdown, 10);
	        soundMap.put(2,player.load(this, R.raw.sitdown, 10));
	
	        play.load(this, R.raw.wancheng, 10);
	        soundMap.put(3,player.load(this, R.raw.wancheng, 10));
        //显示
		
		user=new Client(this.getApplicationContext(),socketListener);
//		
//		sm = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
//		aSensor = sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
//		mSensor = sm.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
//		
//		
//			
//		
//		
//		sm.registerListener(myListener, aSensor, SensorManager.SENSOR_DELAY_NORMAL);
//		sm.registerListener(myListener, mSensor,SensorManager.SENSOR_DELAY_NORMAL);
//		calculateOrientation();
		
		
		
		
		
		  rollBar=(SeekBar)findViewById(R.id.rollBar);
		  pitchBar=(SeekBar)findViewById(R.id.pitchBar);
		  rollBar.setMax(90);
		  rollBar.setProgress(45);
		  pitchBar.setProgress(45);
		  pitchBar.setMax(90);
		  roll=(TextView)findViewById(R.id.roll);
	        rollBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
	            /**
	             * 拖动条停止拖动的时候调用
	             */
	            @Override
	            public void onStopTrackingTouch(SeekBar seekBar) {
	            	roll.setText("roll："+(roll_newProgress-45)+"'");
	            }
	            /**
	             * 拖动条开始拖动的时候调用
	             */
	            @Override
	            public void onStartTrackingTouch(SeekBar seekBar) {
	            	roll.setText("start darg");
	            }
	            /**
	             * 拖动条进度改变的时候调用
	             */
	            @Override
	            public void onProgressChanged(SeekBar seekBar, int progress,
	                    boolean fromUser) {
	            	roll_newProgress=progress;

	            	roll.setText("roll："+(roll_newProgress-45)+"'");
	            }
	        });
	        
	        
	        
	        pitch=(TextView)findViewById(R.id.pitch);
	        pitchBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
	            /**
	             * 拖动条停止拖动的时候调用
	             */
	            @Override
	            public void onStopTrackingTouch(SeekBar seekBar) {
	            	pitch.setText("pitch："+(pitch_newProgress-45)+"'");
	            }
	            /**
	             * 拖动条开始拖动的时候调用
	             */
	            @Override
	            public void onStartTrackingTouch(SeekBar seekBar) {
	            	pitch.setText("start darg");
	            }
	            /**
	             * 拖动条进度改变的时候调用
	             */
	            @Override
	            public void onProgressChanged(SeekBar seekBar, int progress,
	                    boolean fromUser) {
	            	pitch_newProgress=progress;
	            	pitch.setText("pitch："+(pitch_newProgress-45)+"'");
	            }
	        });
	 

		
		
		
		
	       
	        
	      
	}
	
	
//	final SensorEventListener myListener = new SensorEventListener() {
//		public void onSensorChanged(SensorEvent sensorEvent) {
//			
//		if (sensorEvent.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
//		magneticFieldValues = sensorEvent.values;
//		
//		if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
//			accelerometerValues = sensorEvent.values;
//		
//		calculateOrientation();
//		}
		
		
//			//可以得到传感器实时测量出来的变化值
//			public void onSensorChanged(SensorEvent event) {
//			//方向传感器
//			if(event.sensor.getType()==Sensor.TYPE_ORIENTATION){
//			//x表示手机指向的方位，0表示北,90表示东，180表示南，270表示西
//			float x = event.values[SensorManager.DATA_X];
//			float y = event.values[SensorManager.DATA_Y];
//			float z = event.values[SensorManager.DATA_Z];
//			
//			  status.setText(x+"");
//			//tv_orientation是界面上的一个TextView标签，不再赘述
//			}
//			}
//	
//			
//			public void onAccuracyChanged(Sensor sensor, int accuracy) {}
//		
//		
//		
//		
//		
//		
//		};
//		
//		protected void onResume() {
//		    Sensor sensor_orientation=sensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION);
//		    sensorManager.registerListener(mySensorEventListener,sensor_orientation, SensorManager.SENSOR_DELAY_UI);
//		super.onResume();
//		}
//		

//		private  void calculateOrientation() {
//		      float[] values = new float[3];
//		      float[] R = new float[9];
//		      SensorManager.getRotationMatrix(R, null, accelerometerValues, magneticFieldValues);	      
//		      SensorManager.getOrientation(R, values);
//
//		      // 要经过一次数据格式的转换，转换为度
//		     
//		      values[0] = (float) Math.toDegrees(values[0]);
//		      degree=(int)values[0];
//		      degree+=90;
//		      if(degree <0)
//		    	  degree = 360+degree;
//		      
////		      if(connected==0)
////		    	  status.setText(degree+"");
//		    	  
//
//		    }
		
	
	
		
		public boolean onTouch(View v, MotionEvent event) {
			if(event.getAction() == MotionEvent.ACTION_DOWN){
				x = 0;
				y = 0;
				width = 0;
				height = 0;
				x = (int) event.getRawX();
				origin_x=x;
				y = (int) event.getRawY();
				origin_y=y;
			}
			if(event.getAction() == MotionEvent.ACTION_MOVE){
//				m = (int) event.getRawX();
				
				if(event.getRawX()>x){
					width = (int)event.getRawX()-x;
					height = (int)event.getRawX()-x;
				}else{
					width = (int)(x-event.getRawX());
					x = (int) event.getRawX();
					
					height = (int)(x-event.getRawX());
					y = (int) event.getRawY();
				}
//				n = (int) event.getRawY();
				
				
				ratiox=(int)(origin_x-image_x)*640/image_width;
				ratioy=(int)(origin_y-image_y)*480/image_height;
				ratiowidth=(int)(width*640/image_width);
				ratioheight=(int)(height*480/image_height);
				
				if(ratiowidth>80)
					ratiowidth=80;
				else if(ratiowidth<45)
					ratiowidth=45;
				
				
				
				myView.setSeat(x, y, x+(ratiowidth*image_width/640), y+(ratiowidth*image_width/640));
				myView.postInvalidate();
			}
			if(event.getAction() == MotionEvent.ACTION_UP){
				if(event.getRawX()>x){
					width = (int)event.getRawX()-x;
					height = (int)event.getRawX()-x;
				}else{
					width = (int)(x-event.getRawX());
					x = (int) event.getRawX();
					
					height = (int)(x-event.getRawX());
					y = (int) event.getRawY();
				}
				
//				image_width=320;
//				image_height=240;
			
				
//				if(event.getY()>y){
//					height = (int) event.getRawY()-y;
//					
//				}else{
//					height = (int)(y-event.getY());
//					
//					y = (int) event.getRawY();
//				}
//				image2.setImageBitmap(getBitmap(this));
			}
			if(myView.isSign()){
				return false;
			}else{
				return true;
			}
		}
		
		
		
		
		
	
	


	
//	
//	public void onPause(){
////		sm.unregisterListener(myListener);
//		super.onPause();
//	}	
	
	
	   public static Bitmap getRoundCornerImage(Bitmap bitmap, int roundPixels)
	    {
	        //创建一个和原始图片一样大小位图
	        Bitmap roundConcerImage = Bitmap.createBitmap(bitmap.getWidth(),
	                bitmap.getHeight(), Config.ARGB_8888);
	        //创建带有位图roundConcerImage的画布
	        Canvas canvas = new Canvas(roundConcerImage);
	        //创建画笔
	        Paint paint = new Paint();
	        //创建一个和原始图片一样大小的矩形
	        Rect rect = new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight());
	        RectF rectF = new RectF(rect);
	        // 去锯齿 
	        paint.setAntiAlias(true);
	        //画一个和原始图片一样大小的圆角矩形
	        canvas.drawRoundRect(rectF, roundPixels, roundPixels, paint);
	        //设置相交模式
	        paint.setXfermode(new PorterDuffXfermode(Mode.SRC_IN));
	        //把图片画到矩形去
	        canvas.drawBitmap(bitmap, null, rect, paint);
	        return roundConcerImage;
	    }
	

	
	private void initView()
	{
		viewcount=1;
//		ip=(EditText) findViewById(R.id.ip);
		status=(TextView) findViewById(R.id.status);
		
//		ip.setText(board_ip);
		
		 imageView = (ImageView)findViewById(R.id.image_view);
		 
		 
		 
		 if(cuhk_logo==1)
		 {
			 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.cuhk);
			 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70));  
		 
			 cuhk_logo=0;
		 }

		 																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																											
		    
		 imageView.post(new Runnable(){   
	        	  
	            @Override  
	            public void run() {  
	                // TODO Auto-generated method stub  
	                  
	                //ImageView的宽和高  
	                Log.d("lxy", "iv_W = " + imageView.getWidth() + ", iv_H = " + imageView.getHeight());  
	  
	                //获得ImageView中Image的真实宽高，  
	                int dw = imageView.getDrawable().getBounds().width();  
	                int dh = imageView.getDrawable().getBounds().height();  
	                Log.d("lxy", "drawable_X = " + dw + ", drawable_Y = " + dh);  
	                  
	                //获得ImageView中Image的变换矩阵  
	                Matrix m = imageView.getImageMatrix();  
	                float[] values = new float[10];  
	                m.getValues(values);  
	                  
	                //Image在绘制过程中的变换矩阵，从中获得x和y方向的缩放系数  
	                float sx = values[0];  
	                float sy = values[4];  
	                Log.d("lxy", "scale_X = " + sx + ", scale_Y = " + sy);  
	                  
	                //计算Image在屏幕上实际绘制的宽高  
	                int cw = (int)(dw * sx);  
	                int ch = (int)(dh * sy);  
	                Log.d("lxy", "caculate_W = " + cw + ", caculate_H = " + ch);  
	                
	            	
	    			int[] location = new int[2];  
	    			imageView.getLocationOnScreen(location);  
	             image_x=  (int) location[0];  
	             image_y = 	(int) location[1];  
	                
	                
	    			image_width=cw;
	    			image_height=ch;
	    			
	    			  Log.v("image_x",image_x+"");
	    		         Log.v("image_y",image_y+"");
	    		         Log.v("image_width",image_width+"");
	    		         Log.v("image_height",image_height+"");
	            }});  
	        
		
        
//      
//         image_width=imageView.getMeasuredWidth();
//         image_height=imageView.getMeasuredHeight();	
         
       
//	        image2 = (ImageView) findViewById(R.id.image2);
	        myView = new MyView(this);
	        button = (Button) findViewById(R.id.pdiy);
			myView.setSign(true);
			imageView.setOnTouchListener(this);
		    this.addContentView(myView, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
		 
		    
		    
		    
			
		    
//		    
//		    myWebView2 = (WebView) findViewById(R.id.webView2);
//			
//			WebSettings setting=myWebView2.getSettings();
//			
//              setting.setJavaScriptEnabled(true);     
//              setting.setPluginState(PluginState.ON);
//              setting.setAllowFileAccess(true);
//              setting.setSupportZoom(true); //可以缩放
//              setting.setBuiltInZoomControls(true); //显示放大缩小 controler                //设置出现缩放工具
//              
//              setting.setLayoutAlgorithm(LayoutAlgorithm.SINGLE_COLUMN);
//
//              myWebView2.setWebViewClient(new WebViewClient(){  
//                  @Override  
//                  public boolean shouldOverrideUrlLoading(WebView view, String url) {  
//                      view.loadUrl(url);  
//                      return true;  
//                  }  
//              });  
//              
//              
//              myWebView2.addJavascriptInterface(new Object() {         
//                  public void clickOnAndroid() {         
//                      View mHandler = null;
//					mHandler.post(new Runnable() {         
//                          public void run() {         
//                        	  myWebView2.loadUrl("javascript:wave()");         
//                          }         
//                      });         
//                  }         
//              }, "demo"); 
////			
//              
////            
////            	  WebView mWebFlash = (WebView) findViewById(R.id.WebView);
////            	 WebSettings settings = mWebFlash.getSettings();
////            	 settings.setPluginState(PluginState.ON);
////            	 settings.setJavaScriptEnabled(true);
////            	 settings.setAllowFileAccess(true);
////            	 settings.setDefaultTextEncodingName("GBK");
////            	  mWebFlash.setBackgroundColor(0);
//            	String urlweb="http://"+camera_ip.toString()+":8080/jsfs.html";
////            	  mWebFlash.loadUrl(urlweb);
////            	 
////              
//              
//		
//            	myWebView2.loadUrl(urlweb);
//		    
//		    
//		    
//		    
//		    
		    
		    
		    
		    //通过控件的ID来得到代表控件的对象 
	        styleGroup=(RadioGroup)findViewById(R.id.styleGroup); 
	        s1RadioButton=(RadioButton)findViewById(R.id.s1); 
	        s2RadioButton=(RadioButton)findViewById(R.id.s2); 
	        s3RadioButton=(RadioButton)findViewById(R.id.s3); 
	        
	        handGroup=(RadioGroup)findViewById(R.id.handGroup); 
	        h1RadioButton=(RadioButton)findViewById(R.id.nohand); 
	        h2RadioButton=(RadioButton)findViewById(R.id.hand); 
	        
	        
	        
	        rollBar = (SeekBar) findViewById(R.id.rollBar);
	        pitchBar = (SeekBar) findViewById(R.id.pitchBar);

	        //给RadioGroup设置事件监听 
	        styleGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() { 
	            @Override 
	            public void onCheckedChanged(RadioGroup group, int checkedId) { 
	                // TODO Auto-generated method stub 
	                if(checkedId==s1RadioButton.getId()){ 
	                  style_choice=1;
	                }else if(checkedId==s2RadioButton.getId()){ 
	                	 style_choice=2;
	                } else if(checkedId==s3RadioButton.getId()){ 
	                	 style_choice=3;
	                } 
	            } 
	        }); 
	        
	        handGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() { 
	            @Override 
	            public void onCheckedChanged(RadioGroup group, int checkedId) { 
	                // TODO Auto-generated method stub 
	                if(checkedId==h1RadioButton.getId()){ 
	                	hand_choice=1;
	                }else if(checkedId==h2RadioButton.getId()){ 
	                	hand_choice=2;
	                } 
	            } 
	        }); 
	        
	        
	        
        		findViewById(R.id.webView2).setVisibility(View.INVISIBLE);

		    
		    
		    
		    
		    
		    
		    
		    
		    
		    
		    
		findViewById(R.id.open).setOnClickListener(listener);
		findViewById(R.id.comfrim).setOnClickListener(listener);
		findViewById(R.id.reset).setOnClickListener(listener);
		
		findViewById(R.id.compass).setOnClickListener(listener);

		findViewById(R.id.start).setOnClickListener(listener);
		
		findViewById(R.id.next).setOnClickListener(listener);
		findViewById(R.id.pattern1).setOnClickListener(listener);
		findViewById(R.id.pattern2).setOnClickListener(listener);
		findViewById(R.id.pattern3).setOnClickListener(listener);
		findViewById(R.id.pattern4).setOnClickListener(listener);
		
		findViewById(R.id.single).setOnClickListener(listener);
		findViewById(R.id.ddouble).setOnClickListener(listener);
		findViewById(R.id.multiple).setOnClickListener(listener);
		findViewById(R.id.video).setOnClickListener(listener);


		findViewById(R.id.pdiy).setOnClickListener(listener);

		findViewById(R.id.mback).setOnClickListener(listener);

		findViewById(R.id.pattern5).setOnClickListener(listener);
		findViewById(R.id.pattern6).setOnClickListener(listener);
		findViewById(R.id.pattern7).setOnClickListener(listener);
		findViewById(R.id.pattern8).setOnClickListener(listener);
		findViewById(R.id.pattern9).setOnClickListener(listener);
		findViewById(R.id.pattern10).setOnClickListener(listener);
		
//		findViewById(R.id.single).setOnClickListener(listener);
	
		switch(state)
		{
		case STATE_NOT_CONNECTED:
			findViewById(R.id.comfrim).setVisibility(View.INVISIBLE);
			findViewById(R.id.start).setVisibility(View.INVISIBLE);
			findViewById(R.id.compass).setVisibility(View.INVISIBLE);
			findViewById(R.id.next).setVisibility(View.INVISIBLE);

			findViewById(R.id.pattern1).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern2).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern3).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern4).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern5).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern6).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern7).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern8).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern9).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern10).setVisibility(View.INVISIBLE);
			findViewById(R.id.pdiy).setVisibility(View.INVISIBLE);
			findViewById(R.id.mback).setVisibility(View.INVISIBLE);

			findViewById(R.id.single).setVisibility(View.INVISIBLE);
			findViewById(R.id.ddouble).setVisibility(View.INVISIBLE);
			findViewById(R.id.multiple).setVisibility(View.INVISIBLE);
			findViewById(R.id.video).setVisibility(View.INVISIBLE);

			
			findViewById(R.id.styleGroup).setVisibility(View.INVISIBLE);
			findViewById(R.id.s1).setVisibility(View.INVISIBLE);
			findViewById(R.id.s2).setVisibility(View.INVISIBLE);
			findViewById(R.id.s3).setVisibility(View.INVISIBLE);

			findViewById(R.id.roll).setVisibility(View.INVISIBLE);
			findViewById(R.id.rollBar).setVisibility(View.INVISIBLE);
			findViewById(R.id.pitch).setVisibility(View.INVISIBLE);
			findViewById(R.id.pitchBar).setVisibility(View.INVISIBLE);
	        
	        
			break;
		
		case STATE_CONNECTED:
			findViewById(R.id.comfrim).setVisibility(View.INVISIBLE);
			
			break;
			
		case STATE_COMPASS:
			findViewById(R.id.compass).setVisibility(View.VISIBLE);
			findViewById(R.id.comfrim).setVisibility(View.INVISIBLE);
			findViewById(R.id.start).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.pattern1).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern2).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern3).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern4).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern5).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern6).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern7).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern8).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern9).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern10).setVisibility(View.INVISIBLE);
			findViewById(R.id.pdiy).setVisibility(View.INVISIBLE);
			findViewById(R.id.mback).setVisibility(View.INVISIBLE);

			findViewById(R.id.single).setVisibility(View.INVISIBLE);
			findViewById(R.id.ddouble).setVisibility(View.INVISIBLE);
			findViewById(R.id.multiple).setVisibility(View.INVISIBLE);
			findViewById(R.id.video).setVisibility(View.INVISIBLE);

			findViewById(R.id.open).setVisibility(View.INVISIBLE);
			findViewById(R.id.reset).setVisibility(View.VISIBLE);
			findViewById(R.id.next).setVisibility(View.INVISIBLE);
			break;
			
		case STATE_MODE:
			myView.setSeat(0, 0, 0, 0);

			findViewById(R.id.compass).setVisibility(View.INVISIBLE);

			findViewById(R.id.single).setVisibility(View.VISIBLE);
			findViewById(R.id.ddouble).setVisibility(View.VISIBLE);
			findViewById(R.id.multiple).setVisibility(View.VISIBLE);
			findViewById(R.id.video).setVisibility(View.VISIBLE);


			findViewById(R.id.open).setVisibility(View.INVISIBLE);
			findViewById(R.id.comfrim).setVisibility(View.INVISIBLE);
			findViewById(R.id.start).setVisibility(View.INVISIBLE);
			
			
			findViewById(R.id.mback).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern1).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern2).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern3).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern4).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern5).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern6).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern7).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern8).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern9).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern10).setVisibility(View.INVISIBLE);
			findViewById(R.id.pdiy).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.styleGroup).setVisibility(View.VISIBLE);
			findViewById(R.id.s1).setVisibility(View.VISIBLE);
			findViewById(R.id.s2).setVisibility(View.VISIBLE);
			findViewById(R.id.s3).setVisibility(View.VISIBLE);
			
			findViewById(R.id.roll).setVisibility(View.VISIBLE);
			findViewById(R.id.rollBar).setVisibility(View.VISIBLE);
			findViewById(R.id.pitch).setVisibility(View.VISIBLE);
			findViewById(R.id.pitchBar).setVisibility(View.VISIBLE);
			
			break;
			
		case STATE_SINGLE:
			findViewById(R.id.single).setVisibility(View.INVISIBLE);
			findViewById(R.id.ddouble).setVisibility(View.INVISIBLE);
			findViewById(R.id.multiple).setVisibility(View.INVISIBLE);
			findViewById(R.id.video).setVisibility(View.INVISIBLE);

			
			findViewById(R.id.mback).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern1).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern2).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern3).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern4).setVisibility(View.VISIBLE);
			findViewById(R.id.pdiy).setVisibility(View.VISIBLE);

			
			findViewById(R.id.pattern5).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern6).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern7).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.pattern8).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern9).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern10).setVisibility(View.INVISIBLE);
			
			break;
			
		case STATE_DOUBLE:
			findViewById(R.id.single).setVisibility(View.INVISIBLE);
			findViewById(R.id.ddouble).setVisibility(View.INVISIBLE);
			findViewById(R.id.multiple).setVisibility(View.INVISIBLE);
			findViewById(R.id.video).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.mback).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern1).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern2).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern3).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern4).setVisibility(View.INVISIBLE);
			findViewById(R.id.pdiy).setVisibility(View.INVISIBLE);

			
			findViewById(R.id.pattern5).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern6).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern7).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.pattern8).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern9).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern10).setVisibility(View.INVISIBLE);
			
			break;
			
		case STATE_MULTIPLE:
			findViewById(R.id.single).setVisibility(View.INVISIBLE);
			findViewById(R.id.ddouble).setVisibility(View.INVISIBLE);
			findViewById(R.id.multiple).setVisibility(View.INVISIBLE);
			findViewById(R.id.video).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.mback).setVisibility(View.VISIBLE);
			
			findViewById(R.id.pattern1).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern2).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern3).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern4).setVisibility(View.INVISIBLE);
			findViewById(R.id.pdiy).setVisibility(View.INVISIBLE);

			
			findViewById(R.id.pattern5).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern6).setVisibility(View.VISIBLE);
			findViewById(R.id.pattern7).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.pattern8).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern9).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern10).setVisibility(View.INVISIBLE);
			
			break;
			
		case STATE_VIDEO:
			findViewById(R.id.video).setVisibility(View.INVISIBLE);
		
			break;
			
		case STATE_START:
			findViewById(R.id.start).setVisibility(View.VISIBLE);
			
	
			
			break;
			
		case STATE_WAIT_CONFIRM:
			findViewById(R.id.single).setVisibility(View.INVISIBLE);
			findViewById(R.id.ddouble).setVisibility(View.INVISIBLE);
			findViewById(R.id.multiple).setVisibility(View.INVISIBLE);
			findViewById(R.id.video).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.mback).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.pattern1).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern2).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern3).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern4).setVisibility(View.INVISIBLE);
			findViewById(R.id.pdiy).setVisibility(View.INVISIBLE);

			
			findViewById(R.id.pattern5).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern6).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern7).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.pattern8).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern9).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern10).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.styleGroup).setVisibility(View.INVISIBLE);
			findViewById(R.id.s1).setVisibility(View.INVISIBLE);
			findViewById(R.id.s2).setVisibility(View.INVISIBLE);
			findViewById(R.id.s3).setVisibility(View.INVISIBLE);

			findViewById(R.id.roll).setVisibility(View.INVISIBLE);
			findViewById(R.id.rollBar).setVisibility(View.INVISIBLE);
			findViewById(R.id.pitch).setVisibility(View.INVISIBLE);
			findViewById(R.id.pitchBar).setVisibility(View.INVISIBLE);
			
			break;
			
		case STATE_CONFIRM:
			findViewById(R.id.open).setVisibility(View.INVISIBLE);
			findViewById(R.id.comfrim).setVisibility(View.VISIBLE);
			findViewById(R.id.start).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern2).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern1).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern3).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern4).setVisibility(View.INVISIBLE);
			findViewById(R.id.mback).setVisibility(View.INVISIBLE);

			findViewById(R.id.pdiy).setVisibility(View.INVISIBLE);

			
			findViewById(R.id.pattern5).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern6).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern7).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.pattern8).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern9).setVisibility(View.INVISIBLE);
			findViewById(R.id.pattern10).setVisibility(View.INVISIBLE);
			
			findViewById(R.id.styleGroup).setVisibility(View.INVISIBLE);
			findViewById(R.id.s1).setVisibility(View.INVISIBLE);
			findViewById(R.id.s2).setVisibility(View.INVISIBLE);
			findViewById(R.id.s3).setVisibility(View.INVISIBLE);

			findViewById(R.id.roll).setVisibility(View.INVISIBLE);
			findViewById(R.id.rollBar).setVisibility(View.INVISIBLE);
			findViewById(R.id.pitch).setVisibility(View.INVISIBLE);
			findViewById(R.id.pitchBar).setVisibility(View.INVISIBLE);
			break;
			
		case STATE_FINISH:
			
			break;
		}
				
	
		
	
		
	
	}
	
	
	private void sendview()
	{
		viewcount=2;
		findViewById(R.id.Back).setOnClickListener(listener);
		
		findViewById(R.id.Up).setOnClickListener(listener);
		findViewById(R.id.Down).setOnClickListener(listener);
		findViewById(R.id.Left).setOnClickListener(listener);
		findViewById(R.id.Right).setOnClickListener(listener);
		
		findViewById(R.id.cUp).setOnClickListener(listener);
		findViewById(R.id.cDown).setOnClickListener(listener);
		findViewById(R.id.cLeft).setOnClickListener(listener);
		findViewById(R.id.cRight).setOnClickListener(listener);
		
		findViewById(R.id.liftUp).setOnClickListener(listener);
		findViewById(R.id.liftDown).setOnClickListener(listener);
	}
	
	
	
	
	
	
	public static Bitmap getHttpBitmap(String url){
    	URL myFileURL;
    	Bitmap bitmap=null;
    	try{
    		myFileURL = new URL(url);
    		//获得连接
    		HttpURLConnection conn=(HttpURLConnection)myFileURL.openConnection();
    		//设置超时时间为6000毫秒，conn.setConnectionTiem(0);表示没有时间限制
    		conn.setConnectTimeout(6000);
    		//连接设置获得数据流
    		conn.setDoInput(true);
    		//不使用缓存
    		conn.setUseCaches(false);
    		//这句可有可无，没有影响
    		//conn.connect();
    		//得到数据流
    		InputStream is = conn.getInputStream();
    		//解析得到图片
    		bitmap = BitmapFactory.decodeStream(is);
    		//关闭数据流
    		is.close();
    	}catch(Exception e){
    		e.printStackTrace();
    	}
    	
		return bitmap;
    	
    }
	
	
	private void getMessage() {  
		
        if (cs == null)  
            return;  
        DataInputStream inputStream = null;  
        inputStream = cs.getMessageStream();  
        try {  
           
            int bufferSize = 8192;  
            byte[] buf = new byte[bufferSize];  
            long passedlen = 0;  
            long len = 0;  
              
            Log.v("Client","savePath"+savePath);  
            DataOutputStream fileOut = new DataOutputStream(  
                    new BufferedOutputStream(new BufferedOutputStream(  
                            new FileOutputStream(savePath))));  
//            len = inputStream.readLong();  
            Log.v("Client","filesize:"+len);  
            Log.v("Client","start");  
            while(true) {  
                int read = 0;  
                if (inputStream != null) {  
                    read = inputStream.read(buf);  
                }
//                else
//                {
//                 break;
//                }
                passedlen += read;  
                if (read == -1) {  
                    break;  
                }  
//                Log.d("AndroidClient","文件接收了"+(passedlen*100/len)+"%/n");  
                fileOut.write(buf,0,read);  
            }  
            Log.v("AndroidClient","finish"+savePath);  
            fileOut.close();  
        } catch (IOException e) {  
            // TODO Auto-generated catch block  
            e.printStackTrace();  
        }  
    }  
	
   
    
    private boolean createConnection() {  
    	Log.v("before", "before");  
        cs = new ClientSocket(board_ip, file_transfer_port);  
        Log.v("new", "new"); 
        
        cs.createConnection();  
        
        Log.v("after", "after");  
        return true;  
    }  
	
	private ISocketResponse socketListener=new ISocketResponse() {
		
		@Override
		public void onSocketResponse(final String txt) {
		runOnUiThread(new Runnable() {
				public void run() {
				
					Log.v("equals",txt);
					if(txt.equals("cr"))
					{
						status.setText("connected");
//						state=STATE_CONNECTED;
						state=STATE_MODE;
//						initView();
						connected=1;
						initView();
						
					
					}
					
					else if(txt.equals("sm"))
					{
						status.setText("start");
						
						packet.pack("0"+style_choice);
						user.send(packet);
						
						packet.pack("0"+hand_choice);
						user.send(packet);
						
						
						if(roll_newProgress<10)
						{
							packet.pack("0"+roll_newProgress);
							user.send(packet);
						}
						else 
						{
							packet.pack(""+roll_newProgress);
							user.send(packet);
						}
						
						if(pitch_newProgress<10)
						{
							packet.pack("0"+pitch_newProgress);
							user.send(packet);
						}
						else 
						{
							packet.pack(""+pitch_newProgress);
							user.send(packet);
						}
						
						state=STATE_WAIT_CONFIRM;
						initView();
		        		findViewById(R.id.webView2).setVisibility(View.INVISIBLE);
		        		findViewById(R.id.comfrim).setVisibility(View.INVISIBLE);
					}
					
					//fetch degree notice
					else if(txt.equals("fd"))
					{
						
						state=STATE_COMPASS;
						initView();
						
					}
					
					else if(txt.equals("fc"))
					{
//						calculateOrientation();
						if(degree<10)
						{
							packet.pack("00"+degree);
							user.send(packet);
						}
						else if(degree<100)
						{
							packet.pack("0"+degree);
							user.send(packet);
						}
						else
						{
							packet.pack(""+degree);
							user.send(packet);
						}
					}
					
					else if(txt.equals("cp"))
					{
						status.setText("confrim");
						Log.v("player","play");
//						player.play(soundMap.get(1), 1, 1, 10,0, 1);
						state=STATE_WAIT_FINISH;
						initView();
					}
					
					else if(txt.equals("p1"))
					{
						status.setText("pattern 1");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.right_far);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 
						

					}
					
					else if(txt.equals("p2"))
					{
						status.setText("pattern 2");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.left_far);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 
					}
					
					else if(txt.equals("p3"))
					{
						status.setText("pattern 3");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.cross_line);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					else if(txt.equals("p4"))
					{
						status.setText("pattern 4");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.left_near);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					else if(txt.equals("pd"))
					{
						status.setText("pdiy");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.diy);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					else if(txt.equals("p5"))
					{
						status.setText("pattern 5");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.triple_near);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					
					else if(txt.equals("p6"))
					{
						status.setText("pattern 6");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.triple_far);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					else if(txt.equals("p7"))
					{
						status.setText("pattern 7");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.pattern4);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					else if(txt.equals("p8"))
					{
						status.setText("pattern 8");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.double_near);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					else if(txt.equals("p9"))
					{
						status.setText("pattern 9");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.double_far);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					else if(txt.equals("pa"))
					{
						status.setText("pattern 10");
						state=STATE_START;
						initView();
						 Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.pattern4);
						 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70)); 

					}
					
					/*
					else if(txt.equals("Up"))
					{
						status.setText("car forward");
					}
					
					else if(txt.equals("Do"))
					{
						status.setText("car backward");
					}
					
					else if(txt.equals("Le"))
					{
						status.setText("car left");
					}
					
					else if(txt.equals("Ri"))
					{
						status.setText("car right");
					}
					
					
					else if(txt.equals("cU"))
					{
						status.setText("camera forward");
					}
					
					else if(txt.equals("cD"))
					{
						status.setText("camera backwrd");
					}
					
					else if(txt.equals("cL"))
					{
						status.setText("camera left");
					}
					
					else if(txt.equals("cR"))
					{
						status.setText("camera right");
					}
					
					else if(txt.equals("lu"))
					{
						status.setText("lift left");
					}
					
					else if(txt.equals("ld"))
					{
						status.setText("lift right");
					}
					*/
					else if(txt.equals("nn"))
					{
					state=STATE_CONFIRM;
					initView();

					player.play(soundMap.get(1), 1, 1, 10,0, 1);
					
					}
					
					else if(txt.equals("nx"))
					{
					state=STATE_CONFIRM;
					initView();

					player.play(soundMap.get(2), 1, 1, 10,0, 1);
					
					}
					
					else if(txt.equals("fa"))
					{
					
						if(viewcount==1)
						{
							status.setText("finished_ack");
							
							state=STATE_FINISH;
							initView();
							
							findViewById(R.id.webView2).setVisibility(View.INVISIBLE);
							
//							
//							try {
//								Thread.currentThread();
//								Thread.sleep(2000);
//							} catch (InterruptedException e) {
//								// TODO Auto-generated catch block
//								e.printStackTrace();
//							}
					
					
//							SystemClock.sleep(1);
							
//							if(file_transfer_connected==1)
//							{	
								createConnection();
//							file_transfer_connected=0;
//							}
							
							
							
							 
						            getMessage();  
						            file_transfer_port++;
							  
							  
							Bitmap bitmap = getLoacalBitmap(savePath);
								    
//								    SimpleDateFormat dateformat=new SimpleDateFormat("yyyy-MM-dd_HH-mm");
//							
//								    Log.v("url",dateformat.format(new Date()).toString());
//								  String urlStr  =  "http://"+camera_ip.toString()+":8080/v/photo/photo_"+dateformat.format(new Date()).toString()+".jpg";
//				//				  System.out.println(urlStr); 
//								  //	"http://192.168.43.1:8080/photoaf.jpg";
//								  
//								    
//				//				    String urlStr  ="http://"+camera_ip.toString()+":8080/photoaf.jpg";
//								    Log.v("url",urlStr);
//								 Bitmap bitmap = getHttpBitmap(urlStr);
//								 
//								 if(bitmap==null)
//								 {
//									 urlStr  ="http://"+camera_ip.toString()+":8080/photoaf.jpg";
//									 bitmap = getHttpBitmap(urlStr);
//								 }
								 
							 imageView.setImageBitmap(getRoundCornerImage(bitmap, 70));  
								
								player.play(soundMap.get(3), 1, 1, 10,0, 1);

									 
//									 SystemClock.sleep(4);		 
				//					 saveBitmap(bitmap);
									 
							}
				
						
						
					}
				}
			});
		}
	};
	
	private OnClickListener listener=new OnClickListener() {
		
		@SuppressWarnings("deprecation")
		@Override
		public void onClick(View v) {
			switch(v.getId())
			{
			
				case R.id.open:
					if(connected==0)
					{
					user.open(board_ip, 60000);
					}
					else
					{
						connected=0;
						user.close();
						user.open(board_ip, 60000);
					}
					packet.pack("cr");
					user.send(packet);
					
				
					
					
					break;
		
				case R.id.start:	
					packet.pack("sm");
					user.send(packet);
	        		findViewById(R.id.start).setVisibility(View.INVISIBLE);

					
					break;
					
				
				case R.id.compass:	
//					calculateOrientation();
					if(degree<10)
					{
						packet.pack("00"+degree);
						user.send(packet);
					}
					else if(degree<100)
					{
						packet.pack("0"+degree);
						user.send(packet);
					}
					else
					{
						packet.pack(""+degree);
						user.send(packet);
					}
					
					state=STATE_MODE;
					initView();
					break;

				case R.id.reset:	
					   recreate(); 
					break;
					
				case R.id.comfrim:	
					
			//		play(int soundID, float leftVolume, float rightVolume, int priority, int loop, float rate) ,其中leftVolume和rightVolume表示左右音量，priority表示优先级,loop表示循环次数,rate表示速率，
//					player.play(soundMap.get(1), 1, 1, 10,0, 1);
					packet.pack("cp");
					user.send(packet);
	        		findViewById(R.id.comfrim).setVisibility(View.INVISIBLE);

					break;
					
				case R.id.next:	
					setContentView(R.layout.second);
					sendview();
					  myWebView = (WebView) findViewById(R.id.WebView);
					
					WebSettings setting=myWebView.getSettings();
					
	                  setting.setJavaScriptEnabled(true);     
	                  setting.setPluginState(PluginState.ON);
	                  setting.setAllowFileAccess(true);
	                  setting.setSupportZoom(true); //可以缩放
	                  setting.setBuiltInZoomControls(true); //显示放大缩小 controler                //设置出现缩放工具
	                  
	                  setting.setLayoutAlgorithm(LayoutAlgorithm.SINGLE_COLUMN);

	                  myWebView.setWebViewClient(new WebViewClient(){  
	                      @Override  
	                      public boolean shouldOverrideUrlLoading(WebView view, String url) {  
	                          view.loadUrl(url);  
	                          return true;  
	                      }  
	                  });  
	                  
	                  
	                  myWebView.addJavascriptInterface(new Object() {         
	                      public void clickOnAndroid() {         
	                          View mHandler = null;
							mHandler.post(new Runnable() {         
	                              public void run() {         
	                            	  myWebView.loadUrl("javascript:wave()");         
	                              }         
	                          });         
	                      }         
	                  }, "demo"); 
//					
	                  
//	                
//	                	  WebView mWebFlash = (WebView) findViewById(R.id.WebView);
//	                	 WebSettings settings = mWebFlash.getSettings();
//	                	 settings.setPluginState(PluginState.ON);
//	                	 settings.setJavaScriptEnabled(true);
//	                	 settings.setAllowFileAccess(true);
//	                	 settings.setDefaultTextEncodingName("GBK");
//	                	  mWebFlash.setBackgroundColor(0);
	                	String urlweb="http://"+camera_ip.toString()+":8080/jsfs.html";
//	                	  mWebFlash.loadUrl(urlweb);
//	                	 
//	                  
	                  
				
					myWebView.loadUrl(urlweb);
					break;
					
					
				case R.id.Back:	
					
					setContentView(R.layout.activity_main);
					initView();
					break;
					
					
				case R.id.Up:
					
					packet.pack("Up");
					
					user.send(packet);

					break;
					
					
					
				case R.id.Left:
					
					
					packet.pack("Le");
					
					user.send(packet);
					break;
					
				case R.id.Right:
				
				packet.pack("Ri");
					
					user.send(packet);
					break;
					
				case R.id.Down:
				packet.pack("Do");
					
					user.send(packet);
					break;
					
				case R.id.cUp:
					packet.pack("cU");
					
					user.send(packet);
					break;
					
					
					
				case R.id.cLeft:
					
						packet.pack("cL");
				user.send(packet);
					break;
					
				case R.id.cRight:
					packet.pack("cR");
				
					user.send(packet);
					break;
					
				case R.id.cDown:
					
				
				packet.pack("cD");
					
					user.send(packet);
					break;
					
					
				case R.id.liftUp:	
				
				packet.pack("lu");
					
					user.send(packet);
					break;
					
					
				case R.id.liftDown:	
					packet.pack("ld");
					
					user.send(packet);
			
					break;
					
					
				case R.id.pattern1:	
					
			packet.pack("p1");
					
					user.send(packet);
					break;
					
					
				case R.id.pattern2:	
					
				packet.pack("p2");
					user.send(packet);
					break;

					
				case R.id.pattern3:	
					
					packet.pack("p3");
							
							user.send(packet);
							break;
							
							
						case R.id.pattern4:	
							
						packet.pack("p4");
							user.send(packet);
							break;
							
							
							
						case R.id.pattern5:	
							
						packet.pack("p5");
							user.send(packet);
							break;
							
						case R.id.pattern6:	
							
							packet.pack("p6");
								user.send(packet);
								break;
								
						case R.id.pdiy:	
							if(myView.isSign()){
								
							
								 
								myView.setSeat(0, 0, 0, 0);
								myView.setSign(false);
								button.setText("Stop");
								
								
							}else{
								myView.setSign(true);
								button.setText("DIY");
								
								myView.setSeat(x, y, x+(ratiowidth*image_width/640), y+(ratiowidth*image_width/640));
								myView.postInvalidate();
								
								packet.pack("pd");
								user.send(packet);
								
								if(ratiox<10)
								{
									packet.pack("00"+ratiox);
									user.send(packet);
								}
								else if(ratiox<100)
								{
									packet.pack("0"+ratiox);
									user.send(packet);
								}
								else
								{
									packet.pack(""+ratiox);
									user.send(packet);
								}
								
								
								if(ratioy<10)
								{
									packet.pack("00"+ratioy);
									user.send(packet);
								}
								else if(ratioy<100)
								{
									packet.pack("0"+ratioy);
									user.send(packet);
								}
								else
								{
									packet.pack(""+ratioy);
									user.send(packet);
								}
								
								
								
								if(ratiowidth<10)
								{
									packet.pack("00"+ratiowidth);
									user.send(packet);
								}
								else if(ratiowidth<100)
								{
									packet.pack("0"+ratiowidth);
									user.send(packet);
								}
								else
								{
									packet.pack(""+ratiowidth);
									user.send(packet);
								}
								
								
								
								if(ratioheight<10)
								{
									packet.pack("00"+ratioheight);
									user.send(packet);
								}
								else if(ratioheight<100)
								{
									packet.pack("0"+ratioheight);
									user.send(packet);
								}
								else
								{
									packet.pack(""+ratioheight);
									user.send(packet);
								}
								
								
								
							}
							myView.postInvalidate();
						
//							packet.pack("pdiy-000-000-000-000");
//								user.send(packet);
								break;
								
								
						case R.id.pattern7:	
							
							packet.pack("p7");
								user.send(packet);
								break;
								
						case R.id.pattern8:	
							
							packet.pack("p8");
								user.send(packet);
								break;
								
						case R.id.pattern9:	
							
							packet.pack("p9");
								user.send(packet);
								break;
								
						case R.id.pattern10:	
							
							packet.pack("p10");
								user.send(packet);
								break;
								
						case R.id.mback:	
							state=STATE_MODE;
							initView();
							
								break;
								
						case R.id.single:	
							state=STATE_SINGLE;
							initView();
							
								break;
					
						case R.id.ddouble:	
							state=STATE_DOUBLE;
							initView();
							
								break;
								
						case R.id.multiple:	
							state=STATE_MULTIPLE;
							initView();
							
								break;
								
						case R.id.video:	
//							state=STATE_VIDEO;
							state=STATE_START;
							packet.pack("vi");
							user.send(packet);
							initView();
							
								break;
							
			}
		}
	};
	

	
	
	   public static Bitmap getLoacalBitmap(String url) {
	         try {
	              FileInputStream fis = new FileInputStream(url);
	              return BitmapFactory.decodeStream(fis);  ///把流转化为Bitmap图片        

	           } catch (FileNotFoundException e) {
	              e.printStackTrace();
	              return null;
	         }
	    }
	public void saveBitmap( Bitmap bitmap) {
		String picName="pic.JPG";
		  File f = new File("/sdcard/DCIM/intelcup/", picName);
		  if (f.exists()) {
		   f.delete();
		  }
		  try {
		   FileOutputStream out = new FileOutputStream(f);
		   bitmap.compress(Bitmap.CompressFormat.PNG, 90, out);
		   out.flush();
		   out.close();
	
		  } catch (FileNotFoundException e) {
		   // TODO Auto-generated catch block
		   e.printStackTrace();
		  } catch (IOException e) {
		   // TODO Auto-generated catch block
		   e.printStackTrace();
		  }

		 }

	
	
	 public boolean onKeyDown(int keyCode, KeyEvent event) {
         // TODO Auto-generated method stub
         if (((keyCode == KeyEvent.KEYCODE_BACK) ||
(keyCode == KeyEvent.KEYCODE_HOME))
&& event.getRepeatCount() == 0) {
                dialog_Exit(MainActivity.this);
         }
         return false;
        
         //end onKeyDown
  }

  public static void dialog_Exit(Context context) {
   AlertDialog.Builder builder = new Builder(context);
   builder.setMessage("Leaving the app?");
   builder.setTitle("Reminder");
   builder.setIcon(android.R.drawable.ic_dialog_alert);
   builder.setPositiveButton("Confirm",
           new DialogInterface.OnClickListener() {
               public void onClick(DialogInterface dialog, int which) {
                   dialog.dismiss();
                   android.os.Process.killProcess(android.os.Process
                           .myPid());
               }
           });
  
   builder.setNegativeButton("Cancel",
           new android.content.DialogInterface.OnClickListener() {
               public void onClick(DialogInterface dialog, int which) {
                   dialog.dismiss();
               }
           });
  
   builder.create().show();
}
  private final class MySensorEventListener implements  SensorEventListener{

  	@Override
		public void onSensorChanged(SensorEvent event) {
			if(event.sensor.getType()==Sensor.TYPE_ORIENTATION){
				float x = event.values[SensorManager.DATA_X];
//				float y = event.values[SensorManager.DATA_Y];
//				float z = event.values[SensorManager.DATA_Z];
				degree=(int) x;
				degree+=90;
				if(degree>360)
					degree-=360;
				
				status.setText(degree+"'");

			}
			else{
				
//				float x = event.values[SensorManager.DATA_X];
//				float y = event.values[SensorManager.DATA_Y];
//				float z = event.values[SensorManager.DATA_Z];
//				degree=(int) x;
////				degree+=90;
//				if(degree>360)
//					degree-=360;
//				
//				status.setText(degree+"'");

			}
			
		}

		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
			// TODO Auto-generated method stub
			
		}
  	
  }
  
  @Override
	protected void onResume() {
  	Sensor sensor_orientation=sensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION);
  	sensorManager.registerListener(mySensorEventListener,sensor_orientation, SensorManager.SENSOR_DELAY_UI);
  	
  	Sensor sensor_accelerometer=sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
  	sensorManager.registerListener(mySensorEventListener,sensor_accelerometer, SensorManager.SENSOR_DELAY_UI);
		super.onResume();
	}

  @Override
	protected void onPause() {
  	sensorManager.unregisterListener(mySensorEventListener);
		super.onPause();
	}
  
  
}
