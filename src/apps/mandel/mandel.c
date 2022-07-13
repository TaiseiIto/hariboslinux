#include "dev.h"
#include "math.h"
#include "stdio.h"

typedef struct
{
	double real;
	double imag;
} Complex;

double cabs(Complex c);
Complex cadd(Complex c1, Complex c2);
Complex cmul(Complex c1, Complex c2);
Color next_color(Color color);

int main(void)
{
	#define WINDOW_CREATED 0x01
	#define WINDOW_EXISTS 0x02
	unsigned char flags = WINDOW_EXISTS;
	unsigned int const window_width = 0x0400;
	unsigned int const window_height = 0x0400;
	double min_real = -2.0;
	double max_real = 2.0;
	double min_imag = -2.0;
	double max_imag = 2.0;
	Complex c[window_height][window_width];
	Complex z[window_height][window_width];
	unsigned int window = create_window("mandelbrot", 0x0000, 0x0000, window_width, window_height);
	Color black;
	Color current_color;
	black.red = 0x00;
	black.green = 0x00;
	black.blue = 0x00;
	black.alpha = 0xff;
	current_color.red = 0x00;
	current_color.green = 0x00;
	current_color.blue = 0xff;
	current_color.alpha = 0xff;
	for(unsigned int y = 0; y < window_height; y++)for(unsigned int x = 0; x < window_width; x++)
	{
		c[y][x].real = (double)x * (max_real - min_real) / (double)window_width + min_real;
		c[y][x].imag = (double)y * (max_imag - min_imag) / (double)window_height + min_imag;
		z[y][x].real = 0.0;
		z[y][x].imag = 0.0;
	}
	while(flags & WINDOW_EXISTS)
	{
		ApplicationEvent application_event = dequeue_application_event();
		switch(application_event.type)
		{
		case APPLICATION_EVENT_TYPE_NOTHING:
			if(flags & WINDOW_CREATED)
			{
				for(unsigned int y = 0; y < window_height; y++)for(unsigned int x = 0; x < window_width; x++)if(cabs(z[y][x]) <= 2.0)
				{
					z[y][x] = cadd(cmul(z[y][x], z[y][x]), c[y][x]);
					if(2.0 < cabs(z[y][x]))put_dot_window(window, x, y, current_color);
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
			printf("rotation = %d\n", application_event.event_union.window_vertical_wheel_event.rotation);
			break;
		}
		process_event();
	}
	return 0;
}

double cabs(Complex c)
{
	return sqrt(c.real * c.real + c.imag * c.imag);
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
	if(color.red == 0x00 && color.green == 0x00)color.red = 0x10;
	else if(color.green == 0x00 && color.blue == 0x00)color.green = 0x10;
	else if(color.blue == 0x00 && color.red == 0x00)color.red = 0x10;
	else if(color.red == 0x00)
	{
		if(color.green == 0xff)
		{
			color.blue += 0x10;
			if(color.blue < 0x10)color.blue = 0xff;
		}
		else if(color.blue == 0xff)
		{
			if(color.green < 0x10)color.green = 0x00;
			color.green -= 0x10;
		}
	}
	else if(color.green == 0x00)
	{
		if(color.blue == 0xff)
		{
			color.red += 0x10;
			if(color.red < 0x10)color.red = 0xff;
		}
		else if(color.red == 0xff)
		{
			if(color.blue < 0x10)color.blue = 0x00;
			color.blue -= 0x10;
		}
	}
	else if(color.blue == 0x00)
	{
		if(color.red == 0xff)
		{
			color.green += 0x10;
			if(color.green < 0x10)color.green = 0xff;
		}
		else if(color.green == 0xff)
		{
			if(color.red < 0x10)color.red = 0x00;
			color.red -= 0x10;
		}
	}
	return color;
}

