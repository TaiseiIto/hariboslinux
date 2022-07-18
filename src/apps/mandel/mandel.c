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
	#define CTRL_PUSHED	0x01
	#define DRAGGED		0x02
	#define WINDOW_CREATED	0x04
	#define WINDOW_EXISTS	0x08
	unsigned char flags = WINDOW_EXISTS;
	unsigned short const window_width = 0x0400;
	unsigned short const window_height = 0x0400;
	unsigned int window = create_window("mandelbrot", 0x0000, 0x0000, window_width, window_height);
	double min_real = -2.0;
	double max_real = 2.0;
	double min_imag = -2.0;
	Complex c[window_height][window_width];
	Complex z[window_height][window_width];
	Complex cursor_position;
	unsigned int const accuracy = 10;
	double zoom_ratio = 2.0;
	double pixel_distance = (max_real - min_real) / (double)window_width;
	short drag_x;
	short drag_y;
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
	printf("Center %.*llf%+.*llfi\n", accuracy, c[window_height / 2][window_width / 2].real, accuracy, c[window_height / 2][window_width / 2].imag);
	printf("%.*llf per pixel\n", accuracy, pixel_distance);
	while(flags & WINDOW_EXISTS)
	{
		ApplicationEvent application_event = dequeue_application_event();
		switch(application_event.type)
		{
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
		case APPLICATION_EVENT_TYPE_WINDOW_CLICKED:
			if(application_event.event_union.window_clicked_event.window == window)
			{
				if(flags & CTRL_PUSHED)
				{
					if(application_event.event_union.window_clicked_event.flags & APPLICATION_WINDOW_CLICKED_EVENT_FLAG_PUSHED)
					{
						cursor_position = c[application_event.event_union.window_clicked_event.y][application_event.event_union.window_clicked_event.x];
						if(application_event.event_union.window_clicked_event.flags & APPLICATION_WINDOW_CLICKED_EVENT_FLAG_LEFT_BUTTON)pixel_distance /= zoom_ratio; // zoom in
						if(application_event.event_union.window_clicked_event.flags & APPLICATION_WINDOW_CLICKED_EVENT_FLAG_RIGHT_BUTTON)pixel_distance *= zoom_ratio; // zoom out
						for(unsigned short y = 0; y < window_height; y++)for(unsigned short x = 0; x < window_width; x++)
						{
							c[y][x].real = cursor_position.real + pixel_distance * (double)((short)x - application_event.event_union.window_vertical_wheel_event.x);
							c[y][x].imag = cursor_position.imag + pixel_distance * (double)((short)y - application_event.event_union.window_vertical_wheel_event.y);
							z[y][x].real = 0.0;
							z[y][x].imag = 0.0;
						}
						fill_box_window(window, 0x0000, 0x0000, window_width, window_height, black);
						current_color = blue;
						printf("Center %.*llf%+.*llfi\n", accuracy, c[window_height / 2][window_width / 2].real, accuracy, c[window_height / 2][window_width / 2].imag);
						printf("%.*llf per pixel\n", accuracy, pixel_distance);
					}
				}
				else
				{
					if(application_event.event_union.window_clicked_event.flags & APPLICATION_WINDOW_CLICKED_EVENT_FLAG_PUSHED)
					{
						drag_x = 0;
						drag_y = 0;
						flags |= DRAGGED;
					}
					if(flags & DRAGGED && application_event.event_union.window_clicked_event.flags & APPLICATION_WINDOW_CLICKED_EVENT_FLAG_RELEASED)
					{
						if(drag_x || drag_y)
						{
							for(unsigned short y = 0; y < window_height; y++)for(unsigned short x = 0; x < window_width; x++)
							{
								c[y][x].real -= pixel_distance * (double)drag_x;
								c[y][x].imag -= pixel_distance * (double)drag_y;
								z[y][x].real = 0.0;
								z[y][x].imag = 0.0;
							}
							fill_box_window(window, 0x0000, 0x0000, window_width, window_height, black);
							current_color = blue;
							printf("Center %.*llf%+.*llfi\n", accuracy, c[window_height / 2][window_width / 2].real, accuracy, c[window_height / 2][window_width / 2].imag);
							printf("%.*llf per pixel\n", accuracy, pixel_distance);
						}
						flags &= ~DRAGGED;
					}
				}
			}
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_CREATED:
			if(application_event.event_union.window_created_event.window == window)
			{
				fill_box_window(window, 0x0000, 0x0000, window_width, window_height, black);
				flags |= WINDOW_CREATED;
			}
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			if(application_event.event_union.window_deletion_response_event.window == window)flags &= ~WINDOW_EXISTS;
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_DRAG:
			if(application_event.event_union.window_drag_event.window == window)
			{
				drag_x += application_event.event_union.window_drag_event.x_movement;
				drag_y += application_event.event_union.window_drag_event.y_movement;
			}
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_KEYBOARD:
			if(application_event.event_union.window_keyboard_event.window == window)
			{
				switch(application_event.event_union.window_keyboard_event.keyboard_event.keycode)
				{
				case KEY_CONTROL:
					flags |= CTRL_PUSHED;
					break;
				case KEY_CONTROL | KEY_RELEASED:
					flags &= ~CTRL_PUSHED;
					break;
				}
			}
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_VERTICAL_WHEEL:
			if(application_event.event_union.window_vertical_wheel_event.window == window)
			{
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
				printf("Center %.*llf%+.*llfi\n", accuracy, c[window_height / 2][window_width / 2].real, accuracy, c[window_height / 2][window_width / 2].imag);
				printf("%.*llf per pixel\n", accuracy, pixel_distance);
			}
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

