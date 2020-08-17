# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
            ffmpegCmd

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
        cmdutils.c ffmpeg_filter.c ffmpeg_opt.c ffmpeg_hw.c ffmpeg.c main_ffmpeg.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#target_link_libraries( # Specifies the target library.
#                        ffmpeg
#
#                       # Links the target library to the log library
#                       # included in the NDK.
#                       ${log-lib} )

# --------------------------------1.FFmpeg so依赖库引入-----------------------------
# configure import libs
#set(distribution_ffmpeg_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../distribution_ffmpeg)
#需要取子CMakeLists.txt的相对路径
set(distribution_ffmpeg_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../distribution_ffmpeg_cmd)

# shared lib will also be tucked into APK and sent to target
# refer to app/build.gradle, jniLibs section for that purpose.
# ${ANDROID_ABI} is handy for our purpose here. Probably this ${ANDROID_ABI} is
# the most valuable thing of this sample, the rest are pretty much normal cmake
add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION
        ${distribution_ffmpeg_DIR}/lib/${ANDROID_ABI}/libavcodec.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION
        ${distribution_ffmpeg_DIR}/lib/${ANDROID_ABI}/libavfilter.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION
        ${distribution_ffmpeg_DIR}/lib/${ANDROID_ABI}/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION
        ${distribution_ffmpeg_DIR}/lib/${ANDROID_ABI}/libavutil.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION
        ${distribution_ffmpeg_DIR}/lib/${ANDROID_ABI}/libswresample.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION
        ${distribution_ffmpeg_DIR}/lib/${ANDROID_ABI}/libswscale.so)

# --------------------------------2.FFmpeg 引入头文件-----------------------------
#target_include_directories(ffmpegCmd PRIVATE ${distribution_ffmpeg_DIR}/include ./include)
#include_directories(/Users/long/ffmpeg-4.2.2-20200517) ERROR
#include_directories(${distribution_ffmpeg_DIR}/include) //config.h路径引用问题比较麻烦
include_directories(./include)


# --------------------------------3.告诉编译器生成自己的库需要链接的库-----------------------------
target_link_libraries( # Specifies the target library.
        ffmpegCmd
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        )

