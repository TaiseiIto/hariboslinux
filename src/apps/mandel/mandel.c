#include "dev.h"
#include "math.h"
#include "stdio.h"

typedef struct
{
	double real;
	double imag;
} Complex;

double csqabs(Complex c);
Complex cadd(Complex c1, Complex c2);
Complex cmul(Complex c1, Complex c2);
Color next_color(Color color);

int main(void)
{
	#define WINDOW_CREATED 0x01
	#define WINDOW_EXISTS 0x02
	unsigned char flags = WINDOW_EXISTS;
	unsigned short const window_width = 0x0400;
	unsigned short const window_height = 0x0400;
	double min_real = -2.0;
	double max_real = 2.0;
	double min_imag = -2.0;
	Complex c[window_height][window_width];
	Complex z[window_height][window_width];
	unsigned int window = create_window("mandelbrot", 0x0000, 0x0000, window_width, window_height);
	double zoom_ratio = 2.0;
	double pixel_distance = (max_real - min_real) / (double)window_width;
	Color black;
	Color blue;
	Color current_color;
	black.red = 0x00;
	black.green = 0x00;
	black.blue = 0x00;
	black.alpha = 0xff;
	blue.red = 0x00;
	blue.green = 0x00;
	blue.blue = 0xff;
	blue.alpha = 0xff;
	current_color = blue;
	for(unsigned short y = 0; y < window_height; y++)for(unsigned short x = 0; x < window_width; x++)
	{
		c[y][x].real = (double)x * pixel_distance + min_real;
		c[y][x].imag = (double)y * pixel_distance + min_imag;
		z[y][x].real = 0.0;
		z[y][x].imag = 0.0;
	}
	printf("Center %.10llf%+.10llfi\n", c[window_height / 2][window_width / 2].real, c[window_height / 2][window_width / 2].imag);
	printf("%.10llf per pixel\n", pixel_distance);
	while(flags & WINDOW_EXISTS)
	{
		ApplicationEvent application_event = dequeue_application_event();
		switch(application_event.type)
		{
			Complex cursor_position;
		case APPLICATION_EVENT_TYPE_NOTHING:
			if(flags & WINDOW_CREATED)
			{
				for(unsigned short y = 0; y < window_height; y++)for(unsigned short x = 0; x < window_width; x++)if(csqabs(z[y][x]) <= 4.0)
				{
					z[y][x] = cadd(cmul(z[y][x], z[y][x]), c[y][x]);
					if(4.0 < csqabs(z[y][x]))put_dot_window(window, x, y, current_color);
				}
				current_color = next_color(current_color);
			}
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_CREATED:
			fill_box_window(window, 0x0000, 0x0000, window_width, window_height, black);
			flags |= WINDOW_CREATED;
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			if(application_event.event_union.window_deletion_response_event.window == window)flags &= ~WINDOW_EXISTS;
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_VERTICAL_WHEEL:
			cursor_position = c[application_event.event_union.window_vertical_wheel_event.y][application_event.event_union.window_vertical_wheel_event.x];
			if(0 < application_event.event_union.window_vertical_wheel_event.rotation)pixel_distance *= zoom_ratio; // zoom out
			else if(application_event.event_union.window_vertical_wheel_event.rotation < 0)pixel_distance /= zoom_ratio; // zoom in
			for(unsigned short y = 0; y < window_height; y++)for(unsigned short x = 0; x < window_width; x++)
			{
				c[y][x].real = cursor_position.real + pixel_distance * (double)((short)x - application_event.event_union.window_vertical_wheel_event.x);
				c[y][x].imag = cursor_position.imag + pixel_distance * (double)((short)y - application_event.event_union.window_vertical_wheel_event.y);
				z[y][x].real = 0.0;
				z[y][x].imag = 0.0;
			}
			fill_box_window(window, 0x0000, 0x0000, window_width, window_height, black);
			current_color = blue;
			printf("Center %.10llf%+.10llfi\n", c[window_height / 2][window_width / 2].real, c[window_height / 2][window_width / 2].imag);
			printf("%.10llf per pixel\n", pixel_distance);
			break;
		}
		process_event();
	}
	return 0;
}

double csqabs(Complex c)
{
	return c.real * c.real + c.imag * c.imag;
}

Complex cadd(Complex c1, Complex c2)
{
	Complex result;
	result.real = c1.real + c2.real;
	result.imag = c1.imag + c2.imag;
	return result;
}

Complex cmul(Complex c1, Complex c2)
{
	Complex result;
	result.real = c1.real * c2.real - c1.imag * c2.imag;
	result.imag = c1.real * c2.imag + c1.imag * c2.real;
	return result;
}

Color next_color(Color color)
{
	static const unsigned char color_step = 0x10;
	if(color.red == 0xff && color.green < 0xff && color.blue == 0x00)color.green = color_step < 0xff - color.green ? color.green + color_step : 0xff;
	else if(0x00 < color.red && color.green == 0xff && color.blue == 0x00)color.red = color_step < color.red ? color.red - color_step : 0x00;
	else if(color.red == 0x00 && color.green == 0xff && color.blue < 0xff)color.blue = color_step < 0xff - color.blue ? color.blue + color_step : 0xff;
	else if(color.red == 0x00 && 0x00 < color.green && color.blue == 0xff)color.green = color_step < color.green ? color.green - color_step : 0x00;
	else if(color.red < 0xff && color.green == 0x00 && color.blue == 0xff)color.red = color_step < 0xff - color.red ? color.red + color_step : 0xff;
	else if(color.red == 0xff && color.green == 0x00 && 0x00 < color.blue)color.blue = color_step < color.blue ? color.blue - color_step : 0x00;
	return color;
}

