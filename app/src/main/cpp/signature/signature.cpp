#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../base/util_log.h"
#include "../base/app_build_config.h"

#ifdef __cplusplus
extern "C" {
#endif

        #include <unistd.h>
        #include <pthread.h>

        static const char* kTAG = "Testing";
        static const int sign_error_ttl = 15;//15秒后退出应用
        JavaVM  *gJavaVM;

        const char* pkgs[] = {
                "com.module.ndk",//ndk测试
                "com.douyu.yanhuan",//陌遇马甲包
                "cn.douyu.moyu",
                "cn.cstf.cpzd",
                "com.douyu.milian",
                "com.zzllgglove.mo",
                "com.mengyu.hualiao",
                "com.douyu.myjy",
                "com.douyu.miai",
                "com.douyu.wmbl",
                "com.douyu.videochat",
                "cn.xinzepinapp.yh"
        };

        const char md5SignEncrypt0xff[][48]={
                "AF:52:DA:62:DF:7D:D3:F1:F8:DE:88:07:B4:04:D0:00",//ndk测试
                "D6:FF:2F:C9:98:C3:27:0E:3D:FA:20:5B:58:0F:01:CC",//Miqu
                "38:4D:00:3F:1B:84:F4:EF:7B:13:2E:69:B6:40:BB:75",//同城随心遇
                "75:C7:4B:B5:93:39:7A:F6:EE:9A:C2:6F:0B:9B:2E:C2",//Moyu
                "7C:3C:92:6C:60:BF:B9:68:27:92:58:2C:8C:66:34:99"
        };

        void  on_callback_verify_failed(JNIEnv *env,jclass jcls_Context ,jobject m_context);
        void* on_callback_thread(void* arg);

        JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
            JNIEnv* env;
            gJavaVM = vm;
            if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
                return JNI_ERR; // JNI version not supported.
            }

            return  JNI_VERSION_1_6;
        }

        JNIEXPORT jint JNICALL Java_com_lib_security_signature_Signature_verifySignature(JNIEnv *env, jobject thiz,
                                                              jobject m_context) {

            //1.对比包名
            jclass      jcls_AppUtil = env->FindClass("com/lib/utils/AppUtil");
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

            //5.Signature[0]
            jclass      jcls_packageInfo = (*env).GetObjectClass(jobj_packageInfo);
            jfieldID    jfd_signatures = env->GetFieldID(jcls_packageInfo, "signatures", "[Landroid/content/pm/Signature;");
            jobjectArray value_signatures = static_cast<jobjectArray>(env->GetObjectField(jobj_packageInfo, jfd_signatures));

            if(NULL == value_signatures){
                return -22;
            }
            jsize length = env->GetArrayLength(value_signatures);
            if(length == 0){
                return -23;
            }

            jobject value_signatures_0 = env->GetObjectArrayElement(value_signatures, 0);

            jclass jcls_Signature = env->FindClass("android/content/pm/Signature");
            jmethodID jm_toByteArray = env->GetMethodID(jcls_Signature, "toByteArray", "()[B");
            jbyteArray value_byteArray = static_cast<jbyteArray>(env->CallObjectMethod(value_signatures_0, jm_toByteArray));

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


            if(!bSignMatch){
                on_callback_verify_failed(env,jcls_Context,jobj_application);
                return -2;
            }
            LOGV(kTAG,"from native verifySignature success");
            return 0;
        }

        void on_callback_verify_failed(JNIEnv *env,jclass jcls_Context ,jobject m_context){
            //2.启动错误页面
            jclass      jcls_Intent = env->FindClass("android/content/Intent");
            jmethodID   jmd_Intent_init  = env->GetMethodID(jcls_Intent,"<init>", "(Landroid/content/Context;Ljava/lang/Class;)V");
            jclass      jcls_SignErrorActivity = env->FindClass("com/lib/security/signature/SignErrorActivity");

            jobject     jobj_Intent = env->NewObject(jcls_Intent,jmd_Intent_init,m_context,jcls_SignErrorActivity);

            jfieldID    jfd_flag_activity_clear_task = env->GetStaticFieldID(jcls_Intent, "FLAG_ACTIVITY_CLEAR_TASK", "I");
            jint        value_flag_activity_clear_task = env->GetStaticIntField(jcls_Intent, jfd_flag_activity_clear_task);

            jfieldID    jfd_flag_activity_new_task = env->GetStaticFieldID(jcls_Intent, "FLAG_ACTIVITY_NEW_TASK", "I");
            jint        value_flag_activity_new_task = env->GetStaticIntField(jcls_Intent, jfd_flag_activity_new_task);

            jmethodID   jmd_addFlags =env->GetMethodID(jcls_Intent,"addFlags","(I)Landroid/content/Intent;");
            env->CallObjectMethod(jobj_Intent,jmd_addFlags,value_flag_activity_clear_task | value_flag_activity_new_task);

            jmethodID   jmd_putExtra =env->GetMethodID(jcls_Intent,"putExtra","(Ljava/lang/String;I)Landroid/content/Intent;");
            env->CallObjectMethod(jobj_Intent,jmd_putExtra,env->NewStringUTF("sign_error_ttl"),sign_error_ttl);

            jmethodID  jmd_startActivity = env->GetMethodID(jcls_Context,"startActivity", "(Landroid/content/Intent;)V");
            env->CallVoidMethod(m_context,jmd_startActivity,jobj_Intent);

            //未将SignErrorActivity 注册进AndroidManifest.xml时不报错
            jthrowable exc = env->ExceptionOccurred();
            if (exc) {
                LOGV(kTAG,"已抛出异常，未将SignErrorActivity注册进AndroidManifest.xml中，但是仍然会在15秒退出程序");
                env->ExceptionDescribe();
                env->ExceptionClear();
            }

            //3.2.通过创建线程的方式
            pthread_t       threadInfo_;
            pthread_attr_t  threadAttr_;

            pthread_attr_init(&threadAttr_);
            pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);

            pthread_create(&threadInfo_, &threadAttr_, on_callback_thread, gJavaVM);
            pthread_attr_destroy(&threadAttr_);
        }

        void* on_callback_thread(void* arg){
            JavaVM *javaVM = (JavaVM *)arg;
            JNIEnv *env = NULL;
            jint res = javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
            if (res != JNI_OK) {
                res = javaVM->AttachCurrentThread(&env, NULL);
                if (JNI_OK != res) {
                    LOGV("Testing","Failed to AttachCurrentThread, ErrorCode = %d", res);
                    return NULL;
                }
            }

            sleep(sign_error_ttl);

            jclass      jcls_Process = env->FindClass("android/os/Process");
            jmethodID   jmd_Process_myPid= env->GetStaticMethodID(jcls_Process, "myPid", "()I");
            jint        myPid= env->CallStaticIntMethod(jcls_Process, jmd_Process_myPid);
            jmethodID   jmd_Process_killProcess= env->GetStaticMethodID(jcls_Process, "killProcess", "(I)V");
            env->CallStaticVoidMethod(jcls_Process, jmd_Process_killProcess,myPid);

            jclass      jcls_System = env->FindClass("java/lang/System");
            jmethodID   jmd_exit= env->GetStaticMethodID(jcls_System, "exit", "(I)V");
            env->CallStaticVoidMethod(jcls_System, jmd_exit,-1);

            javaVM->DetachCurrentThread();
            return arg;
        }

#ifdef __cplusplus
}
#endif