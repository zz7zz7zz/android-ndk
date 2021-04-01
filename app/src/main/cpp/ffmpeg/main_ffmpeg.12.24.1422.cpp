#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../base/util_log.h"
#include "../base/util_type.h"
#include "../base/app_build_config.h"

extern "C"{

#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavutil/fifo.h"
#include "libavcodec/avcodec.h"
//#include "libavcodec/qsv.h"
#include "libavformat/avformat.h"
//	#include "libavformat/url.h"
#include "libavformat/avio.h"
//	#include "libavfilter/avcodec.h"
//#include "libavfilter/avfiltergraph.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
//#include "libavdevice/avdevice.h"

    static const char* kTAG = "ffmpeg";

    //-------------水印变量 start---------------
    AVFormatContext *input_video_av_fmt_cxt;
    AVFormatContext *input_image_av_fmt_cxt;
    AVFormatContext *output_video_av_fmt_cxt;

    AVCodecContext *input_video_av_codec_cxt;
    AVCodecContext *input_image_av_codec_cxt;
    AVCodecContext *out_video_av_codec_cxt;
    int input_video_stream_idx;
    int input_image_stream_idx;

    AVFilterInOut *inputs;
    AVFilterInOut *outputs;
    AVFilterGraph * av_filter_graph;

    AVFilterContext *inputFilterContext[2];
    AVFilterContext *outputFilterContext = nullptr;

    const char *filter_descr = "overlay=100:100";
    //-------------水印变量 end---------------

    JNIEXPORT jstring JNICALL
    Java_com_module_media_FFmpeg_getUrlProtocol(JNIEnv *env, jclass clazz) {
        char info[40000] = {0};
        av_register_all();
        struct URLProtocol *pup = NULL;
        struct URLProtocol **p_temp = &pup;
        avio_enum_protocols((void **) p_temp, 0);
        while ((*p_temp) != NULL) {
            sprintf(info, "%sInput: %s\n", info, avio_enum_protocols((void **) p_temp, 0));
        }
        pup = NULL;
        avio_enum_protocols((void **) p_temp, 1);
        while ((*p_temp) != NULL) {
            sprintf(info, "%sInput: %s\n", info, avio_enum_protocols((void **) p_temp, 1));
        }

        return env->NewStringUTF(info);
    }

    JNIEXPORT jstring JNICALL
    Java_com_module_media_FFmpeg_getAvformat(JNIEnv *env, jclass clazz) {
        char info[40000] = {0};

        av_register_all();

        AVInputFormat *if_temp = av_iformat_next(NULL);
        AVOutputFormat *of_temp = av_oformat_next(NULL);
        while (if_temp != NULL) {
            sprintf(info, "%sInput: %s\n", info, if_temp->name);
            if_temp = if_temp->next;
        }
        while (of_temp != NULL) {
            sprintf(info, "%sOutput: %s\n", info, of_temp->name);
            of_temp = of_temp->next;
        }
        return env->NewStringUTF(info);
    }

    JNIEXPORT jstring JNICALL
    Java_com_module_media_FFmpeg_getAvCodec(JNIEnv *env, jclass clazz) {
        char info[40000] = {0};

        av_register_all();

        AVCodec *c_temp = av_codec_next(NULL);

        while (c_temp != NULL) {
            if (c_temp->decode != NULL) {
                sprintf(info, "%sdecode:", info);
            } else {
                sprintf(info, "%sencode:", info);
            }
            switch (c_temp->type) {
                case AVMEDIA_TYPE_VIDEO:
                    sprintf(info, "%s(video):", info);
                    break;
                case AVMEDIA_TYPE_AUDIO:
                    sprintf(info, "%s(audio):", info);
                    break;
                default:
                    sprintf(info, "%s(other):", info);
                    break;
            }
            sprintf(info, "%s[%10s]\n", info, c_temp->name);
            c_temp = c_temp->next;
        }

        return env->NewStringUTF(info);
    }

    JNIEXPORT jstring JNICALL
    Java_com_module_media_FFmpeg_getAvFilter(JNIEnv *env, jclass clazz) {
        char info[40000] = {0};
        avfilter_register_all();

        AVFilter *f_temp = (AVFilter *) avfilter_next(NULL);
        while (f_temp != NULL) {
            sprintf(info, "%s%s\n", info, f_temp->name);
            f_temp = f_temp->next;
        }
        return env->NewStringUTF(info);
    }

    JNIEXPORT jstring JNICALL
    Java_com_module_media_FFmpeg_getConfiguration(JNIEnv *env, jclass clazz) {
        char info[10000] = {0};

        sprintf(info, "%s\n", avcodec_configuration());


        return env->NewStringUTF(info);
    }

    void ffmpegInit();

    int  openInputFileVideo(const char * input_file_name);
    int  openInputFileImage(const char * input_file_name);

    int  initInputVideoDecodeCodec();
    int  initInputImageDecodeCodec();

    int  initOutPutVideoDecodeCodec(int w, int h);
    int  openOutPutFileVideo(const char * input_file_name);

    int initInputVideoFilter(AVFilterInOut *input,const char *filterName,  int inputIndex);
    int InitOutputFilter(AVFilterInOut *output,const char *filterName);
    void FreeInout();
    std::shared_ptr<AVPacket> ReadPacketFromSource(int inputIndex);
bool DecodeVideo(AVPacket* packet, AVFrame* frame,int inputIndex);
    void closeInputVideo();
    void closeInputImage();
    void CloseOutput();

    JNIEXPORT jboolean JNICALL
    Java_com_module_media_FFmpeg_setWaterMaskToVideo(JNIEnv *env, jclass clazz,
                                                     jstring input_video_path,
                                                     jstring int_water_mask_picture_path, jint x,
                                                     jint y, jstring output_video_path) {

        LOGE(kTAG,"setWaterMaskToVideo start");
        //1.注册组件
        ffmpegInit();
        LOGE(kTAG,"1.ffmpegInit");

        const char * c_input_video_path = env->GetStringUTFChars(input_video_path,NULL);
        const char * c_water_mask_picture_Path = env->GetStringUTFChars(int_water_mask_picture_path,NULL);
        const char * c_output_video_path = env->GetStringUTFChars(output_video_path,NULL);

        LOGE(kTAG,"param is %s %s %d %d %d ",c_input_video_path,c_water_mask_picture_Path,x,y,c_output_video_path);
        //2.打开文件
    openInputFileVideo(c_input_video_path);
    openInputFileImage(c_water_mask_picture_Path);

        //3.初始化解码器
    initInputVideoDecodeCodec();
    initInputImageDecodeCodec();

        //4.编码器
        int ret = initOutPutVideoDecodeCodec(input_video_av_codec_cxt->width,
                                             input_video_av_codec_cxt->height);
        if(ret < 0){
            LOGE(kTAG," initOutPutVideoEncoder failed ");
            return JNI_FALSE;
        }

        //5.打开文件
        ret = openOutPutFileVideo(c_output_video_path);
        if(ret < 0){
            LOGE(kTAG," openOutPutVideo failed ");
            return JNI_FALSE;
        }

        AVFrame *pSrcFrame[2];
        pSrcFrame[0] = av_frame_alloc();
        pSrcFrame[1] = av_frame_alloc();

        AVFrame *inputFrame[2];
        inputFrame[0] = av_frame_alloc();
        inputFrame[1] = av_frame_alloc();
        auto  filterFrame = av_frame_alloc();
        int got_output = 0;
        int64_t  timeRecord = 0;
        int64_t  firstPacketTime = 0;
        int64_t outLastTime = av_gettime();
        int64_t inLastTime = av_gettime();
        int64_t videoCount = 0;

        av_filter_graph = avfilter_graph_alloc();
        if(!av_filter_graph)
        {
            LOGE(kTAG," avfilter_graph_alloc failed ");
            return JNI_FALSE;
        }

        avfilter_graph_parse2(av_filter_graph, filter_descr, &inputs, &outputs);

        //6.打开输入filter
        initInputVideoFilter(inputs,"MainFrame",0);
        initInputVideoFilter(inputs->next,"OverlayFrame",1);
        InitOutputFilter(outputs,"output");

        ret = avfilter_graph_config(av_filter_graph,NULL);
        if(ret<0){
            LOGE(kTAG,"avfilter_graph_config  failed ");
            return JNI_FALSE;
        }

        bool ret1 = true;
        while(ret1)
        {
            auto packet = ReadPacketFromSource(1);
            ret1 = DecodeVideo(packet.get(),pSrcFrame[1],1);
            if(ret1) break;
        }

        while(true)
        {
            outLastTime = av_gettime();
            auto packet = ReadPacketFromSource(0);
            if(packet)
            {
                if(DecodeVideo(packet.get(),pSrcFrame[0],0))
                {
                    av_frame_ref( inputFrame[0],pSrcFrame[0]);
                    if (av_buffersrc_add_frame_flags(inputFilterContext[0], inputFrame[0], AV_BUFFERSRC_FLAG_PUSH) >= 0)
                    {
                        pSrcFrame[1]->pts = pSrcFrame[0]->pts;
                        //av_frame_ref( inputFrame[1],pSrcFrame[1]);
                        if(av_buffersrc_add_frame_flags(inputFilterContext[1],pSrcFrame[1], AV_BUFFERSRC_FLAG_PUSH) >= 0)
                        {
                            ret = av_buffersink_get_frame_flags(outputFilterContext, filterFrame,AV_BUFFERSINK_FLAG_NO_REQUEST);

//                            this_thread::sleep_for(chrono::milliseconds(10));
                            if ( ret >= 0)
                            {
                                std::shared_ptr<AVPacket> pTmpPkt(static_cast<AVPacket*>(av_malloc(sizeof(AVPacket))), [&](AVPacket *p) { av_packet_free(&p); av_freep(&p); });
                                av_init_packet(pTmpPkt.get());
                                pTmpPkt->data = NULL;
                                pTmpPkt->size = 0;
                                ret = avcodec_encode_video2(out_video_av_codec_cxt, pTmpPkt.get(), filterFrame, &got_output);
                                if (ret >= 0 && got_output)
                                {
                                    int ret = av_write_frame(output_video_av_fmt_cxt, pTmpPkt.get());
                                }
                                //this_thread::sleep_for(chrono::milliseconds(10));
                            }
                            av_frame_unref(filterFrame);
                        }
                    }
                }
            }
            else break;
        }


        closeInputVideo();
        closeInputImage();
        CloseOutput();

        env->ReleaseStringUTFChars(input_video_path,c_input_video_path);
        env->ReleaseStringUTFChars(int_water_mask_picture_path,c_water_mask_picture_Path);
        env->ReleaseStringUTFChars(output_video_path,c_output_video_path);

        LOGE(kTAG,"setWaterMaskToVideo end");

        return JNI_TRUE;
    }

    void ffmpegInit(){
        av_register_all();
        avcodec_register_all();
        avfilter_register_all();
    }

    //包含打开文件和初始化解码器
    int openInputFileVideo(const char * input_file_name){
        input_video_av_fmt_cxt = avformat_alloc_context();
        int ret = avformat_open_input(&input_video_av_fmt_cxt, input_file_name, NULL, NULL);
        if(ret < 0){
            LOGE(kTAG,"can not open file");
            return ret;
        }

        ret = avformat_find_stream_info(input_video_av_fmt_cxt, NULL);
        if(ret<0){
            LOGE(kTAG,"can not find stream info");
            return ret;
        }

        AVCodec *avCodec;
        ret = av_find_best_stream(input_video_av_fmt_cxt,AVMEDIA_TYPE_VIDEO,-1,-1,&avCodec,0);
        if(ret < 0){
            LOGE(kTAG,"can not find video steam");
            return ret;
        }
        input_video_stream_idx = ret;
        input_video_av_codec_cxt = input_video_av_fmt_cxt->streams[input_video_stream_idx]->codec;

        ret = avcodec_open2(input_video_av_codec_cxt,avCodec,NULL);
        if(ret < 0){
            LOGE(kTAG,"can not open video decoder");
            return ret;
        }
        return ret;
    }

    int openInputFileImage(const char * input_file_name){
        input_image_av_fmt_cxt = avformat_alloc_context();
        int ret = avformat_open_input(&input_image_av_fmt_cxt, input_file_name, NULL, NULL);
        if(ret < 0){
            LOGE(kTAG,"openInputImage can not open file");
            return ret;
        }

        ret = avformat_find_stream_info(input_image_av_fmt_cxt, NULL);
        if(ret<0){
            LOGE(kTAG,"openInputImage can not find stream info");
            return ret;
        }

        AVCodec *avCodec;
        ret = av_find_best_stream(input_image_av_fmt_cxt, AVMEDIA_TYPE_VIDEO, -1, -1, &avCodec, 0);
        if(ret < 0){
            LOGE(kTAG,"can not find image steam");
            return ret;
        }
        input_image_stream_idx = ret;
        input_image_av_codec_cxt = input_image_av_fmt_cxt->streams[input_image_stream_idx]->codec;

        ret = avcodec_open2(input_image_av_codec_cxt, avCodec, NULL);
        if(ret < 0){
            LOGE(kTAG,"can not open image decoder");
            return ret;
        }
        return ret;
    }

    int initInputVideoDecodeCodec(){
        auto codec = avcodec_find_decoder(input_video_av_codec_cxt->codec->id);
        if(!codec){
            LOGE(kTAG,"initVideoDecoder avcodec_find_decoder filed");
            return -1;
        }

        return 0;
    }

    int initInputImageDecodeCodec(){
        auto codec = avcodec_find_decoder(input_image_av_codec_cxt->codec->id);
        if(!codec){
            LOGE(kTAG,"initImageDecoder avcodec_find_decoder filed");
            return -1;
        }

        return 0;
    }

    int initOutPutVideoDecodeCodec(int w, int h){
        AVCodec * pH264Codec = avcodec_find_decoder(AV_CODEC_ID_MPEG4);
        if(!pH264Codec){
            LOGE(kTAG,"avcodec_find_decoder filed");
            return -1;
        }

        out_video_av_codec_cxt = avcodec_alloc_context3(pH264Codec);
        out_video_av_codec_cxt->gop_size = 30;
        out_video_av_codec_cxt->has_b_frames = 0;
        out_video_av_codec_cxt->max_b_frames = 0;
        out_video_av_codec_cxt->codec_id = pH264Codec->id;
        out_video_av_codec_cxt->time_base.num = input_video_av_fmt_cxt->streams[input_video_stream_idx]->codec->time_base.num;
        out_video_av_codec_cxt->time_base.den = input_video_av_fmt_cxt->streams[input_video_stream_idx]->codec->time_base.den;
//        out_video_av_codec_cxt->pix_fmt = *pH264Codec->pix_fmts;
        out_video_av_codec_cxt->width = w;
        out_video_av_codec_cxt->height = h;
        out_video_av_codec_cxt->me_subpel_quality = 0;
        out_video_av_codec_cxt->refs = 1;
        out_video_av_codec_cxt->scenechange_threshold = 0;
        out_video_av_codec_cxt->trellis = 0;
        out_video_av_codec_cxt->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        AVDictionary * options = NULL;
        int ret = avcodec_open2(out_video_av_codec_cxt,pH264Codec,&options);
        if(ret < 0){
            LOGE(kTAG," open codec failed");
            return ret;
        }
        return ret;
    }

    int openOutPutFileVideo(const char * outPutFileName){
        int ret = avformat_alloc_output_context2(&output_video_av_fmt_cxt, NULL, "mpegts", input_file_name);
        if(ret < 0){
            LOGE(kTAG," avformat_alloc_output_context2 failed");
            return ret;
        }

        ret = avio_open2(&output_video_av_fmt_cxt->pb, input_file_name, AVIO_FLAG_READ_WRITE, NULL, NULL);
        if(ret < 0){
            LOGE(kTAG," avio_open2 failed");
            return ret;
        }

        AVStream * videoStream = avformat_new_stream(output_video_av_fmt_cxt,out_video_av_codec_cxt->codec);
        videoStream->codec = out_video_av_codec_cxt;

        AVStream * audioStream = avformat_new_stream(output_video_av_fmt_cxt,out_video_av_codec_cxt->codec);
        audioStream->codec = out_video_av_codec_cxt;

        ret = avformat_write_header(output_video_av_fmt_cxt,NULL);
        if(ret < 0){
            LOGE(kTAG," avformat_write_header failed");
            return ret;
        }
        return  ret;
    }

    int initInputVideoFilter(AVFilterInOut *input,const char *filterName,  int inputIndex)
    {
        char args[512];
        memset(args,0, sizeof(args));
        AVFilterContext *padFilterContext = input->filter_ctx;

        auto filter = avfilter_get_by_name("buffer");
//        auto codecContext = context[inputIndex]->streams[0]->codec;
        auto codecContext = inputIndex== 0 ? input_video_av_codec_cxt : input_image_av_codec_cxt;

        snprintf(args, sizeof(args),
                  "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                  codecContext->width, codecContext->height, codecContext->pix_fmt,
                  codecContext->time_base.num, codecContext->time_base.den /codecContext->ticks_per_frame,
                  codecContext->sample_aspect_ratio.num, codecContext->sample_aspect_ratio.den);

        int ret = avfilter_graph_create_filter(&inputFilterContext[inputIndex],filter,filterName, args,NULL, av_filter_graph);
        if(ret < 0)
            return ret;

        ret = avfilter_link(inputFilterContext[inputIndex],0,padFilterContext,input->pad_idx);
        return ret;
    }

int InitOutputFilter(AVFilterInOut *output,const char *filterName)
{
    AVFilterContext *padFilterContext = output->filter_ctx;
    auto filter = avfilter_get_by_name("buffersink");


    int ret = avfilter_graph_create_filter(&outputFilterContext,filter,filterName, NULL,
                                           NULL, av_filter_graph);
    if(ret < 0)  return ret;
    ret = avfilter_link(padFilterContext,output->pad_idx,outputFilterContext,0);

    return ret;
}

void FreeInout()
{
    avfilter_inout_free(&inputs->next);
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
}

    void closeInputVideo(){
        if(input_video_av_fmt_cxt){
            avformat_close_input(&input_video_av_fmt_cxt);
        }
        input_video_av_fmt_cxt = NULL;
    }

    void closeInputImage(){
        if(input_image_av_fmt_cxt){
            avformat_close_input(&input_image_av_fmt_cxt);
        }
        input_image_av_fmt_cxt = NULL;
    }

void CloseOutput()
{
    if(output_video_av_fmt_cxt != nullptr)
    {
        for(int i = 0 ; i < output_video_av_fmt_cxt->nb_streams; i++)
        {
            AVCodecContext *codecContext = output_video_av_fmt_cxt->streams[i]->codec;
            avcodec_close(codecContext);
        }
        avformat_close_input(&output_video_av_fmt_cxt);
    }
}

std::shared_ptr<AVPacket> ReadPacketFromSource(int inputIndex)
{
    std::shared_ptr<AVPacket> packet(static_cast<AVPacket*>(av_malloc(sizeof(AVPacket))), [&](AVPacket *p) { av_packet_free(&p); av_freep(&p); });
    av_init_packet(packet.get());
    int ret = av_read_frame(inputIndex == 0 ? input_video_av_fmt_cxt : input_image_av_fmt_cxt, packet.get());
    if(ret >= 0)
    {
        return packet;
    }
    else
    {
        return nullptr;
    }
}

bool DecodeVideo(AVPacket* packet, AVFrame* frame,int inputIndex)
{
    int gotFrame = 0;
    auto hr = avcodec_decode_video2(inputIndex == 0 ? input_video_av_codec_cxt : input_image_av_codec_cxt, frame, &gotFrame, packet);
    if(hr >= 0 && gotFrame != 0)
    {
        frame->pts = packet->pts;
        return true;
    }
    return false;
}

}