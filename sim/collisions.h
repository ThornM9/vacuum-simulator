#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

bool is_opaque(int left, int top, int width, int height, char pixel, int i, int j);

bool pixel_collision(int x0, int y0, int w0, int h0, char pixels0[], int x1, int y1, int w1, int h1, char pixels1[]);

bool has_hit_left_wall(int x, int y);

bool has_hit_right_wall(int x, int y);

bool has_hit_top_wall(int x, int y);

bool has_hit_bottom_wall(int x, int y);

bool has_hit_wall(int x, int y);

int * check_collision_dust(int x0, int y0, int w0, int h0, char pixels0[], int idx, char type, int dust_num, char * dust, double dust_x[], double dust_y[]);

int * check_collision_slime(int x0, int y0, int w0, int h0, char pixels0[], int idx, char type, int slime_num, char * slime, double slime_x[], double slime_y[]);

int * check_collision_trash(int x0, int y0, int w0, int h0, char pixels0[], int idx, char type, int trash_num, char * trash, double trash_x[], double trash_y[]);

bool pixel_collision_with_station(int x, int y, int w, int h, char * robot, char * charging_station);

bool is_area_clear(int x0, int y0, int w0, int h0, char pixels0[], int idx, char type, int pos_x, int pos_y, char * robot, 
char * charging_station, int dust_num, char * dust, double dust_x[], double dust_y[], int slime_num, char * slime, double slime_x[], double slime_y[],
int trash_num, char * trash, double trash_x[], double trash_y[]);

void set_collision_globals(void);