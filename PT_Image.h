#include <cstdint>
#include <cstdio>
#include <vector>

struct PT_Image {
    std::vector<uint8_t> pixels;
    //uint8_t* pixels = NULL;
    size_t size = 0;
    int width;
    int height;
    int numberOfChannels;

    PT_Image(const char* filename);
    PT_Image(int init_width, int init_height, int init_channels);
    PT_Image(const PT_Image& image);
    ~PT_Image();

    bool read_image_file(const char* filename);

}; 