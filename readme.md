定位crash问题方法

1.使用addr2line工具（不同CPU架构，不同路径）
/Users/long/Library/Android/sdk/ndk/21.0.6113669/toolchains/x86-4.9/prebuilt/darwin-x86_64/bin/i686-linux-android-addr2line -e /Users/long/Downloads/android-ndk/app/build/intermediates/cmake/debug/obj/x86/libnio_net_socket.so 0000af81


2.ndk-stack

2.1 实时分析日志 
    adb logcat | ndk-stack -sym /Users/long/Downloads/android-ndk/app/build/intermediates/cmake/debug/obj/x86

2.2 通过日志分析
    ndk-stack -sym /Users/long/Downloads/android-ndk/app/build/intermediates/cmake/debug/obj/x86 -dump /Users/long/Downloads/android-ndk/crash.log

3.使用arm-linux-androideabi-objdump  定位出错的函数信息
/Users/long/Library/Android/sdk/ndk/21.0.6113669/toolchains/x86_64-4.9/prebuilt/darwin-x86_64/bin/x86_64-linux-android-objdump -S -D /Users/long/Downloads/android-ndk/app/build/intermediates/cmake/debug/obj/x86/libnio_net_socket.so >/Users/long/Downloads/android-ndk/crash_detail.log


举例日志地址：/Users/long/Downloads/android-ndk/crash.log