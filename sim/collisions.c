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

bool is_opaque(int left, int top, int width, int height, char pixel, int i, int j) {
    if (i >= left && i < left + width && j >= top && j < top + height && pixel != ' ') {
        return true;
    }
    return false;
}

bool pixel_collision(int x0, int y0, int w0, int h0, char pixels0[], int x1, int y1, int w1, int h1, char pixels1[]) {
    for (int j = y0; j < y0 + h0; j++) {
        for (int i = x0; i < x0 + w0; i++) {
            if (is_opaque(x0, y0, w0, h0, pixels0[(i - x0) + (j - y0) * w0], i, j)
                && is_opaque(x1, y1, w1, h1, pixels1[(i - x1) + (j - y1) * w1], i, j)) {
                return true;
            }
        }
    } 
    return false;
}

bool has_hit_left_wall(int x, int y) {
    return (x - 5 <= 0);
}

bool has_hit_right_wall(int x, int y) {
    return (x + 5 > width - 2);
}

bool has_hit_top_wall(int x, int y) {
    return (y - 5 < 7);
}

bool has_hit_bottom_wall(int x, int y) {
    return (y + 5 > height - 6);
}

bool has_hit_wall(int x, int y) {
    return (has_hit_left_wall(x, y) || 
        has_hit_right_wall(x, y) || has_hit_top_wall(x, y) || has_hit_bottom_wall(x, y));
}

void set_collision_globals() {
    height = screen_height();
    width = screen_width();
}

int * check_collision_dust(int x0, int y0, int w0, int h0, char pixels0[], int idx, char type, int dust_num, char * dust, double dust_x[], double dust_y[]) {
    int sum = 0;
    int position_counter = 1;
    static int arr[1000];
    for (int i = 0; i < dust_num; i++) {
        if (i == idx && type == 'd') {
            continue;
        }
        int x = dust_x[i];
        int y = dust_y[i];
        if (pixel_collision(x0, y0, w0, h0, pixels0, x, y, 1, 1, dust)) {
            sum += 1;
            arr[position_counter] = i;
            position_counter += 1;
        }
    }
    arr[0] = sum;
    return arr;
}

int * check_collision_slime(int x0, int y0, int w0, int h0, char pixels0[], int idx, char type, int slime_num, char * slime, double slime_x[], double slime_y[]) {
    int sum = 0;
    int position_counter = 1;
    static int arr[11];
    for (int i = 0; i < slime_num; i++) {
        if (i == idx && type == 's') {
            continue;
        }
        int x = slime_x[i];
        int y = slime_y[i];
        if (pixel_collision(x0, y0, w0, h0, pixels0, x - 3, y - 3, 5, 5, slime)) {
            sum += 1;
            arr[position_counter] = i;
            position_counter += 1;
        }
    }
    arr[0] = sum;
    return arr;
}

int * check_collision_trash(int x0, int y0, int w0, int h0, char pixels0[], int idx, char type, int trash_num, char * trash, double trash_x[], double trash_y[]) {
    int sum = 0;
    int position_counter = 1;
    static int arr[6];
    for (int i = 0; i < trash_num; i++) {
        if (i == idx && type == 't') {
            continue;
        }
        int x = trash_x[i];
        int y = trash_y[i];
        if (pixel_collision(x0, y0, w0, h0, pixels0, x - 6, y - 3, 11, 6, trash)) {
            sum += 1;
            arr[position_counter] = i;
            position_counter += 1;
        }
    }
    arr[0] = sum;
    return arr;
}

bool pixel_collision_with_station(int x, int y, int w, int h, char * robot, char * charging_station) {
    return pixel_collision(x, y, w, h, robot, round((width / 2) - (9 / 2)), 7, 9, 3, charging_station);
}


bool is_area_clear(int x0, int y0, int w0, int h0, char pixels0[], int idx, char type, int pos_x, int pos_y, char * robot, 
char * charging_station, int dust_num, char * dust, double dust_x[], double dust_y[], int slime_num, char * slime, double slime_x[], double slime_y[],
int trash_num, char * trash, double trash_x[], double trash_y[]) {
    // check collision w/ robot
    if (pixel_collision(x0, y0, w0, h0, pixels0, pos_x - 5, pos_y - 5, 9, 9, robot)) {
        return false;
    }

    // check collision w/ charging station
    if (pixel_collision(x0, y0, w0, h0, pixels0, round((width / 2) - (9 / 2)), 7, 9, 3, charging_station)) {
        return false;
    }

    // check collision w/ dust
    int *dust_collisions;
    dust_collisions = check_collision_dust(x0, y0, w0, h0, pixels0, idx, type, dust_num, dust, dust_x, dust_y);
    if (*(dust_collisions + 0) > 0) {
        return false;
    }

    // check collision w/ slime
    int * slime_collisions;
    slime_collisions = check_collision_slime(x0, y0, w0, h0, pixels0, idx, type, slime_num, slime, slime_x, slime_y);
    if (*(slime_collisions + 0) > 0) {
        return false;
    }

    // check collision w/ trash
    int * trash_collisions;
    trash_collisions = check_collision_trash(x0, y0, w0, h0, pixels0, idx, type, trash_num, trash, trash_x, trash_y);
    if (*(trash_collisions + 0) > 0) {
        return false;
    }

    return true;
}