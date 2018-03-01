package com.example.admin.dnktest;

import android.util.Log;

/**
 * Created by Administrator on 2018/3/1.
 */

public class JniMethod {

    //可以通过javap -s 全类名获取类里面所有方法的标识符

    private int addSum(int a, int b) {
        int sum = a + b;
        Log.i("DJC", "callAddSum = " + sum);
        return sum;
    }

    //jni调用java方法
    public native void callAddSum();

    //调用MainActivity中showToast方法
    public native void getShowToast(MainActivity activity);
}
