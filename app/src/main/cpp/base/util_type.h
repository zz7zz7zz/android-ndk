//
// Created by 杨龙辉 on 2020/5/3.
//

#ifndef ANDROID_NDK_UTIL_TYPE_H
#define ANDROID_NDK_UTIL_TYPE_H

#include "jni.h"
#include "string.h"

//1.字符串转换
std::string jstring_to_cppstring(JNIEnv *env, jstring value){
    jboolean isCopy;
    const char* c_value = env->GetStringUTFChars(value, &isCopy);
    std::string ret(c_value);
    if (isCopy == JNI_TRUE)
        env->ReleaseStringUTFChars(value, c_value);
    return ret;
}

jstring cppstring_to_jstring(JNIEnv *env,std::string& value){
    return env->NewStringUTF(value.c_str());
}

char* jstring_to_cchar(JNIEnv *env,jstring jstr){

    // byte[] bytes= "str".getBytes("utf-8");

    char * ret;
    jclass jcls_string = env->FindClass("java/lang/String");
    jstring jstr_utf_8 = env->NewStringUTF("utf-8");
    jmethodID jmd_getBytes = env->GetMethodID(jcls_string,"getBytes", "(Ljava/lang/String;)[B");
    jbyteArray jba = static_cast<jbyteArray>(env->CallObjectMethod(jstr, jmd_getBytes, jstr_utf_8));
    jsize len = env->GetArrayLength(jba);
    jbyte * jb = env->GetByteArrayElements(jba, JNI_FALSE);
    if(len > 0){
        ret = static_cast<char *>(malloc(len + 1));
        memcpy(ret,jb,len);
        ret[len] = 0;
    }
    env->ReleaseByteArrayElements(jba,jb,0);
    return ret;
}

jstring cchar_to_jstring(JNIEnv *env,char* value){

    return  env->NewStringUTF(value);

//    jclass jcls_string = env->FindClass("java/lang/String");
//    jmethodID jmd_ctor = env->GetMethodID(jcls_string,"<init>", "([BLjava/lang/String;)V");
//    jbyteArray jba = env->NewByteArray(strlen(value));
//    env->SetByteArrayRegion(jba, 0, strlen(value), reinterpret_cast<const jbyte *>(value));
//    jstring jstring_utf8 = env->NewStringUTF("utf-8");
//    return static_cast<jstring>(env->NewObject(jcls_string, jmd_ctor, jba, jstring_utf8));
}

char * cppstring_to_cchar(){
    return NULL;
}

char * cchar_to_cppstring_(){
    return NULL;
}

//2.bool型转换
bool jboolean_to_cbool(JNIEnv *env,jboolean value){
    return value == JNI_TRUE;
}

jboolean cbool_to_jboolean(JNIEnv *env, bool value){
    return value ? JNI_TRUE : JNI_FALSE;
}

//3.byte型转换

//4.char型转换

//5.short型转换

//6.int型转换

//7.long型转换

//8.float型转换

//9.double型转换

#endif //ANDROID_NDK_UTIL_TYPE_H
