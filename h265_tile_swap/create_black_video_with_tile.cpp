
// The application take an input yuv file and its resolution and creates
// an output yuv file of output resolution with black video and the input
// yuv overlaid on the black video 

// TO BUILD: g++ create_black_video_with_tile.cpp -o create_black_video_with_tile

#include <iostream>
#include <cstdlib>

#define USAGE "./create_black_video_with_file <inp_res(e.g.128x128)> <input_yuv_file> <out_coord(e.g. 1792x0)> <out_res(e.g.1920x1080)> <out_file>"

int main(int argc, char *argv[]) {
    if(argc < 6) {
        std::cout << USAGE << std::endl;
        exit(1);
    }

    char *inp_res = argv[1];
    char *inp_yuv = argv[2];
    char *out_coord = argv[3];
    char *out_res = argv[4];
    char *out_yuv = argv[5];

    int inp_res_width;
    int inp_res_height;
    int out_res_width;
    int out_res_height;
    int out_coord_x;
    int out_coord_y;

    char *token;
    char *rest;

    FILE *inp_yuv_file;
    FILE * out_yuv_file;

    unsigned char *inp_yuv_buffer;
    unsigned char *out_yuv_buffer;

    unsigned char *loc_inp_buf_y;
    unsigned char *loc_inp_buf_u;
    unsigned char *loc_inp_buf_v;

    unsigned char *loc_out_buf_y;
    unsigned char *loc_out_buf_u;
    unsigned char *loc_out_buf_v;


    rest = inp_res;
    token = strtok_r(rest, "x", &rest);
    inp_res_width = atoi(token);
    token = strtok_r(NULL, "x", &rest);
    inp_res_height = atoi(token);

    rest = out_res;
    token = strtok_r(rest, "x", &rest);
    out_res_width = atoi(token);
    token = strtok_r(NULL, "x", &rest);
    out_res_height = atoi(token);

    rest = out_coord;
    token = strtok_r(rest, "x", &rest);
    out_coord_x = atoi(token);
    token = strtok_r(NULL, "x", &rest);
    out_coord_y = atoi(token);

    inp_yuv_file = fopen(inp_yuv, "rb");
    if (inp_yuv_file == NULL) {
        std::cout << "ERROR: input file " << inp_yuv << " not found" << std::endl;
        exit(1);
    }
    out_yuv_file = fopen(out_yuv, "wb");
    if (out_yuv_file == NULL) {
        std::cout << "ERROR: Could not create output file " << out_yuv << std::endl;
        exit(1);
    }

    std::cout << "inp_res_width:" << inp_res_width << std::endl;
    std::cout << "inp_res_height:" << inp_res_height << std::endl;
    std::cout << "out_res_width:" << out_res_width << std::endl;
    std::cout << "out_res_height:" << out_res_height << std::endl;

    std::cout << "out_coord_x:" << out_coord_x << std::endl;
    std::cout << "out_coord_y:" << out_coord_y << std::endl;


    inp_yuv_buffer = new unsigned char[inp_res_height * inp_res_width * 3];
    out_yuv_buffer = new unsigned char[out_res_height * out_res_width * 3];

    // Initialize out_yuv_buffer to black
    memset(out_yuv_buffer, 16, out_res_height * out_res_width);
    memset((out_yuv_buffer + (out_res_width * out_res_height)), 128, (out_res_width * out_res_height) >> 1);

    int frame_num = 0;
    int fread_res;
    while (1) {
        fread_res = fread(inp_yuv_buffer, 1, (inp_res_height * inp_res_width * 3) >> 1, inp_yuv_file);

        if (fread_res == 0) {
            std::cout << "All frames processed. Quitting" << std::endl;
            break;
        }

        loc_out_buf_y = out_yuv_buffer + (out_coord_y * out_res_height) + out_coord_x;
        loc_out_buf_u = out_yuv_buffer + (out_res_width * out_res_height) + ((out_coord_y * out_res_height)>>2) + (out_coord_x >> 1);
        loc_out_buf_v = loc_out_buf_u + ((out_res_width * out_res_height) >> 2);

        loc_inp_buf_y = inp_yuv_buffer;
        loc_inp_buf_u = inp_yuv_buffer + (inp_res_height * inp_res_width);
        loc_inp_buf_v = loc_inp_buf_u + ((inp_res_height * inp_res_width) >> 2);
        for(int i = 0; i < inp_res_height; i++) {
            memcpy(loc_out_buf_y, loc_inp_buf_y, inp_res_width);
            loc_out_buf_y += out_res_width;
            loc_inp_buf_y += inp_res_width;
        }
        for(int i = 0; i < (inp_res_height >> 1); i++) {
            memcpy(loc_out_buf_u, loc_inp_buf_u, (inp_res_width >> 1));
            memcpy(loc_out_buf_v, loc_inp_buf_v, (inp_res_width >> 1));
            
            loc_out_buf_u += (out_res_width >> 1);
            loc_out_buf_v += (out_res_width >> 1);            
            loc_inp_buf_u += (inp_res_width >> 1);
            loc_inp_buf_v += (inp_res_width >> 1);
        }
        std::cout << "Writing frame:" << frame_num++ << std::endl;
        fwrite(out_yuv_buffer, (out_res_width * out_res_height * 3) >> 1, 1, out_yuv_file);
    }
    fclose(out_yuv_file);
    fclose(inp_yuv_file);

}