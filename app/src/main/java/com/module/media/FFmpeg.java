package com.module.media;

public class FFmpeg {

    static {

        //error 以下将因链接顺序而报错，在电脑交叉编译后，lib目录下的pkgconfig 有libxxx.pc文件，描述了依赖情况
//        System.loadLibrary("avcodec");
//        System.loadLibrary("avfilter");
//        System.loadLibrary("avformat");
//        System.loadLibrary("avutil");
//        System.loadLibrary("swresample");
//        System.loadLibrary("swscale");

        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("swscale");
        System.loadLibrary("avfilter");
//        System.loadLibrary("avdevice");

//        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpegCmd");
    }

    public static native int execute(String []argv);

    public static native String getUrlProtocol();
    public static native String getAvformat();
    public static native String getAvCodec();
    public static native String getAvFilter();
    public static native String getConfiguration();

}
