// ▄ (U+2584) █ (U+2588), ▓ (U+2593), ▒ (U+2592), ░ (U+2591)


#include <curses.h>

#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>
#include <cmath>
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

// includes implemented by me
// #include pt_color.cpp
#include "PT_Image.h"



// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #define STB_IMAGE_RESIZE_IMPLEMENTATION
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

#define COLOR_MODE 5
#define BLOCKSIZE 1
#define IMG_SCALE 1


#define IMPATH "sf_ken.png"
#define CLOTH "clothes.jpg"
#define SHEET1 "ryu.png"

struct RGBA
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

struct Pair2D
{
	int x;
	int y;
};


string get_pixel_string(int choice);
void blackout(void);
string get_pixel(unsigned char *bucket, int w, int x, int y, int num_channels, int mode);
string get_color_string(int r, int g, int b);
int color_to_16(int r, int g, int b, int mode);
int color_to_256(int r, int g, int b);
int color_to_gray(int r, int g, int b);
int color_distance(int r1, int g1, int b1, int r2, int g2, int b3);
RGBA color_to_True(int r, int g, int b);
RGBA rgb_to_struct(int r, int g, int b);
int map_to_range(int value, int a1, int a2, int b1, int b2);
double linearize_sRGB(int val);
string pt_display_subimage(unsigned char *bucket, int w, int h, Pair2D origin, Pair2D sub, int num_channels);


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

string pt_display_subimage(unsigned char *bucket, int w, int h, Pair2D origin, Pair2D sub, int num_channels)
{
	string image_string = "";

	for (int row = origin.y; row < sub.y; row += 2)
	{
		for (int col = origin.x; col < sub.x; col++)
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
// 8, 16, 256, grayscale strings: bg: \033[48;5;{color}m fg: \033[38;5;{color}m
// true color: bg: \033[48;2;{color};{color};{color}m fg: \033[38;2;{color};{color};{color}m

string get_pixel(unsigned char *bucket, int w, int x, int y, int num_channels, int mode)
{
	unsigned char *off = bucket + (x + (w * y)) * num_channels;
	unsigned char top_red =   	 off[0],
				  top_green = 	 off[1],
				  top_blue =  	 off[2];
				  
	mode = BLOCKSIZE;
	// Single color only modes
	if (mode == 0)
	{
		return fmt::format("\033[48;{}m{}",
						get_color_string(top_red, top_green, top_blue),
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
						get_color_string(top_red, top_green, top_blue),
						get_color_string(bottom_red, bottom_green, bottom_blue),
						get_pixel_string(2));
	}
}

string get_color_string(int r, int g, int b)
{
	int mode = COLOR_MODE; // Probably change to a switch statement
	
	if (mode < 3) // 8 colors, 8 colors BRIGHT, 16 colors
	{ 
		return fmt::format("5;{};", color_to_16(r,g,b,mode)); 
	}
	if (mode == 3) // 256 colors
	{ 
		return fmt::format("5;{};",color_to_256(r,g,b)); 
	}
	if (mode == 4)
	{
		return fmt::format("5;{};",color_to_gray(r,g,b));
	}
	if (mode == 5)
	{  // return color_to_True(r,g,b); 
		return fmt::format("2;{};{};{}", r, g, b);
	}
	return "poop";
}

int color_to_16(int r, int g, int b, int mode)
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

int color_to_256(int r, int g, int b)
{
	int den = 216;

	r = (r * 5/den);
	g = (g * 5/den);
	b = (b * 5/den);

	int color = 16 + 36 * r + 6 * g + b;

	return color;
}

// Returns equivalent grayscale value for pixel as integer
int color_to_gray(int r, int g, int b)
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

double linearize_sRGB(int val)
{
	double dec = ((1.0 * val)/255.0);

	if ( dec <= 0.04045)
	{
		return dec / 12.92;
	}
	return pow((dec + 0.055)/1.055, 2.4);
}


// THIS FUNCTION PROBABLY REDUNDANT
// EXCEPT FOR STRUCTURE CONSISTENCY
RGBA color_to_True(int r, int g, int b) 
{
	return rgb_to_struct(r,g,b);
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

int map_to_range(int value, int a1, int a2, int b1, int b2)
{
	double slope = 1.0 * (b2 - b1) / (a2 - a1);
	double output  = b1 + (slope * (value - a1));
	return (int)output;
}

RGBA rgb_to_struct(int r, int g, int b)
{

	RGBA st;
	st.R = (unsigned char)r;
	st.G = (unsigned char)g;
	st.B = (unsigned char)b;
	st.A = 0;
	
	return st;
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

void split_sprite_series(unsigned char *bucket, Pair2D origin, Pair2D xy_incr, int n, int n_channel, int output[])
{
	output[0] = 5;
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
	unsigned char *data = stbi_load(SHEET1, &l_width, &l_height, &num_channels, 0);
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
	else { delete[] rData; }

	//string s_test = tg_dp_teeny_string(pic, rW, rH, num_channels);
	//string t_test = tg_dp_teeny_string(pic, 100, 60, num_channels);
	//  string u_test = pt_display_subimage(pic, rW, rH, Pair2D{0, 0}, Pair2D{50,90}, num_channels);
	string v_test = pt_display_subimage(pic, rW, rH, Pair2D{50, 0}, Pair2D{100,90}, num_channels);

	// Pair2D org = {0,0};
	// Pair2D incr = {30,60};
	// int num_sprites = 4;

	PT_Image ptest(CLOTH);
	cout << fmt::format("Width: {}", ptest.width) << endl;
	printf("ehhlo");


	// int *series = new int[incr.x * incr.y * num_sprites];
	// split_sprite_series(pic, org, incr, num_sprites, series);

	// cout << "test: " << series[0];

	getchar();

	say("\x1b[2J");
	say("\x1b[?1049l");


	// refresh();
	// getch();
	// endwin();
	blackout();
	return 0;
}



