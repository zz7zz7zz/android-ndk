//
// Created by Long on 2020/12/15.
//

#ifndef ANDROIDNDK_UTIL_ENDIAN_H
#define ANDROIDNDK_UTIL_ENDIAN_H

/*
大端模式，是指数据的高字节保存在内存的低地址中，而数据的低字节保存在内存的高地址中，这样的存储模式有点儿类似于把数据当作字符串顺序处理：地址由小向大增加，而数据从高位往低位放；这和我们的阅读习惯一致。
小端模式，是指数据的高字节保存在内存的高地址中，而数据的低字节保存在内存的低地址中，这种存储模式将地址的高低和数据位权有效地结合起来，高地址部分权值高，低地址部分权值低。

下面以unsigned int value = 0x12345678为例，分别看看在两种字节序下其存储情况，我们可以用unsigned char buf[4]来表示value

 Big-Endian: 低地址存放高位，如下：
高地址
　　---------------
　　buf[3] (0x78) -- 低位
　　buf[2] (0x56)
　　buf[1] (0x34)
　　buf[0] (0x12) -- 高位
　　---------------
　　低地址

 Little-Endian: 低地址存放低位，如下：
高地址
　　---------------
　　buf[3] (0x12) -- 高位
　　buf[2] (0x34)
　　buf[1] (0x56)
　　buf[0] (0x78) -- 低位
　　--------------
低地址
        内存地址	小端模式存放内容	大端模式存放内容
0x4000	0x78	0x12
0x4001	0x56	0x34
0x4002	0x34	0x56
0x4003	0x12	0x78

 */

//返回值1.大端模式，2小端模式
u_char getEndianType(){
    typedef union{
        int a;
        char b;
    }EndianTest;    /* 定义一个联合体数据类型 */

    EndianTest et;
    et.a = 0x12345678;
    if(et.b == 0x78){
        return 2;//小端模式
    }else{
        return 1;//大端模式
    }
}


u_char getEndianType2(){
    short int x;
    char x0,x1;
    x=0x1122;
    x0=((char*)&x)[0]; //低地址单元
    x1=((char*)&x)[1]; //高地址单元

    if(x0 == 0x22){
        return 2;//小端模式
    }else{
        return 1;//大端模式
    }
}

#endif //ANDROIDNDK_UTIL_ENDIAN_H
