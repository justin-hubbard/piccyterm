#include <cstdint>
#include <cstdio>
#include <vector>

struct RGBA
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

struct TermColor
{
    uint8_t color;
    uint8_t alpha;
};

struct Pair2D
{
	int x;
	int y;
};

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


    // Color functions
    int color_to_16(int r, int g, int b, int mode);
    int color_to_256(int r, int g, int b);
    int color_to_gray(int r, int g, int b);
    int color_distance(int r1, int g1, int b1, int r2, int g2, int b3);
   // RGBA color_to_True(int r, int g, int b);
    //RGBA rgb_to_struct(int r, int g, int b);
    int map_to_range(int value, int a1, int a2, int b1, int b2);
    double linearize_sRGB(int val);

}; 