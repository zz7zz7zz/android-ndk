#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../base/util_log.h"
#include "../base/util_type.h"
#include "../base/app_build_config.h"

#ifdef __cplusplus
extern "C" {
#endif

        const char* pkgs[] = {
                "com.module.ndk",//ndk测试
                "com.douyu.yanhuan",//陌遇马甲包
                "cn.douyu.moyu",
                "cn.cstf.cpzd",
                "com.douyu.milian",
                "com.mengyu.hualiao",
                "com.douyu.myjy"
        };

        const char md5SignEncrypt0xff[][48]={
            "AF:52:DA:62:DF:7D:D3:F1:F8:DE:88:07:B4:04:D0:00",//ndk测试
            "D6:FF:2F:C9:98:C3:27:0E:3D:FA:20:5B:58:0F:01:CC"//陌遇测试
        };

    void on_callback_verify_failed(JNIEnv *env,jclass jcls_Context ,jobject m_context);

    JNIEXPORT jint JNICALL Java_com_module_security_signature_Signature_verifySignature(JNIEnv *env, jobject thiz,
                                                          jobject m_context) {

        //1.对比包名
//        Context context = AppUtil.getApplication();//防止别人注入自定义Context
//        if(null == context){
//            context = mContext;
//        }
//        boolean isPkgMatch = false;
//        String pkg = context.getPackageName();
//        for (int i = 0; i < pkgs.length; i++) {
//            if(pkgs[i].equals(pkg)){
//                isPkgMatch = true;
//                break;
//            }
//        }
//        Log.v(TAG,"isPkgMatch " +isPkgMatch);
//        if(!isPkgMatch){
//            on_callback_verify_failed(mContext);
//            return;
//        }
        jclass      jcls_AppUtil = env->FindClass("com/module/utils/AppUtil");
        jmethodID   jmd_AppUtil_getApplication = env->GetStaticMethodID(jcls_AppUtil, "getApplication", "()Landroid/app/Application;");
        jobject     jobj_application = env->CallStaticObjectMethod(jcls_AppUtil, jmd_AppUtil_getApplication);
        if(jobj_application == NULL) {
            jobj_application = m_context;
        }
        jclass      jcls_Context = (*env).GetObjectClass(jobj_application);
        jmethodID   jmd_getPackageName = env->GetMethodID(jcls_Context, "getPackageName", "()Ljava/lang/String;");
        jstring     value_packageName = static_cast<jstring>(env->CallObjectMethod(jobj_application, jmd_getPackageName));
        const char* c_pkgName = env->GetStringUTFChars(value_packageName, NULL);
        if(c_pkgName == NULL) {
            return -10;
        }
        bool bPkgMatch = 0;
        int size = sizeof(pkgs)/sizeof(pkgs[0]);
        for(int i=0;i<size;i++){
            if(strcmp(pkgs[i],c_pkgName) == 0){
                bPkgMatch = 1;
                break;
            }
        }
        if(!bPkgMatch){
            on_callback_verify_failed(env,jcls_Context,jobj_application);
            return -1;
        }

        env->ReleaseStringUTFChars(value_packageName, c_pkgName);

        //2.签名对比
//        byte[] signBytes = null;
//        try {
//            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(pkg, PackageManager.GET_SIGNATURES);
//            Signature[] signs = packageInfo.signatures;
//            if ((signs == null) || (signs.length == 0)) {
//                return;
//            }
//            signBytes = signs[0].toByteArray();
//        } catch (PackageManager.NameNotFoundException e) {
//            e.printStackTrace();
//        }

        jmethodID   jmd_getPackageManager = env->GetMethodID(jcls_Context, "getPackageManager", "()Landroid/content/pm/PackageManager;");
        jobject     jobj_packageManager = env->CallObjectMethod(jobj_application, jmd_getPackageManager);
        jthrowable exc = env->ExceptionOccurred();
        if (exc) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            return -21;
        }

        //2.mContext.getPackageName()
//        jmethodID   jmd_getPackageName = env->GetMethodID(jcls_Context, "getPackageName", "()Ljava/lang/String;");
//        jstring     value_packageName = static_cast<jstring>(env->CallObjectMethod(m_context, jmd_getPackageName));

        //3.PackageManager.GET_SIGNATURES
        jclass      jcls_packageManager = env->FindClass("android/content/pm/PackageManager");
        jfieldID    jfd_get_signatures = env->GetStaticFieldID(jcls_packageManager, "GET_SIGNATURES", "I");
        jint        value_get_signatures = env->GetStaticIntField(jcls_packageManager, jfd_get_signatures);

        //4. PackageInfo packageInfo = PackageManager.getPackageInfo(Sting packageName, int flags)
        jmethodID   jmd_getPackageInfo = env->GetMethodID(jcls_packageManager, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
        jobject     jobj_packageInfo = env->CallObjectMethod(jobj_packageManager, jmd_getPackageInfo, value_packageName, value_get_signatures);

        env->DeleteLocalRef(jcls_packageManager);
        env->DeleteLocalRef(jobj_packageManager);
        env->DeleteLocalRef(value_packageName);

        //5.Signature[0]
        jclass      jcls_packageInfo = (*env).GetObjectClass(jobj_packageInfo);
        jfieldID    jfd_signatures = env->GetFieldID(jcls_packageInfo, "signatures", "[Landroid/content/pm/Signature;");
        jobjectArray value_signatures = static_cast<jobjectArray>(env->GetObjectField(jobj_packageInfo, jfd_signatures));

        env->DeleteLocalRef(jobj_packageInfo);

        if(NULL == value_signatures){
            return -22;
        }
        jsize length = env->GetArrayLength(value_signatures);
        if(length == 0){
            env->DeleteLocalRef(value_signatures);
            return -23;
        }

        jobject value_signatures_0 = env->GetObjectArrayElement(value_signatures, 0);

        jclass jcls_Signature = env->FindClass("android/content/pm/Signature");
        jmethodID jm_toByteArray = env->GetMethodID(jcls_Signature, "toByteArray", "()[B");
        jbyteArray value_byteArray = static_cast<jbyteArray>(env->CallObjectMethod(value_signatures_0, jm_toByteArray));

        env->DeleteLocalRef(value_signatures_0);

        /*
            byte[] digestBytes = null;
            try {
                MessageDigest digest = MessageDigest.getInstance("MD5");//"MD5"/"SHA-1"/"SHA-256"
                if (digest != null) {
                    digest.update(signBytes);
                    digestBytes = digest.digest();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
         */

        jclass      jcls_MessageDigest = env->FindClass("java/security/MessageDigest");
        jmethodID   jmd_MessageDigest_getInstance = env->GetStaticMethodID(jcls_MessageDigest, "getInstance", "(Ljava/lang/String;)Ljava/security/MessageDigest;");
        jobject     jobj_MessageDigest = env->CallStaticObjectMethod(jcls_MessageDigest, jmd_MessageDigest_getInstance,env->NewStringUTF("MD5"));
        exc = env->ExceptionOccurred();
        if (exc) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            return -24;
        }
        if(jobj_MessageDigest == NULL){
            return -25;
        }
        jmethodID jm_update = env->GetMethodID(jcls_MessageDigest, "update", "([B)V");
        env->CallVoidMethod(jobj_MessageDigest,jm_update,value_byteArray);
        jmethodID   jmd_digest = env->GetMethodID(jcls_MessageDigest, "digest", "()[B");
        jbyteArray value_byteArray2 = static_cast<jbyteArray> (env->CallObjectMethod(jobj_MessageDigest,jmd_digest));
        if(value_byteArray2 == NULL){
            return -26;
        }

        length = env->GetArrayLength(value_byteArray2);
        if( length == 0){
            return -27;
        }
//        StringBuilder sb = new StringBuilder(digestBytes.length *2 + (digestBytes.length-1));
//        for (int i = 0;i<digestBytes.length;i++) {
////          String str = Integer.toString(0xFF & digestBytes[i], 16);//原始值
//            String str = Integer.toString(0xFF & (~(0xFF & digestBytes[i])), 16);//加密后值
//            if (str.length() == 1) {
//                str = "0" + str;
//            }
//            sb.append(str);
//            if(i != digestBytes.length -1){
//                sb.append(":");
//            }
//        }
//        String MD5Sign = sb.toString().toUpperCase();
//
////        String[] md5SignOriginal = encrypt0xff(md5SignEncrypt0xff);
//        boolean isSignMatch = false;
//        for (int i = 0; i < md5SignEncrypt0xff.length; i++) {
//            if(md5SignEncrypt0xff[i].equals(MD5Sign)){
//                isSignMatch = true;
//                break;
//            }
//        }
//        Log.v(TAG,"isSignMatch " +isSignMatch);
//        if(!isSignMatch){
//            on_callback_verify_failed(mContext);
//            return;
//        }

        jbyte *pjbyte = env->GetByteArrayElements(value_byteArray2,NULL);
        size = 2*length+length;//+length 是因为(length -1 个":"和一个'\0')
        char *encrypt0xffString = (char *) malloc(size);
        memset(encrypt0xffString, 0, sizeof(encrypt0xffString));
        if(encrypt0xffString == NULL){
            return -31;
        }
        char buf[3];
        for(int i = 0;i<length;i++){
//          sprintf(buf, "%02X",0xff & pjbyte[i]);
            sprintf(buf, "%02X",0xff & (~(0xff & pjbyte[i])));
            strcat(encrypt0xffString, buf);
            if(i != length-1){
                strcat(encrypt0xffString, ":");
            }
        }
        env->ReleaseByteArrayElements(value_byteArray2,pjbyte,0);

        bool bSignMatch = 0;
        size = sizeof(md5SignEncrypt0xff) / sizeof(md5SignEncrypt0xff[0]);
        for(int i=0;i<size;i++){
            if(strcmp(md5SignEncrypt0xff[i], encrypt0xffString) == 0){
                bSignMatch = 1;
                break;
            }
        }
        free(encrypt0xffString);
        encrypt0xffString = NULL;

        env->DeleteLocalRef(value_byteArray2);
        env->DeleteLocalRef(value_byteArray);

        if(!bSignMatch){
            on_callback_verify_failed(env,jcls_Context,jobj_application);
            env->DeleteLocalRef(jcls_Context);
            env->DeleteLocalRef(value_signatures);
            return -2;
        }
        env->DeleteLocalRef(jcls_Context);
        env->DeleteLocalRef(value_signatures);
        LOGV("Testing","Java_com_module_security_signature_SignatureActivity_verifySignature success");
        return 0;
    }

    void on_callback_verify_failed(JNIEnv *env,jclass jcls_Context ,jobject m_context){
        //1.弹出Toast
//        Toast.makeText(mContext,"应用校验错误，将在 5s 后关闭应用 !",Toast.LENGTH_SHORT).show();
        jclass      jcls_Toast = env->FindClass("android/widget/Toast");
        jmethodID   jmd_Toast_makeText = env->GetStaticMethodID(jcls_Toast, "makeText", "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
        jfieldID    jfd_Toast_LENGTH_SHORT = env->GetStaticFieldID(jcls_Toast, "LENGTH_SHORT", "I");

        jint        value_length_long = env->GetStaticIntField(jcls_Toast, jfd_Toast_LENGTH_SHORT);
        jobject     jobj_Toast = env->CallStaticObjectMethod(jcls_Toast, jmd_Toast_makeText, m_context, env->NewStringUTF("应用校验错误，将在 8s 后关闭应用 !"), value_length_long);

        jmethodID jmd_show = env->GetMethodID(jcls_Toast,"show", "()V");
        env->CallVoidMethod(jobj_Toast,jmd_show);


        env->DeleteLocalRef(jcls_Toast);
        env->DeleteLocalRef(jobj_Toast);


        //2.启动错误页面
//        Intent mIntent = new Intent(mContext,SignErrorActivity.class);
//        mIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_NEW_TASK);
//        mContext.startActivity(mIntent);

        jclass      jcls_Intent = env->FindClass("android/content/Intent");
        jmethodID   jmd_Intent_init  = env->GetMethodID(jcls_Intent,"<init>", "(Landroid/content/Context;Ljava/lang/Class;)V");
        jclass      jcls_SignErrorActivity = env->FindClass("com/module/security/signature/SignErrorActivity");

        jobject     jobj_Intent = env->NewObject(jcls_Intent,jmd_Intent_init,m_context,jcls_SignErrorActivity);

        jfieldID    jfd_flag_activity_clear_task = env->GetStaticFieldID(jcls_Intent, "FLAG_ACTIVITY_CLEAR_TASK", "I");
        jint        value_flag_activity_clear_task = env->GetStaticIntField(jcls_Intent, jfd_flag_activity_clear_task);

        jfieldID    jfd_flag_activity_new_task = env->GetStaticFieldID(jcls_Intent, "FLAG_ACTIVITY_NEW_TASK", "I");
        jint        value_flag_activity_new_task = env->GetStaticIntField(jcls_Intent, jfd_flag_activity_new_task);

        jmethodID   jmd_addFlags =env->GetMethodID(jcls_Intent,"addFlags","(I)Landroid/content/Intent;");
        env->CallObjectMethod(jobj_Intent,jmd_addFlags,value_flag_activity_clear_task | value_flag_activity_new_task);

        jmethodID  jmd_startActivity = env->GetMethodID(jcls_Context,"startActivity", "(Landroid/content/Intent;)V");
        env->CallVoidMethod(m_context,jmd_startActivity,jobj_Intent);

        env->DeleteLocalRef(jcls_Intent);
        env->DeleteLocalRef(jcls_SignErrorActivity);
        env->DeleteLocalRef(jobj_Intent);

        //3.延迟关闭应用
//        new Handler().postDelayed(new SignExitTask(),5000);
        jclass     jcls_SignExitTask = env->FindClass("com/module/security/signature/SignExitTask");
        jmethodID  jmd_SignExitTask_init  = env->GetMethodID(jcls_SignExitTask,"<init>", "()V");
        jobject    jobj_SignExitTask = env->NewObject(jcls_SignExitTask,jmd_SignExitTask_init);

        jclass     jcls_Handler = env->FindClass("android/os/Handler");
        jmethodID  jmd_Handler_init  = env->GetMethodID(jcls_Handler,"<init>", "()V");
        jobject    jobj_Handler = env->NewObject(jcls_Handler,jmd_Handler_init);

        jmethodID  jmd_postDelayed = env->GetMethodID(jcls_Handler,"postDelayed", "(Ljava/lang/Runnable;J)Z");
        env->CallBooleanMethod(jobj_Handler,jmd_postDelayed,jobj_SignExitTask,(long long)(8000+500));
    }

#ifdef __cplusplus
}
#endif