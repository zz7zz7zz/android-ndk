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
    #include "nio_tcp_net_socket_client.h"
    #include "socket_config.h"

    int  server_socket_array[MAX_CLIENT_COUNT];
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

    int  max_index;
    JavaSocket java_socket[MAX_CLIENT_COUNT];

    //前向定义函数
    JNIEXPORT void JNICALL Java_com_module_net_socket_CSocket_close(JNIEnv *env, jobject thiz);

    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
        JNIEnv* env;

        for (int i = 0 ;i< MAX_CLIENT_COUNT;i++){


            memset(&java_socket[i], 0, sizeof(java_socket[i]));

            java_socket[i].javaVM = vm;
            java_socket[i].j_socket = NULL;
            java_socket[i].c_host = NULL;
            java_socket[i].c_port = 0;
            java_socket[i].j_socket_handler = NULL;
            java_socket[i].j_socket_handler_onSocketConnectResult = NULL;
            java_socket[i].j_socket_handler_onSocketResponse = NULL;
            java_socket[i].done = 0;

            java_socket[i].sfd = -1;
        }
        max_index = 0;

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
        int openErrorCode = 0;
        p_java_socket->sfd = nio_tcp_net_socket_open(p_java_socket->c_host,p_java_socket->c_port,&openErrorCode);
        env->CallVoidMethod(p_java_socket->j_socket_handler,p_java_socket->j_socket_handler_onSocketConnectResult,openErrorCode == 0);

        LOGE(kTAG,"on_socket_event start FD_SETSIZE %d openErrorCode %d fd %d",FD_SETSIZE,openErrorCode,p_java_socket->sfd);

        pthread_mutex_lock(&p_java_socket->lock);
        p_java_socket->done = 0;
        pthread_mutex_unlock(&p_java_socket->lock);

        //2.读写数据
        BYTE read_buf[MAX_MSG_LENGTH];
        while (p_java_socket->sfd != -1){
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


            socklen_t len = nio_tcp_net_socket_read(p_java_socket->sfd,read_buf);
            if(len == -1){
                p_java_socket->done = 0;
                break;
            }

            if(len > 0){
                env->CallVoidMethod(p_java_socket->j_socket_handler,p_java_socket->j_socket_handler_onSocketResponse,NULL);
            }


            gettimeofday(&curTime, NULL);
            timersub(&curTime, &beginTime, &usedTime);
            timersub(&kOneSecond, &usedTime, &leftTime);
            struct timespec sleepTime;
            sleepTime.tv_sec = 5;
            sleepTime.tv_nsec = leftTime.tv_usec * 1000;

//            if (sleepTime.tv_sec <= 1) {
                nanosleep(&sleepTime, NULL);
//            }
        }

//        Java_com_module_net_socket_CSocket_close(env,p_java_socket->j_socket);

        javaVM->DetachCurrentThread();
        LOGE(kTAG,"on_socket_event end DetachCurrentThread");

        return param;
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_open(JNIEnv *env, jobject thiz, jstring host, jint port,
                                            jobject handler) {

        LOGE(kTAG,"open start");
        if(max_index >= MAX_CLIENT_COUNT){
            LOGE(kTAG,"open failed , max connects is %d ",MAX_CLIENT_COUNT);
            return;
        }

        for (int i = 0 ;i< MAX_CLIENT_COUNT;i++) {
            jboolean isSame = env->IsSameObject(java_socket[i].j_socket,thiz);
            if(isSame == JNI_TRUE){
                LOGE(kTAG,"open issame with index %d",i);
                return;
            }
        }

        pthread_t       threadInfo_;
        pthread_attr_t  threadAttr_;

        pthread_attr_init(&threadAttr_);
        pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);


        JavaSocket * p_java_socket = &java_socket[max_index];
        
        pthread_mutex_init(&(p_java_socket->lock), NULL);

        p_java_socket->j_socket = env->NewGlobalRef(thiz);
        p_java_socket->c_host = jstring_to_cchar(env,host);
        p_java_socket->c_port = port;
        p_java_socket->j_socket_handler = env->NewGlobalRef(handler);

        jclass  cls_j_socket_handler = env->GetObjectClass(handler);
        p_java_socket->j_socket_handler_onSocketConnectResult = env->GetMethodID(cls_j_socket_handler,"onSocketConnectResult", "(Z)V");
        p_java_socket->j_socket_handler_onSocketResponse = env->GetMethodID(cls_j_socket_handler,"onSocketResponse", "([B)V");
        env->DeleteLocalRef(cls_j_socket_handler);

        pthread_create( &threadInfo_, &threadAttr_, on_socket_event, p_java_socket);
        pthread_attr_destroy(&threadAttr_);

        max_index++;

        LOGE(kTAG,"open end .socket size %d ",max_index);
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_reconnect(JNIEnv *env, jobject thiz) {
        // TODO: implement reconnect()
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_write(JNIEnv *env, jobject thiz, jbyteArray bytes) {
        // TODO: implement write()

        int index = -1;
        for (int i = 0 ;i< MAX_CLIENT_COUNT;i++) {
            jboolean isSame = env->IsSameObject(java_socket[i].j_socket,thiz);
            if(isSame == JNI_TRUE){
                index = i;

                LOGE(kTAG,"write index %d",i);
                break;
            }
        }
        if(index>=0 ) {

            jsize length = env->GetArrayLength(bytes);
            jbyte *pjbyte = env->GetByteArrayElements(bytes,NULL);
            nio_tcp_net_socket_send(java_socket[index].sfd,(BYTE *)pjbyte,length);
            env->ReleaseByteArrayElements(bytes,pjbyte,0);
        }

    }

    JNIEXPORT jbyteArray JNICALL
    Java_com_module_net_socket_CSocket_read(JNIEnv *env, jobject thiz) {
        // TODO: implement read()
        return NULL;
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_close(JNIEnv *env, jobject thiz) {

        LOGE(kTAG,"close start");

        int index = -1;
        for (int i = 0 ;i< MAX_CLIENT_COUNT;i++) {
            jboolean isSame = env->IsSameObject(java_socket[i].j_socket,thiz);
            if(java_socket[i].j_socket && isSame == JNI_TRUE){
                index = i;

                LOGE(kTAG,"close index %d",i);
                break;
            }
        }

        if( index >= 0 ){
            JavaSocket * p_java_socket = &java_socket[index];

            LOGE(kTAG,"close AAA");
//            pthread_mutex_lock(&p_java_socket->lock);
            p_java_socket->done = 1;
//            pthread_mutex_unlock(&p_java_socket->lock);

            LOGE(kTAG,"close BBB");

//         waiting for the network thread to flip the done flag
//            struct timespec sleepTime;
//            memset(&sleepTime, 0, sizeof(sleepTime));
//            sleepTime.tv_nsec = 100000000;
//            while (p_java_socket->done) {
//                nanosleep(&sleepTime, NULL);
//            }

            env->DeleteGlobalRef(p_java_socket->j_socket);
            p_java_socket->j_socket = NULL;

            if(p_java_socket->c_host){
                free(p_java_socket->c_host);
            }
            p_java_socket->c_host = NULL;

            p_java_socket->c_port = 0;

            env->DeleteGlobalRef(p_java_socket->j_socket_handler);
            p_java_socket->j_socket_handler = NULL;

            p_java_socket->j_socket_handler_onSocketConnectResult = NULL;
            p_java_socket->j_socket_handler_onSocketResponse = NULL;

            nio_tcp_net_socket_close(p_java_socket->sfd);

            p_java_socket->sfd = -1;

            pthread_mutex_destroy(&p_java_socket->lock);


            JavaSocket t = java_socket[index];
            for (int i = index ;i< MAX_CLIENT_COUNT-1;i++) {
                java_socket[i] = java_socket[i+1];
            }
            java_socket[MAX_CLIENT_COUNT-1] = t;

            -- max_index;
        }else{
            LOGE(kTAG,"close index not found");
        }

        LOGE(kTAG,"close end");
    }

    /**

//clang 3.8.0

#include  <stdio.h>

struct A{
	int i;
	int j;
};
int main() {

	struct A a[5];
	for(int i = 0;i<5;i++){
		a[i].i = i;
		a[i].j = i;
	}

	for(int i = 0;i<5;i++){
		printf(" %d %d %d ",&a[i],a[i].i,a[i].j);
	}

	printf("-------------\n");

	int index = 1;
	struct A t = a[index];
	printf("---------%d %d \n", &a[index],&t);
 	for (int i = index ;i< 5-1;i++) {
		a[i] = a[i+1];
	}
    a[5-1] = t;

	for(int i = 0;i<5;i++){
		printf(" %d %d %d",&a[i],a[i].i,a[i].j);
	}

    printf("-------------" );
    return 0;
}


     //clang 3.8.0

#include  <stdio.h>

typedef struct {
	int i;
	int j;
} ClassA;


int main() {

	ClassA c;
	c.i = 100;
	c.j=1000;

	ClassA d = c;
	printf("---------%d %d %d % d", c,d,d.i,d.j);

    return 0;
}
     */
}