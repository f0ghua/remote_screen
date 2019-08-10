/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期二 06/08/2019 16:55.
 * Last Revision: 星期二 06/08/2019 16:55.
 */

#include <Compresser.hpp>

void Compresser::parts_motion_jpeg(uchar* compressed, int* size, const uchar* input){
    const int x_pl = m_width / PART_X;
    const int y_pl = m_height / PART_Y;
    bool *mark = new bool[PART_X * PART_Y];
    memset(mark, 0, PART_X*PART_Y);

    *size = 0;

    for(int i=0;i<m_height;i++){
        int y_ind = i/y_pl;
        for(int j=0;j<PART_X;j++){
            if(mark[y_ind*PART_X + j]) continue;
            for(int s=j*x_pl;s<(j+1)*x_pl;s++){
                for(int c=0;c<3;c++)
                    if(input[i*m_width*3 + s*3 + c] != last_frame[i*m_width*3 + s*3 +c]){
                        mark[y_ind*PART_X + j] = true;
                        break;
                    }
            }
        }
    }
    memcpy(compressed+*size, mark, PART_X*PART_Y);
    *size += PART_X * PART_Y;
    int sum = 0;
    for(int i=0;i<PART_X*PART_Y;i++){
        if(!mark[i]) continue;
        int x = (i%PART_X)*x_pl;
        int y = (i/PART_X)*y_pl;
        if(mark[i]) sum ++;
        cv::Mat mat_part(y_pl, x_pl, CV_8UC3);
        for(int j=0;j<y_pl;j++){
            for(int k=0;k<x_pl;k++){
                for(int c=0;c<3;c++)
                    mat_part.at<cv::Vec3b>(j, k)[c] = input[(y+j)*m_width*3 + (x+k)*3 + c];// - last_frame[(y+j)*m_width +x+k];
            }

        }
        std::vector<uchar> data;
        std::vector<int> params;
        params.push_back(cv::IMWRITE_JPEG_QUALITY);
        params.push_back(40);
        cv::imencode(".jpg", mat_part, data, params);
        int sz = data.size();
        memcpy(compressed+*size, &sz, sizeof(int));
        *size += sizeof(int);
        memcpy(compressed+*size, &(data[0]), sz);
        *size += sz;
    }
    //cout<<sum<<endl;
    memcpy(last_frame, input, m_width*m_height*3);
    delete mark;
}

void Compresser::parts_motion_jpeg_gray(uchar* compressed, int* size, const uchar* input){
    const int x_pl = m_width / PART_X;
    const int y_pl = m_height / PART_Y;
    bool *mark = new bool[PART_X * PART_Y];
    memset(mark, 0, PART_X*PART_Y);

    *size = 0;

    for(int i=0;i<m_height;i++){
        int y_ind = i/y_pl;
        for(int j=0;j<PART_X;j++){
            if(mark[y_ind*PART_X + j]) continue;
            for(int s=j*x_pl;s<(j+1)*x_pl;s++){
                if(input[i*m_width + s] != last_frame[i*m_width + s]){
                    mark[y_ind*PART_X + j] = true;
                    break;
                }
            }
        }
    }
    memcpy(compressed+*size, mark, PART_X*PART_Y);
    *size += PART_X * PART_Y;
    int sum = 0;
    for(int i=0;i<PART_X*PART_Y;i++){
        if(!mark[i]) continue;
        int x = (i%PART_X)*x_pl;
        int y = (i/PART_X)*y_pl;
        if(mark[i]) sum ++;
        cv::Mat mat_part(y_pl, x_pl, CV_8UC1);
        for(int j=0;j<y_pl;j++){
            for(int k=0;k<x_pl;k++){
                mat_part.at<uchar>(j, k) = input[(y+j)*m_width + x+k];// - last_frame[(y+j)*m_width +x+k];
            }

        }
        std::vector<uchar> data;
        std::vector<int> params;
        params.push_back(cv::IMWRITE_JPEG_QUALITY);
        params.push_back(40);
        cv::imencode(".jpg", mat_part, data, params);
        int sz = data.size();
        memcpy(compressed+*size, &sz, sizeof(int));
        *size += sizeof(int);
        memcpy(compressed+*size, &(data[0]), sz);
        *size += sz;
    }
    //cout<<sum<<endl;
    memcpy(last_frame, input, m_width*m_height);
    delete mark;
}

Compresser::Compresser(const int height=1080, const int width=1920, const CompressType type = PARTS_MOTION_GRAY_JPEG){
    m_height = height;
    m_width = width;
    m_type = type;

    assert(m_width%PART_X == 0);
    assert(m_height%PART_Y == 0);

    last_frame = new uchar[width*height*3];
    memset(last_frame, 0, width*height*3);

    channel_list[PARTS_MOTION_JPEG] = 3;
    channel_list[PARTS_MOTION_GRAY_JPEG] = 1;
}

Compresser::~Compresser(){
    delete last_frame;
}

void Compresser::compress(uchar* compressed, int* size, const uchar* input){
    void (Compresser::* kernel)(uchar*, int*, const uchar*);
    switch(m_type){
        case PARTS_MOTION_JPEG: kernel = &Compresser::parts_motion_jpeg; break;
        case PARTS_MOTION_GRAY_JPEG: kernel = &Compresser::parts_motion_jpeg_gray; break;
        default: break;
    }
    (this->*kernel)(compressed, size, input);
}

Decompresser::Decompresser(const int height=1080, const int width=1920, const CompressType type = PARTS_MOTION_GRAY_JPEG){
    m_height = height;
    m_width = width;
    m_type = type;

    assert(m_width%PART_X == 0);
    assert(m_height%PART_Y == 0);

    last_frame = new uchar[width*height*3];
    memset(last_frame, 0, width*height*3);

    channel_list[PARTS_MOTION_JPEG] = 3;
    channel_list[PARTS_MOTION_GRAY_JPEG] = 1;
}

Decompresser::~Decompresser(){
    delete last_frame;
}

void Decompresser::decompress(uchar* output, const uchar* input){
    void (Decompresser::* kernel)(uchar*, const uchar*);
    switch(m_type){
        case PARTS_MOTION_JPEG: kernel = &Decompresser::parts_motion_jpeg; break;
        case PARTS_MOTION_GRAY_JPEG: kernel = &Decompresser::parts_motion_jpeg_gray; break;
        default: break;
    }
    (this->*kernel)(output, input);
}


void Decompresser::parts_motion_jpeg_gray(uchar* output, const uchar* input){
    const int x_pl = m_width / PART_X;
    const int y_pl = m_height / PART_Y;
    bool* mark = new bool[PART_X*PART_Y];
    int offset = 0;
    memcpy(mark, input+offset, PART_X*PART_Y);
    offset += PART_X*PART_Y;
    int len;
    for(int i=0;i<PART_X*PART_Y;i++){
        if(!mark[i]) continue;
        memcpy(&len, input+offset, sizeof(int));
        offset += sizeof(int);
        cv::Mat jpg(1, len, CV_8UC1);
        memcpy(jpg.data, input+offset, len);
        offset += len;
        cv::Mat out = cv::imdecode(jpg, CV_LOAD_IMAGE_GRAYSCALE);
        //cv::imshow("interm", out);
        //cv::waitKey(1);
        int x = (i%PART_X)*x_pl;
        int y = (i/PART_X)*y_pl;
        for(int j=0;j<y_pl;j++){
            for(int k=0;k<x_pl;k++){
                last_frame[(y+j)*m_width + x+k] = out.at<uchar>(j, k);
            }
        }
    }
    memcpy(output, last_frame, m_width*m_height);

    delete mark;

}

void Decompresser::parts_motion_jpeg(uchar* output, const uchar* input){
    const int x_pl = m_width / PART_X;
    const int y_pl = m_height / PART_Y;
    bool* mark = new bool[PART_X*PART_Y];
    int offset = 0;
    memcpy(mark, input+offset, PART_X*PART_Y);
    offset += PART_X*PART_Y;
    int len;
    for(int i=0;i<PART_X*PART_Y;i++){
        if(!mark[i]) continue;
        memcpy(&len, input+offset, sizeof(int));
        offset += sizeof(int);
        cv::Mat jpg(1, len, CV_8UC3);
        memcpy(jpg.data, input+offset, len);
        offset += len;
        cv::Mat out = cv::imdecode(jpg, CV_LOAD_IMAGE_COLOR);
        //cv::imshow("interm", out);
        //cv::waitKey(1);
        int x = (i%PART_X)*x_pl;
        int y = (i/PART_X)*y_pl;
        for(int j=0;j<y_pl;j++){
            for(int k=0;k<x_pl;k++){
                for(int c=0;c<3;c++)
                    last_frame[(y+j)*m_width*3 + (x+k)*3 + c] = out.at<cv::Vec3b>(j, k)[c];
            }
        }
    }
    memcpy(output, last_frame, m_width*m_height*3);

    delete mark;

}

/* EOF */

