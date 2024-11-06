/*
 * game.h
 *
 *  Created on: Mar 20, 2024
 *      Author: dylanlim
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

void clearscreen();
void turnoffscreen();
void turnonscreen();
void addpixel(uint8_t r, uint8_t c);
void pwm_pixel(uint8_t r, uint8_t c);
void removepixel(uint8_t r, uint8_t c);
void display_end();
void display_snake();
void display_pause();
void change_coordinate(int x_pos[4], int y_pos[4], int new_x_point, int new_y_point);
int move_direction(int dir);
int new_x_coordinates(int new_head_x);
int new_y_coordinates(int new_head_y);
int check_keep_playing();



#endif /* INC_GAME_H_ */
