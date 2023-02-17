#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

#include "PT_Image.h"

    // std::vector<uint8_t> pixels;
    // //uint8_t* pixels = NULL;
    // size_t size = 0;
    // int width;
    // int height;
    // int numberOfChannels;


    PT_Image::PT_Image(const char* filename)
    {
        // do a check that the file exists or something
        if (read_image_file(filename))
        {
            size = width * height * numberOfChannels;
            printf("Success");
        }
        else { printf("didnt work"); }
    }

    // Init all black image of width x height x num_channels
    PT_Image::PT_Image(int init_width, int init_height, int init_channels)
    : width(init_width), height(init_height), numberOfChannels(init_channels)
    {
        size = width * height * numberOfChannels;
        pixels.resize(size); 
    }

    // Initialize as copy of input image
    PT_Image::PT_Image(const PT_Image& image)
    {
        int i = 1;
    }

    PT_Image::~PT_Image()
    {
        //stbi_image_free(pixels);
    }

    bool PT_Image::read_image_file(const char* filename)
    {
        uint8_t *bucket = stbi_load(filename, &width, &height, &numberOfChannels, 0);
        printf("%d\n", width);
        return bucket != NULL;
    }








    int PT_Image::color_to_16(int r, int g, int b, int mode)
    {

        RGBA black =  {  0,   0,   0, 0};
        RGBA maroon = {128,   0,   0, 0};
        RGBA green =  {  0, 128,   0, 0};
        RGBA olive =  {128, 128,   0, 0};
        RGBA navy =   {  0,   0, 128, 0};
        RGBA purple = {128,   0, 128, 0};
        RGBA teal =   {  0, 128, 128, 0};
        RGBA silver = {192, 192, 192, 0};

        RGBA grey =        {  0,   0,   0, 0};
        RGBA lightRed =    {255,   0,   0, 0};
        RGBA lightGreen =  {  0, 255,   0, 0};
        RGBA yellow =      {255, 255,   0, 0};
        RGBA lightBlue =   {  0,   0, 255, 0};
        RGBA lightPurple = {255,   0, 255, 0};
        RGBA lightCyan =   {  0, 255, 255, 0};
        RGBA highWhite =   {255, 255, 255, 0};

        RGBA colors[16];
        colors[0] = black;
        colors[1] = maroon;
        colors[2] = green;
        colors[3] = olive;
        colors[4] = navy;
        colors[5] = purple;
        colors[6] = teal;
        colors[7] = silver;

        colors[8] = grey;
        colors[9] = lightRed;
        colors[10] = lightGreen;
        colors[11] = yellow;
        colors[12] = lightBlue;
        colors[13] = lightPurple;
        colors[14] = lightCyan;
        colors[15] = highWhite;

        int st = 0, end = 8;					// 8 colors dark
        if (mode == 1) { st = 8, end = 16; }	// 8 colors bright
        if (mode == 2) { end = 16; }			// 16 colors


        // Simple loop version
        int color_num = 0;
        int col_dist = 1000;

        for (int i = st; i < end; i++)
        {
            int dist = color_distance(r, g, b, colors[i].R, colors[i].G, colors[i].B);
            if (dist < col_dist)
            {
                color_num = i;
                col_dist = dist;
            }
        }

        return color_num;
    }

    int PT_Image::color_to_256(int r, int g, int b)
    {
        int den = 216;

        r = (r * 5/den);
        g = (g * 5/den);
        b = (b * 5/den);

        int color = 16 + 36 * r + 6 * g + b;

        return color;
    }

    int PT_Image::color_to_gray(int r, int g, int b)
    {
        double rG = linearize_sRGB(r);
        double gG = linearize_sRGB(g);
        double bG = linearize_sRGB(b);

        double Y = (0.2126 * rG) + (0.7152 * gG) + (0.0722 * bG);
        double L;
        if (Y <= (216.0/24389.0))
        {
            L = Y * (24389.0/27.0);
        }
        else
        {
            L = (pow(Y, (1.0/3)) * 116) - 16;
        }
            
        return map_to_range(L, 0, 100, 232, 255);
    }

    int PT_Image::color_distance(int r1, int g1, int b1, int r2, int g2, int b2)
    {
        long r_mean = ((long)r1 + (long)r2)/2;
        long rD = (long)r1 - (long)r2;
        long gD = (long)g1 - (long)g2;
        long bD = (long)b1 - (long)b2;

        return sqrt(
            ( (2 + (r_mean / 256) ) * (rD * rD) ) +
            (4 * (gD * gD)) +
            ((2 + ((255 - r_mean) / 256)) * (bD * bD))
            );
    }

    // RGBA PT_Image::color_to_True(int r, int g, int b)
    // {
    //      return rgb_to_struct(r,g,b);
    // }
    // RGBA PT_Image::rgb_to_struct(int r, int g, int b)
    // {

        //     RGBA st;
        //     st.R = (unsigned char)r;
        //     st.G = (unsigned char)g;
        //     st.B = (unsigned char)b;
        //     st.A = 0;
            
        //     return st;
        // }

    int PT_Image::map_to_range(int value, int a1, int a2, int b1, int b2)
    {
        double slope = 1.0 * (b2 - b1) / (a2 - a1);
        double output  = b1 + (slope * (value - a1));
        return (int)output;
    }

    double PT_Image::linearize_sRGB(int val)
    {

        double dec = ((1.0 * val)/255.0);

        if ( dec <= 0.04045)
        {
            return dec / 12.92;
        }
        return pow((dec + 0.055)/1.055, 2.4);
    }