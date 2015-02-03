package org.cocos2dx.cpp;

import android.util.Log;

public class TestJni
{
	public static void func1()
	{
		Log.e("xiaota","java:func1,called succeed!");
	}
	public static int func2()
	{
		return 3838438;
	}
	public static String func3(int i)
	{
		String str = "get int value:"+i;
		Log.e("xiaota",str);
		return str;
	}
	public static String func4(String str)
	{
		Log.e("xiaota",str);
		return str;
	}
	public static int func5(int a,int b)
	{
		int c = a+b;
		Log.e("xiaota","func5");
		return c;
	}
}