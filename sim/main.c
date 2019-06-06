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
#include "drawing.h"
#include "collisions.h"

int width;
int height;
int delay = 10;
int timeout = INT_MAX;
double start_time;
int seconds;

int angle;
double velocity;
double pos_x;
double pos_y;

double robot_battery = 100.00;
int load = 0;

int dust_num;
int slime_num;
int trash_num;

double dust_x[1000], dust_y[1000], slime_x[10], slime_y[10], trash_x[5], trash_y[5];

bool paused = false;
bool game_finished = false;
bool charging_mode = false;
bool detect_collisions = true;
bool manual_move = false;
bool move_to_base = false;
double charging_rate;

char * dust = 
"."
;

char * slime = 
"  ~  "
" ~~~ "
"~~~~~"
" ~~~ "
"  ~  "
;

char * trash = 
"     &     "
"    &&&    "
"   &&&&&   "
"  &&&&&&&  "
" &&&&&&&&& "
"&&&&&&&&&&&"
;

char * robot =
"  @@@@@  "
" @@@@@@@ "
"@@@@@@@@@"
"@@@@@@@@@"
"@@@@@@@@@"
"@@@@@@@@@"
"@@@@@@@@@"
" @@@@@@@ "
"  @@@@@  "
;

char * charging_station = 
"#########"
"#########"
"#########"
;


int swivel() {
    int angle_change = (rand() % (60 - 30 + 1)) + 30;
    return angle_change;
}

bool is_timed_out() {
    return get_current_time() - start_time >= timeout;
}

void set_init_values() {
    paused = true;
    angle = 90;
    velocity = 0.2;
    robot_battery = 100.00;
    pos_x = round(width / 2);
    pos_y = round(height / 2);
    start_time = get_current_time();
    dust_num = get_int("How much dust do you want? ");
    slime_num = get_int("How much slime do you want? ");
    trash_num = get_int("How much trash do you want? ");
    if (dust_num > 1000) {dust_num = 1000;}
    if (slime_num > 10) {slime_num = 10;}
    if (trash_num > 5) {trash_num = 5;}
};

void handle_robot_station_collision() {
    if (move_to_base) {
        paused = true;
        charging_mode = true;
        move_to_base = false;
        charging_rate = ((100 - robot_battery) / 3000) * delay;
        
        load = 0;
    } else {
        angle = (rand() % (170 - 10 + 1)) + 10;
    }
}

void handle_dust_robot_collision(int i) {
    for (int c = i; c < dust_num - 1; c++) {
        dust_x[c] = dust_x[c+1];
        dust_y[c] = dust_y[c+1];
    }
    load += 1;
    dust_num -= 1;
}

void handle_slime_robot_collision(int i) {
    for (int c = i; c < slime_num - 1; c++) {
        slime_x[c] = slime_x[c+1];
        slime_y[c] = slime_y[c+1];
    }
    load += 5;
    slime_num -= 1;
}

void handle_trash_robot_collision(int i) {
    for (int c = i; c < trash_num - 1; c++) {
        trash_x[c] = trash_x[c+1];
        trash_y[c] = trash_y[c+1];
    }
    load += 20;
    trash_num -= 1;
}

void detect_robot_collisions(int x, int y) {
    if (has_hit_wall(x, y)) {
        angle += swivel();
    }

    // station
    if (pixel_collision(x - 5, y - 5, 10, 10, robot, round((width / 2) - (9 / 2)), 7, 9, 3, charging_station)) {
        handle_robot_station_collision();
    }

    if (!detect_collisions) {
        return;
    }

    // remove all dust touching
    for (int i = 0; i < dust_num; i++) {
        if (pixel_collision(x - 4, y - 4, 9, 9, robot, dust_x[i], dust_y[i], 1, 1, dust)) {
            handle_dust_robot_collision(i);
        }
    }

    for (int i = 0; i < slime_num; i++) {
        if (pixel_collision(x - 4, y - 4, 9, 9, robot, slime_x[i] - 3, slime_y[i] - 3, 5, 5, slime)) {
            handle_slime_robot_collision(i);
        }
    }

    for (int i = 0; i < trash_num; i++) {
        if (pixel_collision(x - 4, y - 4, 9, 9, robot, trash_x[i] - 6, trash_y[i] - 3, 11, 6, trash)) {
            handle_trash_robot_collision(i);
        }
    }
}

void setup_dust(int i) {
    do {
        dust_x[i] = (rand() % ((width - 2) - 1 + 1)) + 1;
        dust_y[i] = (rand() % ((height - 6) - 7 + 1)) + 7;
    }
    while (!is_area_clear(dust_x[i] -1, dust_y[i] - 1, 2, 1, dust, i, 'd',
    pos_x, pos_y, robot, charging_station, dust_num, dust, dust_x, dust_y, slime_num, slime, 
    slime_x, slime_y, trash_num, trash, trash_x, trash_y));
}

void setup_slime(int i) {
    do {
        slime_x[i] = (rand() % ((width - 7) - 6 + 1)) + 6;
        slime_y[i] = (rand() % ((height - 11) - 12 + 1)) + 12;
    }
    while (!is_area_clear(slime_x[i] -4, slime_y[i] - 4, 6, 6, slime, i, 's',
    pos_x, pos_y, robot, charging_station, dust_num, dust, dust_x, dust_y, slime_num, slime, 
    slime_x, slime_y, trash_num, trash, trash_x, trash_y));
}

void setup_trash(int i) {
    do {
        trash_x[i] = (rand() % ((width - 8) - 7 + 1)) + 7;
        trash_y[i] = (rand() % ((height - 10) - 11 + 1)) + 11;
    }
    while (!is_area_clear(trash_x[i] -6, trash_y[i] - 3, 11, 6, trash, i, 't',
    pos_x, pos_y, robot, charging_station, dust_num, dust, dust_x, dust_y, slime_num, slime, 
    slime_x, slime_y, trash_num, trash, trash_x, trash_y));
}

void setup_rubbish() {
    for (int i = 0; i < trash_num; i++) {
        setup_trash(i);
    }

    for (int i = 0; i < slime_num; i++) {
        setup_slime(i);
    }
    
    for (int i = 0; i < dust_num; i++) {
        setup_dust(i);
    }

}


void return_to_base() {
    double t1 = pos_x - round(width / 2);
	double t2 = (pos_y - 5) - 9;
	double d = sqrt(t1 * t1 + t2 * t2);
    double dx = t1 * velocity / d;
	double dy = t2 * velocity / d;
    angle = (atan2(dy, dx) * 180 / M_PI) + 180;
    move_to_base = true;
};

void drop_dust() {
    double x = get_int("Dust x position");
    double y = get_int("Dust y position");
    if (dust_num > 999) {
        return;
    }

    dust_x[dust_num] = x;
    dust_y[dust_num] = y;
    dust_num += 1;
};

void drop_slime() {
    double x = get_int("Slime x position");
    double y = get_int("Slime y position");
    if (slime_num > 9) {
        return;
    }
    slime_x[slime_num] = x;
    slime_y[slime_num] = y;
    slime_num += 1;
};

void drop_trash() {
    double x = get_int("Trash x position");
    double y = get_int("Trash y position");
    if (trash_num > 4) {
        return;
    }
    trash_x[trash_num] = x;
    trash_y[trash_num] = y;
    trash_num += 1;
};



void move_robot(int ch) {
    manual_move = true;
    if (ch == 'i' && !has_hit_top_wall(pos_x, pos_y)
    && !pixel_collision_with_station(pos_x-4, pos_y-5, 9, 9, robot, charging_station)) {
        pos_y -= 1;
    } else if (ch == 'j' && !has_hit_left_wall(pos_x, pos_y)
    && !pixel_collision_with_station(pos_x-5, pos_y-4, 9, 9, robot, charging_station)) {
        pos_x -= 1;
    } else if (ch == 'k' && !has_hit_bottom_wall(pos_x, pos_y)
    && !pixel_collision_with_station(pos_x-4, pos_y-3, 9, 9, robot, charging_station)) {
        pos_y += 1;
    } else if (ch == 'l' && !has_hit_right_wall(pos_x, pos_y)
    && !pixel_collision_with_station(pos_x-3, pos_y-4, 9, 9, robot, charging_station)) {
        pos_x += 1;
    }
};

void set_delay() {
    delay = get_int("How many milliseconds between loops?");
};

void set_timeout() {
    timeout = get_int("How many seconds shall the program run?");
};

void pause_play() {
    paused = !paused;
};

void reset() {
    set_init_values();
    setup_rubbish();
};

void pick_location_and_angle() {
    pos_x = (double) get_int("New x position?");
    pos_y = (double) get_int("New y position?");
    angle = get_int("New direction?");
};

void change_weight() {
    int add_load = get_int("How much weight would you like to add?");
    load += add_load;
};

void change_battery() {
    robot_battery = (double) get_int("What is the new battery value?");
}

void show_help_screen() {
    clear_screen();
    int x_pos = width / 2 - 40;
    draw_string(x_pos, 10, "'b': activate return to base");
    draw_string(x_pos, 11, "'d': drop a piece of dust");
    draw_string(x_pos, 12, "'i': push the device one unit up");
    draw_string(x_pos, 13, "'j': push the device one unit left");
    draw_string(x_pos, 14, "'k': push the device one unit down");
    draw_string(x_pos, 15, "'l': push the device one unit right");
    draw_string(x_pos, 16, "'m': set the delay in milliseconds");
    draw_string(x_pos, 17, "'o': set the time-out period in seconds");
    draw_string(x_pos, 18, "'p': pause simulation");
    draw_string(x_pos, 19, "'q': quit simulation");
    draw_string(x_pos, 20, "'r': reset simulation");
    draw_string(x_pos, 21, "'s': drop a piece of slime (medium rubbish) on the floor");
    draw_string(x_pos, 22, "'t': drop a piece of trash (large rubbish) on the floor");
    draw_string(x_pos, 23, "'v': move the device to a new location and specify a new direction");
    draw_string(x_pos, 24, "'w': change the amount (weight) of the rubbish currently on board the device");
    draw_string(x_pos, 25, "'y': change the battery level");
    draw_string(x_pos, 26, "'?': display help screen");
    draw_string(x_pos, 28, "Press any key to exit");
    show_screen();
    wait_char();
}

void show_exit_screen(bool isResetable) {
    int top = height / 2 - 10;
    int left = width / 2 - 20;
    draw_rect(left, top, left + 45, top + 20, '$');
    if (isResetable) {
        draw_string(left + 5, top + 4, "SIMULATION OVER");
        draw_string(left + 5, top + 5, "Press 'r' to reset");
        draw_string(left + 5, top + 6, "Press 'q' to exit");
        show_screen();
        int ch = wait_char();
        if (ch == 'r') {
            reset();
        } else if (ch == 'q') {
            game_finished = true;
        }
    } else {
        draw_string(left + 5, top + 5, "Press any key to exit");
        show_screen();
        wait_char();
    }
}

void do_operation(int ch) {
    if (ch == 'b') {
        return_to_base();
    } else if (ch == 'd') {
        drop_dust();
    } else if (ch == 's') {
        drop_slime();
    } else if (ch == 't') {
        drop_trash();
    } else if (ch == 'i' || ch == 'j' || ch == 'k' || ch =='l') {
        move_robot(ch);
    } else if (ch == 'm') {
        set_delay();
    } else if (ch == 'o') {
        set_timeout();
    } else if (ch == 'p') {
        pause_play();
    } else if (ch == 'r') {
        reset();
    } else if (ch == 'v') {
        pick_location_and_angle();
    } else if (ch == 'w') {
        change_weight();
    } else if (ch == 'y') {
        change_battery();
    } else if (ch == '?') {
        show_help_screen();
    }
}

void draw_rubbish(double dust_x[], double dust_y[], double slime_x[], double slime_y[], double trash_x[], double trash_y[], int dust_num, int slime_num, int trash_num) {
    for (int i = 0; i < dust_num; i++) {
        draw_dust(dust_x[i], dust_y[i]);
    }
    for (int i = 0; i < slime_num; i++) {
        draw_slime(slime_x[i], slime_y[i]);
    }

    for (int i = 0; i < trash_num; i++) {
        draw_trash(trash_x[i], trash_y[i]);
    }
}

void setup() {
    set_drawing_globals();
    set_collision_globals();
    set_init_values();
    draw_banner(angle, robot_battery, start_time, load, dust_num, slime_num, trash_num);
    draw_border();
    draw_station();
    draw_robot(pos_x, pos_y);
    setup_rubbish();
};

void handle_not_paused_update() {
    if (!paused) {
        if (robot_battery < 25 || load > 45 || move_to_base) {
            return_to_base();
            detect_collisions = false;
        }
        if (!manual_move) {
            pos_x = pos_x + velocity * cos(angle * M_PI / 180);
            pos_y = pos_y + velocity * sin(angle * M_PI / 180);
        }

        detect_robot_collisions(pos_x, pos_y);
        if (has_hit_wall(pos_x, pos_y)) {
            return;
        }
        
        if (floor(get_current_time() - start_time) > seconds && robot_battery > 0) {
            robot_battery--;
        }
        seconds = floor(get_current_time() - start_time);
        manual_move = false;
    }
}

void draw_all() {
    clear_screen();
    draw_banner(angle, robot_battery, start_time, load, dust_num, slime_num, trash_num);
    draw_border();
    draw_station();
    if (charging_mode) {
        char * prompt = "Docked";
        draw_string((width - strlen(prompt)) / 2, height - 2, prompt);
    }
    draw_rubbish(dust_x, dust_y, slime_x, slime_y, trash_x, trash_y, dust_num, slime_num, trash_num);
    draw_robot(pos_x, pos_y);
    show_screen();
}

void update() {
    handle_not_paused_update();
    if (charging_mode) {
        robot_battery += charging_rate;
        if (robot_battery >= 99) {
            robot_battery = 100.00;
            angle = (rand() % (170 - 10 + 1)) + 10;
            paused = false;
            charging_mode = false;
            detect_collisions = true;
        }
    }

    if (robot_battery < 1) {
        robot_battery = 0;
        velocity = 0;
        show_exit_screen(true);
    }

    draw_all();
}

void manage_loop() {
    int ch = get_char();
    if (ch == 'q') {
        game_finished = true;
        show_exit_screen(false);
        return;
    }
    else if (ch >= ' ') {
        do_operation(ch);
    }
    update();
    
    timer_pause(delay);
}

int main(void) {
    srand(get_current_time());
    setup_screen();
    height = screen_height();
    width = screen_width();
    
    setup();
    show_screen();
    while (!game_finished && !is_timed_out()) {
        
        manage_loop();
    }
    return 0;
}