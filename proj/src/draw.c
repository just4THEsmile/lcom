//
// Created by diogo on 22/04/2023.
//

#include "draw.h"


xpm_image_t     bomberman_idle_down_white,bomberman_down_wallking_1_white ,bomberman_down_wallking_2_white,
                bomberman_idle_up_white,bomberman_up_wallking_1_white ,bomberman_up_wallking_2_white,
                bomberman_idle_right_white,bomberman_right_wallking_1_white,bomberman_right_wallking_2_white ,
                bomberman_idle_left_white,bomberman_left_wallking_1_white,bomberman_left_wallking_2_white ,


                bomberman_idle_down_black,bomberman_down_wallking_1_black ,bomberman_down_wallking_2_black,
                bomberman_idle_up_black,bomberman_up_wallking_1_black ,bomberman_up_wallking_2_black,
                bomberman_idle_right_black,bomberman_right_wallking_1_black,bomberman_right_wallking_2_black ,
                bomberman_idle_left_black,bomberman_left_wallking_1_black,bomberman_left_wallking_2_black ,

                explosion_center_1,explosion_top_1,explosion_left_1,explosion_right_1,
                explosion_center_2,explosion_top_2,explosion_left_2,explosion_right_2,

                bomb1,bomb2,bomb3,
                brick,
                wall,
                mouse;

int (load_xpms)(){
    //white Bomberman
    xpm_load(downidlewhite_xpm, XPM_8_8_8, &bomberman_idle_down_white);
    xpm_load(downwalk1white_xpm, XPM_8_8_8, &bomberman_down_wallking_1_white);
    xpm_load(downwalk2white_xpm, XPM_8_8_8, &bomberman_down_wallking_2_white);

    xpm_load(upidlewhite_xpm, XPM_8_8_8, &bomberman_idle_up_white);
    xpm_load(upwalk1white_xpm, XPM_8_8_8, &bomberman_up_wallking_1_white);
    xpm_load(upwalk2white_xpm, XPM_8_8_8, &bomberman_up_wallking_2_white);

    xpm_load(leftidlewhite_xpm, XPM_8_8_8, &bomberman_idle_left_white);
    xpm_load(walkleft1white_xpm, XPM_8_8_8, &bomberman_left_wallking_1_white);
    xpm_load(walkleft2white_xpm, XPM_8_8_8, &bomberman_left_wallking_2_white);

    xpm_load(rightidlewhite_xpm, XPM_8_8_8, &bomberman_idle_right_white);
    xpm_load(walkright1white_xpm, XPM_8_8_8, &bomberman_right_wallking_1_white);
    xpm_load(walkright2white_xpm, XPM_8_8_8, &bomberman_right_wallking_2_white);

    xpm_load(downidleblack_xpm, XPM_8_8_8, &bomberman_idle_down_black);
    xpm_load(downwalk1black_xpm, XPM_8_8_8, &bomberman_down_wallking_1_black);
    xpm_load(downwalk2black_xpm, XPM_8_8_8, &bomberman_down_wallking_2_black);

    xpm_load(upidleblack_xpm, XPM_8_8_8, &bomberman_idle_up_black);
    xpm_load(upwalk1black_xpm, XPM_8_8_8, &bomberman_up_wallking_1_black);
    xpm_load(upwalk2black_xpm, XPM_8_8_8, &bomberman_up_wallking_2_black);

    xpm_load(leftidleblack_xpm, XPM_8_8_8, &bomberman_idle_left_black);
    xpm_load(walkleft1black_xpm, XPM_8_8_8, &bomberman_left_wallking_1_black);
    xpm_load(walkleft2black_xpm, XPM_8_8_8, &bomberman_left_wallking_2_black);

    xpm_load(rightidleblack_xpm, XPM_8_8_8, &bomberman_idle_right_black);
    xpm_load(walkright1black_xpm, XPM_8_8_8, &bomberman_right_wallking_1_black);
    xpm_load(walkright2black_xpm, XPM_8_8_8, &bomberman_right_wallking_2_black);


    return 0;

}                

void (drawWalls)(char** arena, struct ArenaModel model){
    for(int i=0;i<model.nWalls;i++){
        arena[model.walls[i].position.x][model.walls[i].position.y]=  'H';
    }
}
void drawBricks(char** arena, struct ArenaModel model){
    for(int i=0;i<model.nBricks;i++){
        arena[model.bricks[i].position.x][model.bricks[i].position.y]=  'h';
    }
}
void drawPowerUps(char** arena, struct ArenaModel model){
    for(int i=0;i<model.nPowerUps;i++){
        arena[model.powerUps[i].position.x][model.powerUps[i].position.y]=  'P';
    }
}
void drawBombs(char** arena, struct ArenaModel model){
    for(int i=0;i<model.nBombs;i++){
        arena[model.bombs[i].position.x][model.bombs[i].position.y]=  'B';
    }
}
void (drawPlayers)(char** arena, struct ArenaModel model){
    for(int i=0;i<2;i++){
//        printf("\n%d\n",model.players[i].id);
        if(model.players[i].id==0)
            arena[model.players[i].position.x][model.players[i].position.y]='P';
        else arena[model.players[i].position.x][model.players[i].position.y]='p';
    }
}
char (**getemptyArena()) {
    char** arena = (char**) malloc(15 * sizeof(char*));
    for (int i = 0; i < 15; i++) {
        arena[i] = (char*) malloc(30 * sizeof(char));
        for (int j = 0; j < 30; j++) {
            arena[i][j] = '_';
        }
    }
    return arena;
}
void draw( struct ArenaModel model){
    char** arena=getemptyArena();
    drawWalls(arena, model);
    drawBricks(arena, model);
    drawPowerUps(arena, model);
    drawBombs(arena, model);
    drawPlayers(arena, model);

    for (int i=0;i<30;i++) {
        printf("\n");
        for(int j=0;j<15;j++)
            printf("%c",arena[j][i]);

    }
}
