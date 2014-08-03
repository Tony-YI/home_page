package com.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.view.View;

public class MyView extends View {
    private int x;
    private int y;
    private int m;
    private int n;
    private boolean sign;//�滭���λ
    private Paint paint;//����
	public MyView(Context context) {
		super(context);
		paint = new Paint(Paint.FILTER_BITMAP_FLAG);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(sign){
			paint.setColor(Color.TRANSPARENT);
			
		}else{
			paint.setColor(Color.BLUE);
			paint.setAlpha(80);
			canvas.drawRect(new Rect(x, y, m, n), paint);
		}
		super.onDraw(canvas);
	}
    
	public void setSeat(int x,int y,int m,int n){
		this.x = x;
		this.y = y;
		this.m = m;
		this.n = n;
	}

	public boolean isSign() {
		return sign;
	}

	public void setSign(boolean sign) {
		this.sign = sign;
	}
    	
}
