#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../base/util_log.h"
#include "../base/util_type.h"
#include "../base/app_build_config.h"



extern "C" {

    #include <pthread.h>

    //-----------------------------
    #include "bio_tcp_net_socket_client.h"
    #include "socket_config.h"

    int  server_socket_array[MAX_CLIENT_COUNT];
    int  max_fd_index;
    char msg_buf[MAX_MSG_LENGTH];
    //-----------------------------

    static const char* kTAG = "net_socket";

    typedef struct Java_Socket {
        jobject  j_socket;
        char *   c_host;
        int      c_port;
        jobject  j_socket_handler;
        jmethodID j_socket_handler_onSocketConnectResult;
        jmethodID j_socket_handler_onSocketResponse;

        JavaVM  *javaVM;

        pthread_mutex_t  lock;
        int      done;

        //-----------------------------
        int sfd;
        //-----------------------------

    } JavaSocket;
    JavaSocket java_socket;


    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
        JNIEnv* env;
        memset(&java_socket, 0, sizeof(java_socket));

        java_socket.javaVM = vm;
        java_socket.j_socket = NULL;
        java_socket.c_host = NULL;
        java_socket.c_port = 0;
        java_socket.j_socket_handler = NULL;
        java_socket.j_socket_handler_onSocketConnectResult = NULL;
        java_socket.j_socket_handler_onSocketResponse = NULL;
        java_socket.done = 0;

        java_socket.sfd = -1;

        if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
            return JNI_ERR; // JNI version not supported.
        }

        LOGE(kTAG,"JNI_OnLoad");
        return  JNI_VERSION_1_6;
    }

    void* on_socket_event(void* param) {
        LOGE(kTAG,"on_socket_event start");
        JavaSocket *p_java_socket = (JavaSocket*) param;
        JavaVM *javaVM = p_java_socket->javaVM;
        JNIEnv *env;
        jint res = javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
        if (res != JNI_OK) {
            res = javaVM->AttachCurrentThread(&env, NULL);
            if (JNI_OK != res) {
                LOGE(kTAG,"AttachCurrentThread failed, ErrorCode = %d", res);
                return NULL;
            }
        }

        struct timeval beginTime, curTime, usedTime, leftTime;
        const struct timeval kOneSecond = {
                (__kernel_time_t)1,
                (__kernel_suseconds_t) 0
        };

        //1.连接
        java_socket.sfd = tcp_net_socket_open(java_socket.c_host,java_socket.c_port);
        env->CallVoidMethod(java_socket.j_socket_handler,java_socket.j_socket_handler_onSocketConnectResult,java_socket.sfd>0);

        //2.读写数据
        BYTE read_buf[MAX_MSG_LENGTH];
        while (java_socket.sfd >0 && 1){
            gettimeofday(&beginTime, NULL);
            pthread_mutex_lock(&p_java_socket->lock);
            int done = p_java_socket->done;
            if (p_java_socket->done) {
                p_java_socket->done = 0;
            }
            pthread_mutex_unlock(&p_java_socket->lock);
            if (done) {
                break;
            }


            tcp_net_socket_read(java_socket.sfd,read_buf);


            env->CallVoidMethod(java_socket.j_socket_handler,java_socket.j_socket_handler_onSocketResponse,NULL);

            gettimeofday(&curTime, NULL);
            timersub(&curTime, &beginTime, &usedTime);
            timersub(&kOneSecond, &usedTime, &leftTime);
            struct timespec sleepTime;
            sleepTime.tv_sec = leftTime.tv_sec;
            sleepTime.tv_nsec = leftTime.tv_usec * 1000;

            if (sleepTime.tv_sec <= 1) {
                nanosleep(&sleepTime, NULL);
            }
        }

        javaVM->DetachCurrentThread();
        LOGE(kTAG,"on_socket_event end DetachCurrentThread");
        return param;
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_open(JNIEnv *env, jobject thiz, jstring host, jint port,
                                            jobject handler) {

        LOGE(kTAG,"open start");
        pthread_t       threadInfo_;
        pthread_attr_t  threadAttr_;

        pthread_attr_init(&threadAttr_);
        pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);

        pthread_mutex_init(&java_socket.lock, NULL);

        java_socket.j_socket = env->NewGlobalRef(thiz);
        java_socket.c_host = jstring_to_cchar(env,host);
        java_socket.c_port = port;
        java_socket.j_socket_handler = env->NewGlobalRef(handler);

        jclass  cls_j_socket_handler = env->GetObjectClass(handler);
        java_socket.j_socket_handler_onSocketConnectResult = env->GetMethodID(cls_j_socket_handler,"onSocketConnectResult", "(Z)V");
        java_socket.j_socket_handler_onSocketResponse = env->GetMethodID(cls_j_socket_handler,"onSocketResponse", "([B)V");
        env->DeleteLocalRef(cls_j_socket_handler);

        pthread_create( &threadInfo_, &threadAttr_, on_socket_event, &java_socket);
        pthread_attr_destroy(&threadAttr_);
        LOGE(kTAG,"open end");
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_reconnect(JNIEnv *env, jobject thiz) {
        // TODO: implement reconnect()
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_write(JNIEnv *env, jobject thiz, jbyteArray bytes) {
        // TODO: implement write()

        jsize length = env->GetArrayLength(bytes);
        jbyte *pjbyte = env->GetByteArrayElements(bytes,NULL);
        tcp_net_socket_send(java_socket.sfd,(BYTE *)pjbyte,length);
        env->ReleaseByteArrayElements(bytes,pjbyte,0);

    }

    JNIEXPORT jbyteArray JNICALL
    Java_com_module_net_socket_CSocket_read(JNIEnv *env, jobject thiz) {
        // TODO: implement read()
        return NULL;
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_close(JNIEnv *env, jobject thiz) {

        LOGE(kTAG,"close start");
        pthread_mutex_lock(&java_socket.lock);
        java_socket.done = 1;
        pthread_mutex_unlock(&java_socket.lock);

//         waiting for the network thread to flip the done flag
        struct timespec sleepTime;
        memset(&sleepTime, 0, sizeof(sleepTime));
        sleepTime.tv_nsec = 100000000;
        while (java_socket.done) {
            nanosleep(&sleepTime, NULL);
        }

        env->DeleteGlobalRef(java_socket.j_socket);
        java_socket.j_socket = NULL;

        if(java_socket.c_host){
            free(java_socket.c_host);
        }
        java_socket.c_host = NULL;

        java_socket.c_port = 0;

        env->DeleteGlobalRef(java_socket.j_socket_handler);
        java_socket.j_socket_handler = NULL;

        java_socket.j_socket_handler_onSocketConnectResult = NULL;
        java_socket.j_socket_handler_onSocketResponse = NULL;

        java_socket.sfd = -1;

        pthread_mutex_destroy(&java_socket.lock);
        LOGE(kTAG,"close end");
    }



}