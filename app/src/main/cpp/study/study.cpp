#include <jni.h>
#include <string>

#include "../base/util_log.h"
#include "../base/util_type.h"


extern "C"{

    JNIEXPORT jboolean JNICALL
    Java_com_module_study_type_Type_paramBoolean(JNIEnv *env, jobject thiz, jboolean input) {
        LOGV("Testing","%s % d",__FUNCTION__,input);
        return !input;
    }

    JNIEXPORT jbyte JNICALL
    Java_com_module_study_type_Type_paramByte(JNIEnv *env, jobject thiz, jbyte input) {
        LOGV("Testing","%s % d",__FUNCTION__,input);
        return (jbyte)(input + 100);
    }

    JNIEXPORT jchar JNICALL
    Java_com_module_study_type_Type_paramChar(JNIEnv *env, jobject thiz, jchar input) {
        LOGV("Testing","%s % d",__FUNCTION__,input);
        return (jchar)(input + 1);
    }

    JNIEXPORT jshort JNICALL
    Java_com_module_study_type_Type_paramShort(JNIEnv *env, jobject thiz, jshort input) {
        LOGV("Testing","%s % d",__FUNCTION__,input);
        return (short) (32767 - input);
    }

    JNIEXPORT jint JNICALL
    Java_com_module_study_type_Type_paramInt(JNIEnv *env, jobject thiz, jint input) {
        return 0;
    }

    JNIEXPORT jlong JNICALL
    Java_com_module_study_type_Type_paramLong(JNIEnv *env, jobject thiz, jlong input) {
        return 0;
    }


    JNIEXPORT jfloat JNICALL
    Java_com_module_study_type_Type_paramFloat(JNIEnv *env, jobject thiz, jfloat input) {
        return 0;

    }

    JNIEXPORT jdouble JNICALL
    Java_com_module_study_type_Type_paramDouble(JNIEnv *env, jobject thiz, jdouble input) {
        return 0;
    }


    JNIEXPORT jobject JNICALL
    Java_com_module_study_type_Type_paramObject(JNIEnv *env, jobject thiz, jobject input) {
        return NULL;
    }

    JNIEXPORT jstring JNICALL
    Java_com_module_study_type_Type_paramString(JNIEnv *env, jobject thiz, jstring input) {

        //1.--------------------------------
//        jboolean isCopy;
//        const char * pchar = env->GetStringUTFChars(input,&isCopy);
//        LOGV("Testing","%s pchar %d input:%d",pchar,pchar,input);
//        env->ReleaseStringUTFChars(input,pchar);
//        LOGV("Testing",(isCopy == JNI_TRUE) ? "isCopy true" : "isCopy false");

        //2.--------------------------------
//        const char * pchar = env->GetStringUTFChars(input,NULL);
//        LOGV("Testing","A %s pchar %d input:%d",pchar,pchar,input);
//        env->ReleaseStringUTFChars(input,pchar);
//        LOGV("Testing","B %s pchar %d input:%d",pchar,pchar,input);
//        return env->NewStringUTF("ABC");

         //3.--------------------------------
//        jboolean isCopy;
//        const char * pchar = env->GetStringUTFChars(input,&isCopy);
//
//        int len1 = strlen(pchar);
//        LOGV("Testing","%d %d %d ",sizeof(pchar),sizeof(pchar[0]), sizeof(*pchar));
//        LOGV("Testing","%s %s pchar:%d input:%d len1 %d len2 %d",(isCopy == JNI_TRUE) ? "isCopy true" : "isCopy false",pchar,pchar,input,len1);
//
//        char * newchar = static_cast<char *>(malloc(len1 + 1 + 5));
//        strcpy(newchar,pchar);
//        strcat(newchar,"ABC");
//
//        jstring ret = env->NewStringUTF(newchar);
//        LOGV("Testing","%s " ,newchar);
//        free(newchar);
//
//        env->ReleaseStringUTFChars(input,pchar);
//        return ret;


        //4.jstring - > char *
//        char * pChar = jstring2cchar(env,input);
//        LOGV("Testing","jstring - > char * : %s ",pChar);
//        free(pChar);

        //5.char * -> jstring

        //yes
//        char ret [] = "cchar2jstring ret [] ";
//        return cchar2jstring(env,ret);
        //yes

        //yes
//       char * p = "cchar2jstring char * p ";
//       return cchar2jstring(env,p);
        //yes

        //yes
//        char * p = "from char * p ";
//        return env->NewStringUTF(p);
        //Yes

        //yes
//        char ret [] = "from char ret []";
//        return env->NewStringUTF(ret);
        //Yes

        //6.jstring -> string
//        std::string str = jstring_to_cppstring(env, input);
//        jstring_to_cppstring(env, input);

        //default
        return env->NewStringUTF("default");

    }

    JNIEXPORT jbooleanArray JNICALL
    Java_com_module_study_type_Type_paramBooleanArray(JNIEnv *env, jobject thiz,
                                                     jbooleanArray input) {
        return NULL;
    }

    JNIEXPORT jbyteArray JNICALL
    Java_com_module_study_type_Type_paramByteArray(JNIEnv *env, jobject thiz, jbyteArray input) {
        return NULL;
    }

    JNIEXPORT jcharArray JNICALL
    Java_com_module_study_type_Type_paramCharArray(JNIEnv *env, jobject thiz, jcharArray input) {
        return NULL;
    }

    JNIEXPORT jshortArray JNICALL
    Java_com_module_study_type_Type_paramShortArray(JNIEnv *env, jobject thiz, jshortArray input) {
        return NULL;
    }

    JNIEXPORT jintArray JNICALL
    Java_com_module_study_type_Type_paramIntArray(JNIEnv *env, jobject thiz, jintArray input) {
        return NULL;
    }

    JNIEXPORT jlongArray JNICALL
    Java_com_module_study_type_Type_paramLongArray(JNIEnv *env, jobject thiz, jlongArray input) {
        return NULL;
    }

    JNIEXPORT jfloatArray JNICALL
    Java_com_module_study_type_Type_paramFloatArray(JNIEnv *env, jobject thiz, jfloatArray input) {
        return NULL;
    }

    JNIEXPORT jdoubleArray JNICALL
    Java_com_module_study_type_Type_paramDoubleArray(JNIEnv *env, jobject thiz, jdoubleArray input) {
        return NULL;
    }

    JNIEXPORT void JNICALL
    Java_com_module_study_type_Type_maxLocalCapacity(JNIEnv *env, jobject thiz, jint input) {

        //1.JNI ERROR (app bug): local reference table overflow (max=512)
//        for (int i = 0;i<input;i++){
//            jclass cls  = (*env).GetObjectClass(thiz);
//            LOGV("Testing"," maxLocalCapacity %d ",i);
//        }

        //2.PushLocalFrame / PopLocalFrame
//        jint _input = 504;
//        env->PushLocalFrame(505);
//        jclass cls  = (*env).GetObjectClass(thiz);
//        jmethodID jmd = env->GetMethodID(cls,"<init>", "()V");
//
//        jobjectArray array = env->NewObjectArray(_input, cls, NULL);
//        for (int i = 0; i < _input; ++i) {
//
//            jobject jobj = env->NewObject(cls,jmd);
//            env->SetObjectArrayElement(array, i, jobj);
//            LOGV("Testing"," maxLocalCapacity %d ",i);
//        }
//        env->PopLocalFrame(array);

        //3.EnsureLocalCapacity 创建成功返回 (JNI_OK)0 ，失败返回小于 0
//        jint ret = env->EnsureLocalCapacity(505);
//        LOGV("Testing"," maxLocalCapacity %d ",ret);
    }
}