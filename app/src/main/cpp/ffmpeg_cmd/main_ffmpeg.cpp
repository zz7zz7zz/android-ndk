#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../base/util_log.h"
#include "../base/util_type.h"
#include "../base/app_build_config.h"

extern "C"{

    #include "ffmpeg.h"

    JNIEXPORT jint JNICALL
    Java_com_module_media_FFmpeg_execute(JNIEnv *env, jclass clazz, jobjectArray commands) {

        int argc = env->GetArrayLength(commands);
        char *argv[argc];
        int i;
        for (i = 0; i < argc; i++) {
            jstring js = (jstring) env->GetObjectArrayElement(commands, i);
            argv[i] = (char *) env->GetStringUTFChars(js, 0);
            LOGD("ffmpeg.cpp","argCmd[%d]=%s",i,argv[i]);
        }

        int retCode = ffmpeg_exec(argc, argv);
        LOGD("ffmpeg.cpp","ffmpeg-invoke: retCode = %d",retCode);

        return 100;
    }


}