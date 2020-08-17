//
// Created by 杨龙辉 on 2020/5/3.
//


#ifndef ANDROID_NDK_UTIL_LOG_H
#define ANDROID_NDK_UTIL_LOG_H

#include "app_build_config.h"

    #ifdef G_DEBUG

        #include <android/log.h>

        #define LOGV(TAG , ...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__))

        #define LOGD(TAG , ...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))

        #define LOGI(TAG , ...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))

        #define LOGW(TAG , ...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))

        #define LOGE(TAG , ...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))

    #else

        #define LOGV(TAG , ...)

        #define LOGD(TAG , ...)

        #define LOGI(TAG , ...)

        #define LOGW(TAG , ...)

        #define LOGE(TAG , ...)

    #endif


#endif //ANDROID_NDK_UTIL_LOG_H
