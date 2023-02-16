#include "PT_Image.h"
#include <vector>

struct Projector
{
    PT_Image frame;
    std::vector<PT_Image> layers;

    Projector();
    ~Projector();

    bool display_frame();
    bool copyImageToLayer(PT_Image image);

};
