#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"

int height;
int width;

// draw border around terminal
void draw_border() {
    // vertical lines
    draw_line(0, 1, 0, height-6, '|');
    draw_line(width-1, 1, width-1, height-6, '|');

    // horizontal lines
    draw_line(1, 0, width-2, 0, '-');
    draw_line(1, height-5, width-2, height-5, '-');

    // corners and intersects
    draw_char(0, 0, '+');
    draw_char(width-1, 0, '+');
    draw_char(0, height-5, '+');
    draw_char(width-1, height-5, '+');

    // banner outer intersects
    draw_char(0, 6, '+');
    draw_char(width-1, 6, '+');
    draw_char(0, 3, '+');
    draw_char(width - 1, 3, '+');

    // other banner intersects
    int column_width = round(width / 3);
    draw_char(column_width, 0, '+');
    draw_char(column_width, 3, '+');
    draw_char(column_width, 6, '+');
    draw_char(column_width * 2, 0, '+');
    draw_char(column_width * 2, 3, '+');
    draw_char(column_width * 2, 6, '+');

    return;
}

void draw_banner_item(char string[], int row, int column) {
    int x;
    int y;
    if (row == 0) {
        y = 2;
    } else if (row == 1) {
        y = 5;
    }
    int column_width = round(width / 3);
    int string_length = strlen(string);
    int offset = (column_width - string_length) / 2;
    x = column_width * column + offset;
    draw_formatted(x, y, "%s", string);
}

void draw_time_status(double start_time) {
    char time_status[15];
    double diff = get_current_time() - start_time;
    int minutes = floor(diff / 60);
    int seconds = (int) diff % 60;
    if (minutes > 10 && seconds > 10) {
        sprintf(time_status, "Elapsed: %d:%d", minutes, seconds);
    } else if (minutes > 10) {
        sprintf(time_status, "Elapsed: %d:0%d", minutes, seconds);
    } else if (minutes < 10 && seconds < 10) {
        sprintf(time_status, "Elapsed: 0%d:0%d", minutes, seconds);
    } else if (minutes < 10) {
        sprintf(time_status, "Elapsed: 0%d:%d", minutes, seconds);
    } else if (seconds < 10) {
        sprintf(time_status, "Elapsed: %d:0%d", minutes, seconds);
    } else if (minutes == 0) {
        sprintf(time_status, "Elapsed: 00:%d", seconds);
    } else {
        sprintf(time_status, "Elapsed: %d:%d", minutes, seconds);
    }
    draw_banner_item(time_status, 1, 0);
}

void draw_banner_items(int angle, int robot_battery, double start_time, int load, int dust_num, int slime_num, int trash_num) {
    // draw student number
    draw_banner_item("n9576321", 0, 0);

    // draw robot direction
    char direction_status[15];
    int direction = angle % 360;
    sprintf(direction_status, "Direction: %d", direction);
    draw_banner_item(direction_status, 0, 1);

    // draw robot battery
    char battery_status[15];
    sprintf(battery_status, "Battery: %d%%", (int) robot_battery);
    draw_banner_item(battery_status, 0, 2);

    // time is drawn in loop
    draw_time_status(start_time);

    // draw load weight
    char load_status[15];
    sprintf(load_status, "Load (g): %d", load);
    draw_banner_item(load_status, 1, 1);
    
    // draw rubbish numbers
    char rubbish_status[30];
    sprintf(rubbish_status, "Litter (d,s,t): %d,%d,%d", dust_num, slime_num, trash_num);
    draw_banner_item(rubbish_status, 1, 2);
}

void draw_banner(int angle, int robot_battery, double start_time, int load, int dust_num, int slime_num, int trash_num) {
    draw_line(1, 6, width-2, 6, '-');
    draw_line(1, 3, width - 2, 3, '-');

    int column_width = round(width / 3);
    draw_line(column_width, 1, column_width, 6, '|');
    draw_line(column_width * 2, 1, column_width * 2, 6, '|');

    draw_banner_items(angle, robot_battery, start_time, load, dust_num, slime_num, trash_num);
}

void draw_pixels(int left, int top, int width, int height, char bitmap[], bool space_is_transparent) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (bitmap[i + j * width] != ' ') {
                draw_char(left + i, top + j, bitmap[i + j * width]);
            } else if (!space_is_transparent) {
                draw_char(left + i, top + j, ' ');
            }
        }
    }
};

void draw_dust(int x, int y) {
    draw_char(x, y, '.');
}

void draw_slime(int x, int y) {
    char * slime = 
    "  ~  "
    " ~~~ "
    "~~~~~"
    " ~~~ "
    "  ~  "
    ;
    draw_pixels(x - 3, y - 3, 5, 5, slime, true);
}

void draw_trash(int x, int y) {
    char * trash = 
    "     &     "
    "    &&&    "
    "   &&&&&   "
    "  &&&&&&&  "
    " &&&&&&&&& "
    "&&&&&&&&&&&"
    ;
    draw_pixels(x - 6, y - 3, 11, 6, trash, true);
}

void draw_robot(int pos_x, int pos_y) {
    int robot_width = 9;
    int robot_height = 9;
    char * robot =
    "  @@@@@  "
    " @=====@ "
    "@=======@"
    "@=======@"
    "@=======@"
    "@=======@"
    "@=======@"
    " @=====@ "
    "  @@@@@  "
    ;
    int left = round(pos_x - (robot_width / 2));
    int top = round(pos_y - (robot_height / 2));
    draw_pixels(left, top, robot_width, robot_height, robot, true);
};

void draw_station() {
    char * charging_station = 
    "#########"
    "#########"
    "#########"
    ;
    draw_pixels(round((width / 2) - (9 / 2)), 7, 9, 3, charging_station, true);
}

void set_drawing_globals() {
    height = screen_height();
    width = screen_width();
}