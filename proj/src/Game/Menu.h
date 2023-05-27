
#ifndef MENU_H
#define MENU_H
#include <stdlib.h>
#include "models/models.h"
#include "draw/draw.h"
#include "load.h"
#include "devices/keyboard.h"
#include "devices/mouse.h"
#include "devices/rtc.h"

struct ArenaModel (Menu)(enum GameState* state,struct ArenaModel arena_model);


#endif // !MENU_H
