/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.cpp;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;

public class AppActivity extends Cocos2dxActivity {
	private static AlertDialog mDialog = null;
	private static Handler mHandler = new Handler(){
		@Override
		public void handleMessage(android.os.Message msg) {
			mDialog.setTitle("提示");
			mDialog.setMessage(msg.obj.toString());
			mDialog.show();
		};
	};
	
	@SuppressWarnings("deprecation")
	@Override
	protected void onCreate(android.os.Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		mDialog = new AlertDialog.Builder(this).create(); 
		mDialog.setButton("确定",
				new AlertDialog.OnClickListener() { 
			@Override
			public void onClick(DialogInterface arg0, int arg1) {
				// TODO Auto-generated method stub
				AppActivity.this.finish();
				System.exit(0);
			} 
		}); 

		mDialog.setButton2("取消",
				new AlertDialog.OnClickListener() { 
			@Override public void onClick(DialogInterface dialog, int which) { 
				mDialog.dismiss();
			}
		}); 
	};

	public static void exitGame() {
		String str= "确定退出五子棋?";
		Message mes = new Message(); 
		mes.obj = str;  
		mHandler.sendMessage(mes); 
	}
}
