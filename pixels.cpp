// ▄ (U+2584) █ (U+2588), ▓ (U+2593), ▒ (U+2592), ░ (U+2591)


#include <curses.h>

#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>
#include <random>
#include <ctime>
#include <string>
#include <cstdlib>
//#include <fmt>

// #include "fmt/core.h"
// #include "fmt/format-inl.h"
// #include "fmt/format.h"

#include <fmt/core.h>
// #include <fmt/format-inl.h>
// #include <fmt/format.h>



#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
using namespace std;


#define ESC "\x1b"
#define ALT "[?1049h"

#define TEENY = "▄"


#define say(str) write(1, str, sizeof(str))


#define SINGLE_BLOCK 0
#define DOUBLE_BLOCK 1
#define LOWER_BLOCK 2
#define BRAILLE 3

// COLOR PALLETES
#define C8 0
#define C8_BRIGHT 1
#define C8_8 2
#define C_256 3 // 16 + 36r + 6g + b
#define C_TRUE 4

#define CUR_COLOR 2
#define IMG_SCALE 3


#define IMPATH "sf_ken.png"
#define CLOTH "clothes.jpg"

struct RGBA
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};


string get_pixel_string(int choice);
void blackout(void);
string get_pixel(unsigned char *bucket, int w, int x, int y, int num_channels, int mode);
string get_color(int r, int g, int b);
string color_to_8(int r, int g, int b);
string color_to_8bright(int r, int g, int b);
string color_to_8plus8(int r, int g, int b);
string color_to_16(int r, int g, int b, int mode);
string color_to_256(int r, int g, int b);
string color_to_24bit(int r, int g, int b);
int color_distance(int r1, int g1, int b1, int r2, int g2, int b3);
string color_to_True(int r, int g, int b);


int tg_loadImage(unsigned char* bucket, char* filename)
{

	return 0;
}

void print_line(unsigned char *start, int line_num, int width, int num_channels)
{
	for (int poo = 0; poo < width; poo++)
	{
		unsigned char *off = start + (poo + (width * 20)) * num_channels;
		printf("\033[48;2;%d;%d;%dm", off[0], off[1], off[2]);
		//printf("#%3d: R: %d, G: %d, B: %d, Alpha: %d\n", poo, off[0], off[1], off[2], off[3]);
		printf("  ");
	}
	printf("\n");
}

void tg_display_pixels_usc(unsigned char *bucket, int width, int height, int num_channels)
{
	for (int i = 0; i < height; i++)
	    {
	        for (int j = 0; j < width; j++)
	        {
	        	unsigned char *off = bucket + (j + width * i) * num_channels;

	        	unsigned char red = off[0];
	        	unsigned char green = off[1];
	        	unsigned char blue = off[2];
	        	unsigned char alpha = off[3];

	        	printf("\033[48;2;%d;%d;%dm", red, green, blue);
	        	//printf("%2d", j%99);
	        	printf("%s",get_pixel_string(1).c_str());
	        }
        	blackout();
	        printf("    line %d\n", i);
	    }
}

void tg_display_teeny_pixels(unsigned char *bucket, int width, int height, int num_channels)
{
	for (int row = 0; row < height; row += 2)
	{
		for (int col = 0; col < width; col++)
		{
			unsigned char *off = bucket + (col + (width * row)) * num_channels;

			unsigned char red = off[0];
	        unsigned char green = off[1];
	        unsigned char blue = off[2];
	        unsigned char alpha = off[3];

	        off += width * num_channels;
	        unsigned char red2 = off[0];
	        unsigned char green2 = off[1];
	        unsigned char blue2 = off[2];
	        unsigned char alpha2 = off[3];


	        printf("\033[48;2;%d;%d;%dm", red, green, blue);
	        printf("\033[38;2;%d;%d;%dm", red2, green2, blue2);
	        printf("▄");

		}
		printf("\n");
	}

}

string tg_dp_teeny_string(unsigned char *bucket, int w, int h, int num_channels)
{
	string image_string = "";

	for (int row = 0; row < h; row += 2)
	{
		for (int col = 0; col < w; col++)
		{
	        image_string.append(get_pixel(bucket, w, col, row, num_channels, 0));
		}
		// image_string.append("\n");
		image_string.append("\033[48;2;0;0;0m\n"); // Set to black so newline isn't colored
								// This should actually set it to "plain"
	}

	std::cout << image_string << endl;
	//fmt::print(image_string);
	return image_string;
}


// string get_pixel_row()

string get_pixel(unsigned char *bucket, int w, int x, int y, int num_channels, int mode)
{
	unsigned char *off = bucket + (x + (w * y)) * num_channels;
	unsigned char top_red =   	 off[0],
				  top_green = 	 off[1],
				  top_blue =  	 off[2];
				  

	// Single color only modes
	if (mode == 0)
	{
		return fmt::format("\033[48;{}m{}",
						get_color(top_red, top_green, top_blue),
						get_pixel_string(0));
	}
	// Two color modes
	if (mode == 1)
	{
		unsigned char bottom_red =   off[0 + (w * num_channels)],
					  bottom_green = off[1 + (w * num_channels)],
					  bottom_blue =  off[2 + (w * num_channels)];

		// teeny block square pixel mode
		return fmt::format("\033[48;{}m\033[38;{}m{}", 
						get_color(top_red, top_green, top_blue),
						get_color(bottom_red, bottom_green, bottom_blue),
						get_pixel_string(2));
	}


	// return fmt::format("\033[48;2;{};{};{}m\033[38;2;{};{};{}m▄",
	//         					top_red, top_green, top_blue,
	//         				 bottom_red, bottom_green, bottom_blue);
	// return fmt::format("\033[48;2;{};{};{}m\033[38;2;{};{};{}m▄",
	//         					top_red, top_green, top_blue,
	//         				 bottom_red, bottom_green, bottom_blue);
	
	// return fmt::format("\033[48;{}m ", get_color(top_red, top_green, top_blue));
}

string get_color(int r, int g, int b)
{
	int mode = CUR_COLOR;
	
	if (mode < 3) // 8 colors, 8 colors BRIGHT, 16 colors
	{ return color_to_16(r,g,b,mode); }
	if (mode == 3) // 256 colors
	{ return color_to_256(r,g,b); }
	if (mode == 4)
	{ return color_to_True(r,g,b); }
	return "fuck";
}

string color_to_16(int r, int g, int b, int mode)
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


	return fmt::format("5;{}", color_num);
}

string color_to_256(int r, int g, int b)
{
	int den = 216;

	r = (r * 5/den);
	g = (g * 5/den);
	b = (b * 5/den);

	int color = 16 + 36 * r + 6 * g + b;

    return fmt::format("5;{}", color);
}

string color_to_True(int r, int g, int b)
{
	return fmt::format("2;{};{};{}",r,g,b);
}

int color_distance(int r1, int g1, int b1, int r2, int g2, int b2)
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


string get_pixel_string(int choice)
{
	// Single block
	if (choice == 0)
	{
		return " ";
	}
	// Double block
	if (choice == 1)
	{
		return "  ";
	}
	if (choice == 2)
	{
		return "▄";
	}
	return "N";
}

void blackout(void)
{
	printf("\033[48;2;%d;%d;%dm", 0,0,0);
}

void resized(int in)
{
	struct winsize ws;
	ioctl(1, TIOCGWINSZ, &ws);
	printf("Columns: %d ", ws.ws_col);
	printf("Rows: %d\n", ws.ws_row);
}

int main(void)
{

	signal(SIGWINCH, resized);
	resized(0);


	say(ESC ALT);


	int r = 50;
	int g = 50;
	int b = 150;

	printf("\033[48;2;%d;%d;%dm", r,g,b);

	RGBA bg;
	bg.R = 40;
	bg.G = 90;
	bg.B = 40;
	bg.A = 255;


	// $$$$$$$$$$$$$$$$$$$$$$$

	int l_width, l_height, num_channels;

	// unsigned char *data = stbi_load(CLOTH, &l_width, &l_height, &num_channels, 0);
	unsigned char *data = stbi_load(IMPATH, &l_width, &l_height, &num_channels, 0);
	// unsigned char *data = stbi_load(IMPATH, &l_width, &l_height, &num_channels, STBI_RGBA_alpha);

	printf("Width: %d, Height: %d, Channels: %d\n", l_width, l_height, num_channels);

	int scale = IMG_SCALE;

	int rW = l_width/scale;
	int rH = l_height/scale;

	unsigned char *pic;
	unsigned char *rData = (unsigned char*)malloc(rH * rW * num_channels);

	pic = data;
	if (scale > 1)
	{
		// stbir_resize_uint8(data, width, height, 0,
		// 					    rData, rW, rH, 0,
		// 					    num_channels);
		stbir_resize_uint8_srgb(data, l_width, l_height, 0,
							    rData, rW, rH, 0,
							    num_channels, STBIR_ALPHA_CHANNEL_NONE, 0);
		pic = rData;
	}

	// print_line(pic, 7, rW, num_channels);
	// tg_display_pixels_usc(pic, rW, rH, num_channels);
	 // tg_display_teeny_pixels(pic, rW, rH, num_channels);
	string s_test = tg_dp_teeny_string(pic, rW, rH, num_channels);





	RGBA image [rW][rH];
	for (int i = 0; i < rW; i++)
	{
		for (int j = 0; j < rH; j++)
		{		
			unsigned char *off = pic + (i + rW * j) * num_channels;

			RGBA pixel;
			pixel.R = off[0];
			pixel.G = off[2];
			pixel.B = off[1];
			pixel.A = (num_channels == 4) ? off[3] : 0;

			if (pixel.A == 0) { pixel = bg;}

			if (i == rW/2)
			{
				//printf("R: %d, G: %d, B: %d, Alpha: %d\n", off[0], off[2], off[1], off[3]);
			}

			image[i][j] = pixel;
		}
	}


	// $$$$$$$$$$$$$$$$$$$$$$$

	int test = 0;

	if (test == 1)
	{
		int testW = 30;
		int testH = 30;

		printf("\033[48;2;%d;%d;%dm", 
			image[testH][testW].R, 
			image[testH][testW].G, 
			image[testH][testW].B);
		printf("X: %d, Y: %d\n", testW, testH);
		printf("R: %d, G: %d, B: %d\n", 
			image[testH][testW].R, 
			image[testH][testW].G, 
			image[testH][testW].B);
		printf(" ");
	}

	for (int p = 0; p < rW; p++)
		{printf("%d", p % 10);}
	printf("\n");

	if (test == 2)
	{
		for (int i = 0; i < rH; i++)
	    {
	    	// for (int p = 0; p < rW; p++)
			// 	{printf("%d", p % 10);}
			//printf("\n");
	        for (int j = 0; j < rW; j++)
	        {
	        	printf("\033[48;2;%d;%d;%dm", image[j][i].R, image[j][i].B, image[j][i].G);
	        	//printf("%d", j%10);
	        	printf("  ");
	        }
	        blackout();
	        printf("line %d\n", i);
	    }
	}
	if (test == 3)
	{
		tg_display_pixels_usc(pic, rW, rH, num_channels);
	}

	// loadImage("hello");

	// for (int i = 0; i < 20; i++)
	// {
	// 	printf(" ");
	// 	printf("\033[48;2;%d;%d;%dm", r,g,b+i*5);
	// }
	// printf("\n");

	// for (int i = 0; i < 50; i++)
    // {
    //     for (int j = 0; j < 50; j++)
    //     {
    //     RGBA color;
    //     color.R = rand()%255+1;
    //     color.G = rand()%255+1;
    //     color.B = rand()%255+1;
    //     pixels[i][j] = color;
    //         //printf("%d\n", pixels[i][j]);
    //         printf("  ");
    //         printf("\033[48;2;%d;%d;%dm", pixels[i][j].R, pixels[i][j].B, pixels[i][j].G);
    //     }
    // }



	
	getchar();

	say("\x1b[2J");
	say("\x1b[?1049l");


	// refresh();
	// getch();
	// endwin();
	blackout();
	return 0;
}



