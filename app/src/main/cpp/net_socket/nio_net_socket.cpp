#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../base/util_log.h"
#include "../base/util_type.h"
#include "../base/app_build_config.h"
#include "../base/util_endian.h"


extern "C" {

    #include <pthread.h>

    //-----------------------------
    #include "nio_tcp_net_socket_client.h"
    #include "socket_config.h"
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

    JavaSocket java_socket[MAX_CLIENT_COUNT];
    int cur_connect_size;//当前的连接数
    int cur_max_index;//当前最大的索引数

    //前向定义函数
    void inner_close(JNIEnv *env, jobject thiz);

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
            java_socket[i].done = 1;
            java_socket[i].sfd = -1;
        }
        cur_connect_size = 0;
        cur_max_index = -1;

        if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
            return JNI_ERR; // JNI version not supported.
        }

        LOGE(kTAG,"JNI_OnLoad %s %s" , (getEndianType() == 1 ? "大端模式":"小端模式") ,(getEndianType2() == 1 ? "大端模式":"小端模式") );
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
                LOGE(kTAG,"on_socket_event AttachCurrentThread failed, ErrorCode = %d", res);
                return NULL;
            }
        }

        //1.连接
        int openErrorCode = 0;
        p_java_socket->sfd = nio_tcp_net_socket_open(p_java_socket->c_host,p_java_socket->c_port,&openErrorCode);
        env->CallVoidMethod(p_java_socket->j_socket_handler,p_java_socket->j_socket_handler_onSocketConnectResult,openErrorCode == 0);
        LOGE(kTAG,"on_socket_event start FD_SETSIZE %d openErrorCode %d fd %d",FD_SETSIZE,openErrorCode,p_java_socket->sfd);

        //2.读数据
        BYTE read_buf[MAX_MSG_LENGTH];
        while (p_java_socket->sfd != -1){

            pthread_mutex_lock(&p_java_socket->lock);
            int done = p_java_socket->done;
            pthread_mutex_unlock(&p_java_socket->lock);
            if (done) {
                break;
            }

            socklen_t len = nio_tcp_net_socket_read(p_java_socket->sfd,read_buf);
            if(len > 0){
                jbyteArray newArr = env->NewByteArray(len);
                env->SetByteArrayRegion(newArr,0, len,(jbyte *)read_buf);
                env->CallVoidMethod(p_java_socket->j_socket_handler,p_java_socket->j_socket_handler_onSocketResponse,newArr);
            }else if(len == 0){//若使用了select等系统函数，若远端断开，则select返回1，recv返回0则断开。其他注意事项同法一。
                p_java_socket->done = 0;
                break;
            }else if(len == -1){
                p_java_socket->done = 0;
                break;
            }else if(len == -100){//自定义错误，表示继续

            }
        }

        //3.断开网络
        inner_close(env,p_java_socket->j_socket);

        javaVM->DetachCurrentThread();
        LOGE(kTAG,"on_socket_event end DetachCurrentThread");
        return param;
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_open(JNIEnv *env, jobject thiz, jstring host, jint port,
                                            jobject handler) {

        LOGE(kTAG,"open start cur_connect_size %d cur_max_index %d",cur_connect_size,cur_max_index);
        if(cur_connect_size >= MAX_CLIENT_COUNT){
            LOGE(kTAG,"open failed , max connects is %d ",MAX_CLIENT_COUNT);
            return;
        }

        for (int i = 0 ;i< MAX_CLIENT_COUNT;i++) {
            jboolean isSame = env->IsSameObject(java_socket[i].j_socket,thiz);
            if(isSame == JNI_TRUE){
                LOGE(kTAG,"open isSame with index %d",i);
                return;
            }
        }

        pthread_t       threadInfo_;
        pthread_attr_t  threadAttr_;

        pthread_attr_init(&threadAttr_);
        pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);

        //找到下一个可用的
        cur_max_index ++;
        for(int i = 0;i<MAX_CLIENT_COUNT;i++){
            int nextIndex = (cur_max_index + i) % MAX_CLIENT_COUNT;
            if(java_socket[nextIndex].done){
                cur_max_index = nextIndex;
                break;
            }
        }

        JavaSocket * p_java_socket = &java_socket[cur_max_index];
        
        pthread_mutex_init(&(p_java_socket->lock), NULL);

        p_java_socket->j_socket = env->NewGlobalRef(thiz);
        p_java_socket->c_host = jstring_to_cchar(env,host);
        p_java_socket->c_port = port;
        p_java_socket->j_socket_handler = env->NewGlobalRef(handler);
        p_java_socket->done = 0;

        jclass  cls_j_socket_handler = env->GetObjectClass(handler);
        p_java_socket->j_socket_handler_onSocketConnectResult = env->GetMethodID(cls_j_socket_handler,"onSocketConnectResult", "(Z)V");
        p_java_socket->j_socket_handler_onSocketResponse = env->GetMethodID(cls_j_socket_handler,"onSocketResponse", "([B)V");
        env->DeleteLocalRef(cls_j_socket_handler);

        pthread_create( &threadInfo_, &threadAttr_, on_socket_event, p_java_socket);
        pthread_attr_destroy(&threadAttr_);

        cur_connect_size++;

        LOGE(kTAG, "open end cur_connect_size %d cur_max_index %d",cur_connect_size, cur_max_index);
    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_reconnect(JNIEnv *env, jobject thiz) {

    }

    JNIEXPORT void JNICALL
    Java_com_module_net_socket_CSocket_write(JNIEnv *env, jobject thiz, jbyteArray bytes) {

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
            LOGE(kTAG,"close AAA");

            JavaSocket * p_java_socket = &java_socket[index];
            pthread_mutex_lock(&p_java_socket->lock);
            int done = p_java_socket->done;
            if (p_java_socket->done == 0) {
                p_java_socket->done = 1;
            }
            pthread_mutex_unlock(&p_java_socket->lock);

            LOGE(kTAG,"close BBB done = %d",done);
        }else{
            LOGE(kTAG,"close index not found");
        }

        LOGE(kTAG,"close end");
    }

    void inner_close(JNIEnv *env, jobject thiz) {

        LOGE(kTAG,"inner_close start");

        int index = -1;
        for (int i = 0 ;i< MAX_CLIENT_COUNT;i++) {
            jboolean isSame = env->IsSameObject(java_socket[i].j_socket,thiz);
            if(java_socket[i].j_socket && isSame == JNI_TRUE){
                index = i;

                LOGE(kTAG,"inner_close index %d",i);
                break;
            }
        }

        if( index >= 0 ){
            LOGE(kTAG,"inner_close AAA");

            JavaSocket * p_java_socket = &java_socket[index];
            pthread_mutex_lock(&p_java_socket->lock);
            p_java_socket->done = 1;
            pthread_mutex_unlock(&p_java_socket->lock);

            LOGE(kTAG,"inner_close BBB");

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

            //移动位置是好的想法，但是如果如果有多个连接，前面的连接关闭后，导致后面的连接前移，而原先指针指向后面的对象，现在指向为空的对象，导致逻辑不对
//            JavaSocket t = java_socket[index];
//            for (int i = index ;i< MAX_CLIENT_COUNT-1;i++) {
//                java_socket[i] = java_socket[i+1];
//            }
//            java_socket[MAX_CLIENT_COUNT-1] = t;

            -- cur_connect_size;
        }else{
            LOGE(kTAG,"inner_close index not found");
        }

        LOGE(kTAG,"inner_close end");
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