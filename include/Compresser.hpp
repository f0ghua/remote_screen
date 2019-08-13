/*
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期二 06/08/2019 16:35.
 * Last Revision: 星期二 06/08/2019 16:35.
 */                     

#ifndef COMPRESSER_H
#define COMPRESSER_H

#include <opencv2/opencv.hpp>
#include <iostream>

using std::cout;
using std::endl;

#define PART_X 8
#define PART_Y 8

enum CompressType{
    PARTS_MOTION_JPEG = 0,
    PARTS_MOTION_GRAY_JPEG = 1,
};

class Compresser
{
private:
    CompressType m_type;
    int m_height, m_width;
    uchar* last_frame;
    int channel_list[16];


    void parts_motion_jpeg_gray(uchar* compressed, int* size, const uchar* input);
    void parts_motion_jpeg(uchar* compressed, int* size, const uchar* input);

public:
    Compresser(const int height, const int width, const CompressType type);
    ~Compresser();
    void compress(uchar* compressed, int* size, const uchar* input);
    void init();
};


class Decompresser
{
private:
    CompressType m_type;
    int m_height, m_width;
    uchar* last_frame;
    int channel_list[16];


    void parts_motion_jpeg_gray(uchar* output, const uchar* input);
    void parts_motion_jpeg(uchar* output, const uchar* input);

public:
    Decompresser(const int height, const int width, const CompressType type);
    ~Decompresser();
    void decompress(uchar* output, const uchar* input);
};
#endif   /* End of COMPRESSER.H */




