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