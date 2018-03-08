#include <jni.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#include <android/log.h>

#define TAG "DJC_JNI" // 这个是自定义的LOG的标识
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

char *jstringToChar(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

extern "C"
JNIEXPORT jstring
JNICALL
Java_com_example_admin_dnktest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_admin_dnktest_MainActivity_helloWorld(JNIEnv *env, jobject instance) {
    return env->NewStringUTF("HelloWorld");
}

extern "C"
JNIEXPORT jstring
JNICALL
Java_com_example_admin_dnktest_MainActivity_getString(JNIEnv *env, jobject instance, jstring str_) {
    const char *str = env->GetStringUTFChars(str_, 0);

    // 将java层传递的参数，转换成c语言可以识别的UTF格式，传递给底层

    env->ReleaseStringUTFChars(str_, str);//释放内存

    return env->NewStringUTF("传递给底层");
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_admin_dnktest_MainActivity_sumArray(JNIEnv *env, jobject instance,
                                                     jintArray arr_) {

    jint i;
    jint sum = 0;
    jint len = env->GetArrayLength(arr_);
    jint buf[len];
//    GetIntArrayRegion
//    将一个int数组中的所有元素复制到一个C缓冲区中，然后本地代码在C缓冲区中的访问这些元素
    env->GetIntArrayRegion(arr_, 0, 10, buf);
    for (i = 0; i < 10; i++) {
        sum += buf[i];
    }
    return sum;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_admin_dnktest_MainActivity_sumArrayOther(JNIEnv *env, jobject instance,
                                                          jintArray arr_) {
    jint len = env->GetArrayLength(arr_);   //获取数组的长度
    jint *arr = env->GetIntArrayElements(arr_, NULL);   //获取本地代码指向基本数据类型数组的指针（相当于一个数组）

    //防止内存不够用，获取不到
    if (arr == NULL) {
        return 0;
    }
    jint i, sum = 0;
    for (i = 0; i < len; i++) {
        sum += arr[i];
    }
    env->ReleaseIntArrayElements(arr_, arr, 0); //释放所占用的内存
    return sum;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_example_admin_dnktest_MainActivity_getArray(JNIEnv *env, jobject instance, jint size) {

    //获取数据的引用类型
    jclass intArrCls = env->FindClass("[I");
    //创建一个数组对象
    jobjectArray result = env->NewObjectArray(size, intArrCls, NULL);

    //内存不够用抛出异常
    if (result == NULL) {
        return NULL;
    }
    //size  创建数组的长度
    jint i;
    for (i = 0; i < size; i++) {
        //创建一个整形数组（数组对象的元素）
        jintArray intArr = env->NewIntArray(size);
        //内存泄漏抛出异常
        if (intArr == NULL) {
            return NULL;
        }
        jint temp[256];//创建一个足够大的整形数组，作为本地数据缓存
        jint j;
        //给创建的整形数组赋值
        for (int j = 0; j < size; j++) {
            temp[j] = j;
        }
        //将缓冲区的元素复制给创建的整形数组
        env->SetIntArrayRegion(intArr, 0, size, temp);
        //给数组对象赋值
        env->SetObjectArrayElement(result, i, intArr);
        //释放内存
        env->DeleteLocalRef(intArr);
    }
    return result;

    //jn中无法直接创建二维数组，所以创建一个元素类型为int数组的一维数组，具体步骤：先创建一个int数组的引用，然后创建一个数组对象，在创建一个整形的数组，然后整形素组进行赋值
    //赋值的时候对于基本数据类型，如果元素不是很多，可以通过GetArrayIntRegion方法先把数据复制到本地缓存，操作本地缓存操作元素数据
    //对于引用数据类型，则可以使用jobjectArray，GetObjectArrayElement() 进行元素的操作，结束要释放内存
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_admin_dnktest_MainActivity_getModifyString(JNIEnv *env, jobject instance) {

    //通过对象获取类的引用
    jclass cls = env->GetObjectClass(instance);
    //获取变量的属性ID
    jfieldID fidStr = env->GetFieldID(cls, "str", "Ljava/lang/String;");
    jfieldID fidInt = env->GetFieldID(cls, "value", "I");

    if (fidStr == NULL || fidInt == NULL) {
        return;
    }
    env->SetIntField(instance, fidInt, 200);

    //通过fid获取属性的对象
    jstring jstr = (jstring) env->GetObjectField(instance, fidStr);
    //获取UTF的字符串
    const char *str = env->GetStringUTFChars(jstr, NULL);
    __android_log_print(ANDROID_LOG_INFO, "DJC", "jstr = %s", str);
    if (str == NULL) {
        return;
    }
    //释放掉这个字符串
    env->ReleaseStringUTFChars(jstr, str);

    //从新创建一个字符串
    jstr = env->NewStringUTF("123");

    if (jstr == NULL) {
        return;
    }
    //通过属性ID进行赋值
    env->SetObjectField(instance, fidStr, jstr);
    //NewStringUTF()方法创建的字符串是用于java层的使用
    //NewStringUTFChar()创建的字符串是用于jni层C语言使用的
    //访问一个对象字段的流程：先获取这个对象对应的类的引用，然后获取属性ID,通过获取属性ID获取属性值，获取他在本地代码中所转换成的数值，然后将其释放掉，在重新创建，然后通过属性ID赋值
    //对于基本数据类型就不用这么麻烦，获取属性ID直接调用函数进行修改
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_admin_dnktest_MainActivity_findMethod(JNIEnv *env, jobject instance) {

    //通过对象获取类的引用
    jclass cls = env->GetObjectClass(instance);
    //获取方法的ID
    jmethodID methodID = env->GetMethodID(cls, "printLog", "()V");
    if (methodID == NULL) {
        return;
    }
    //通过methodID访问方法
    env->CallVoidMethod(instance, methodID);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_admin_dnktest_MainActivity_add(JNIEnv *env, jobject instance, jint a, jint b) {

    jint sum;
    sum = a + b;
    __android_log_print(ANDROID_LOG_INFO, "DJC", "HELLO");
    __android_log_print(ANDROID_LOG_INFO, "DJC", "sum = %d", sum);
    return sum;

    //jni层输出日志，需要引入头文件（类似包）android/log.h
    //调用函数__android_log_print()
    //第一个参数表示log的类型
    //第二个参数表示Tag标识
    //第三个参数  "sum = %d"  表示打印的参数类型
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_admin_dnktest_MainActivity_getAppendStr(JNIEnv *env, jobject instance,
                                                         jstring str_) {
//    const char *str = env->GetStringUTFChars(str_, 0);
    char *str = jstringToChar(env, str_);
    char *add = "add i am add";
    __android_log_print(ANDROID_LOG_INFO, "DJC", "add = %s", add);
    strcat(str, add);
//    strcat((char *) str, add);
//    env->ReleaseStringUTFChars(str_, str);
    return env->NewStringUTF(str);
}


jclass cls;
jclass globalCls;
jclass familyCls;
jclass globalFamilyCls;

jfieldID nameID;
jfieldID ageID;
jfieldID familyID;
jmethodID methodID;

jfieldID familyNameID;
jfieldID familyAgeID;
jmethodID familyMethodID;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_admin_dnktest_MainActivity_init(JNIEnv *env, jobject instance) {

    cls = env->FindClass("com/example/admin/dnktest/TestBean");
    if (cls == NULL) {
        return 0;
    }
    globalCls = (jclass) env->NewGlobalRef(cls);
    if (globalCls == NULL) {
        return 0;
    }
    //获取ID
    nameID = env->GetFieldID(globalCls, "name", "Ljava/lang/String;");
    ageID = env->GetFieldID(globalCls, "age", "I");
    familyID = env->GetFieldID(globalCls, "family", "Lcom/example/admin/dnktest/Family;");
    methodID = env->GetMethodID(globalCls, "<init>", "()V");

    if (nameID == NULL || ageID == NULL || familyID == NULL || methodID == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "ID获取失败");
        return 0;
    }

    familyCls = env->FindClass("com/example/admin/dnktest/Family");
    if (familyCls == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "类创建失败");
        return 0;
    }
    globalFamilyCls = (jclass) env->NewGlobalRef(familyCls);
    if (globalFamilyCls == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "对象创建失败");
        return 0;
    }

    familyNameID = env->GetFieldID(globalFamilyCls, "familyName", "Ljava/lang/String;");
    familyAgeID = env->GetFieldID(globalFamilyCls, "familyAge", "I");
    familyMethodID = env->GetMethodID(globalFamilyCls, "<init>", "()V");

    if (familyNameID == NULL || familyAgeID == NULL || familyMethodID == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "ID获取失败");
        return 0;
    }

    return 1;
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_admin_dnktest_MainActivity_destory(JNIEnv *env, jobject instance) {

    if (globalCls != NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "释放globalCls内存");
        env->DeleteGlobalRef(globalCls);
    }

    if (globalFamilyCls != NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "释放globalFamilyCls内存");
        env->DeleteGlobalRef(globalFamilyCls);
    }
    return 0;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_admin_dnktest_MainActivity_setData(JNIEnv *env, jobject instance, jobject bean) {
    if (globalCls == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "globalCls为NULL");
        return;
    }

    jstring nameStr = (jstring) env->GetObjectField(bean, nameID);
    if (nameStr == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "nameStr == NULL");
        return;
    }
    const char *nameStr_ = env->GetStringUTFChars(nameStr, NULL);
    if (nameStr_ == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "nameStr_ == NULL");
        return;
    }

    env->ReleaseStringUTFChars(nameStr, nameStr_);

    jstring jstr = env->NewStringUTF("CCCCCC");
    env->SetObjectField(bean, nameID, jstr);
    env->SetIntField(bean, ageID, 25);

    //给TestBean里面的对象赋值
    jobject jobjFamily = env->NewObject(globalFamilyCls, familyMethodID);
    jstring familyNameStr = (jstring) env->GetObjectField(jobjFamily, familyNameID);
    if (familyNameStr == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "familyNameStr == NULL");
        return;
    }
    const char *familyNameStr_ = env->GetStringUTFChars(familyNameStr, NULL);

    if (familyNameStr_ == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "familyNameStr_==NULL");
        return;
    }

    env->ReleaseStringUTFChars(familyNameStr, familyNameStr_);

    jstring newFamilyName = env->NewStringUTF("BBBBB");

    if (newFamilyName == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "newFamilyName == NULL");
        return;
    }

    env->SetObjectField(jobjFamily, familyNameID, newFamilyName);
    env->SetIntField(jobjFamily, familyAgeID, 350);
    env->SetObjectField(bean, familyID, jobjFamily);

}
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_admin_dnktest_MainActivity_addTen(JNIEnv *env, jobject instance, jintArray arr_) {

    //创建一个整形数组
//    jint len = env->GetArrayLength(arr_);
//
//    jintArray intArr = env->NewIntArray(len);
//    jint *arr = env->GetIntArrayElements(arr_, NULL);
//
//    jint buf[len];
//
//    jint i;
//    for (i = 0; i < len; i++) {
//        buf[i] = arr[i] + 10;
//        __android_log_print(ANDROID_LOG_INFO, "DJC", "buf =%d", buf[i]);
//    }
//
//    env->ReleaseIntArrayElements(arr_, arr, 0);
//    env->SetIntArrayRegion(intArr, 0, len, buf);//给jni创建的新数组赋值，arr_的值不变，但是要返回
//    return intArr;

//    env->SetIntArrayRegion(arr_, 0, len, buf);直接修改arr_的值
//    return NULL;


    jint len = env->GetArrayLength(arr_);
    jint *intArr = env->GetIntArrayElements(arr_, JNI_FALSE);
    jint i;
    for (int i = 0; i < len; ++i) {
//        *(intArr + i) += 10;
        intArr[i] += 10;
    }
    env->ReleaseIntArrayElements(arr_, intArr, 0);
    return arr_;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_admin_dnktest_MainActivity_getInt(JNIEnv *env, jobject instance, jstring value_) {
    const char *value = env->GetStringUTFChars(value_, 0);

    char *charValue = "123456";

    jint code = strcasecmp(value, charValue);

    env->ReleaseStringUTFChars(value_, value);
    if (code == 0) {
        return 200;
    } else {
        return 404;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_admin_dnktest_JniMethod_callAddSum(JNIEnv *env, jobject instance) {

    //通过反射查找类
    jclass cls = env->FindClass("com/example/admin/dnktest/JniMethod");
    //查找调用方法的ID
    jmethodID methodID = env->GetMethodID(cls, "addSum", "(II)I");
    //实例化类的对象
    jobject jobj = env->AllocObject(cls);
    if (jobj == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "DJC", "Out Of Memery");
        return;
    }
    //调用java方法
    jint value = env->CallIntMethod(jobj, methodID, 3, 4);
    LOGI("value = %d", value);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_admin_dnktest_JniMethod_getShowToast(JNIEnv *env, jobject instance,
                                                      jobject activity) {
    //通过对象查找类
    jclass cls = env->GetObjectClass(activity);
    //查找方法ID
    jmethodID jmethodID = env->GetMethodID(cls, "showToast", "()V");
    if (jmethodID == NULL) {
        LOGI("jmethodID = %s", jmethodID);
        return;
    }
    LOGI("jni层调用getShowToast");
    //调用方法
    env->CallVoidMethod(activity, jmethodID);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_admin_dnktest_MainActivity_appendStr(JNIEnv *env, jobject instance) {
    //通过对象获取类
    jclass _cls = env->GetObjectClass(instance);
    //获取属性ID
    jfieldID _id = env->GetFieldID(_cls, "str", "Ljava/lang/String;");
    //获取属性值
    jstring _str = (jstring) env->GetObjectField(instance, _id);
    //转换成char
    const char *dststr = env->GetStringUTFChars(_str, NULL);
    //创建一个char
    char oriStr[20] = "Success";//或者 char *oriStr = "success";
    //拼接字符串
    strcat((char *) dststr, oriStr);
    LOGI("dststr = %s", dststr);
    return env->NewStringUTF(dststr);
}

//定义一个比较的方法
int compare(const void *a, const void *b) {
    //*(int*)__lhs强制转成一个整形的指针，并且获取这个整形指针的数值
    return (*(int *) a - *(int *) b);
};
extern "C"
JNIEXPORT void JNICALL
Java_com_example_admin_dnktest_MainActivity_sortArr(JNIEnv *env, jobject instance, jintArray arr_) {
    jint *arr = env->GetIntArrayElements(arr_, NULL);
    //获取数组的长度
    jint len = env->GetArrayLength(arr_);
    //调用函数进行排序
    /**
     * __base   arr
     * sizeof(int)  获取整形的内存地址
     */
    qsort(arr, len, sizeof(int), compare);
    env->ReleaseIntArrayElements(arr_, arr, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_admin_dnktest_MainActivity_getException(JNIEnv *env, jobject instance) {

    jstring _str;

    jclass _class = env->GetObjectClass(instance);
    //获取一个java中没有声明的属性的ID
    jfieldID _jfieldID = env->GetFieldID(_class, "str1", "Ljava/lang/String;");
    //获取属性值
    //判断是否发生异常
    jthrowable _jthrowable = env->ExceptionOccurred();
    if (_jthrowable != NULL) {//捕捉到异常
        //清除异常
        env->ExceptionClear();
        _jfieldID = env->GetFieldID(_class, "str", "Ljava/lang/String;");
    }
    _str = (jstring) env->GetObjectField(instance, _jfieldID);
    char *str = (char *) env->GetStringUTFChars(_str, NULL);
    if (strcmp(str, "WWW") != 0) {
        //假设异常为非法参数异常
        jclass _cls = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(_cls, "非法参数异常");
    }
    env->ReleaseStringChars(_str, (const jchar *) str);
}