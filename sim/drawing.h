#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

void draw_border(void);

void draw_banner_item(char string[], int row, int column);

void draw_time_status(double start_time);

void draw_banner_items(int angle, int robot_battery, double start_time, int load, int dust_num, int slime_num, int trash_num);

void draw_banner(int angle, int robot_battery, double start_time, int load, int dust_num, int slime_num, int trash_num);

void draw_pixels(int left, int top, int width, int height, char bitmap[], bool space_is_transparent);

void draw_dust(int x, int y);

void draw_slime(int left, int top);

void draw_trash(int left, int top);

void draw_robot(int pos_x, int pos_y);


void draw_station(void);

void set_drawing_globals(void);