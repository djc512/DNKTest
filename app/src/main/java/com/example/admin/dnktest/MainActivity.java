package com.example.admin.dnktest;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }

    public String str;
    public int value;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText("数组里面的和为：" + getSum());
        showElement();
        str = "abc";
        value = 1;
        Log.i("DJC", "bean的值为：" + str + "  value的值：" + value);
        getModifyString();
        Log.i("DJC", "经过本地代码修改过的bean的值为：" + str + " value的值：" + value);
        findMethod();

        TextView tvAdd = (TextView) findViewById(R.id.tv);
        tvAdd.setText("JNI里面求和：" + add(3, 4));
        String appendStr = getAppendStr("I am one");
        Log.i("DJC", "拼接后的字符串：" + appendStr);
        long init = init();
        Log.i("DJC", "初始化的结果为：" + init);

        TestBean bean = new TestBean();
        setData(bean);
        Log.i("DJC", "age的值：" + bean.getAge());
        Log.i("DJC", "name的值：" + bean.getName());

        Log.i("DJC", "age的值：" + bean.getFamily().getFamilyAge());
        Log.i("DJC", "name的值：" + bean.getFamily().getFamilyName());

        long destory = destory();
        Log.i("DJC", "释放内存的结果为：" + destory);
    }

    public native String stringFromJNI();

    //helloworld
    public native String helloWorld();

    //java层传递参数，jni层返回
    public native String getString(String str);

    //调用GetIntArrayRegion()
    //通过将基本数组复制到C的缓存区，然后本地代码操作缓存区的元素
    public native int sumArray(int[] arr);

    //GetIntArrayElements
    //获取本地代码指向基本类型数组的指针的方式操作元素
    public native int sumArrayOther(int[] arr);

    public int getSum() {
        int[] arrInt = new int[10];
        for (int i = 0; i < arrInt.length; i++) {
            arrInt[i] = i;
        }
        Log.i("DJC", Arrays.toString(arrInt));
        return sumArrayOther(arrInt);
    }

    //通过本地代码创建一个二维数组，并且打印出来元素
    //无法直接创建一个二维数组，所以创建一个元素类型为整形数组的一维数组
    public native int[][] getArray(int size);

    public void showElement() {
        int[][] arr = getArray(3);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                Log.i("DJC", "创建的数组元素为：" + arr[i][j]);
            }
        }
    }

    //获取属性ID,在本地代码中修改java层变量的数值
    public native void getModifyString();

    //在本地代码中访问java层方法
    public native void findMethod();

    private void printLog() {
        Log.i("DJC", "我是在底层进行调用的");
    }

    //求和
    public native int add(int a, int b);

    //在本地代码中拼接字符创并返回
    public native String getAppendStr(String str);

    //创建一个全局的引用，并获取属性ID，方法ID
    public native long init();

    //通过本地代码给对象赋值
    public native void setData(TestBean bean);

    //对象销毁，释放内存
    public native long destory();
}
