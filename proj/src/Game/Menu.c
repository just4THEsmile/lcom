#include "Menu.h"

// First argument is y and second is x 
char arena[15][30] = {
    "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHH",
    "H_______H_____hCB__h________hH",
    "H________H____H__hh_____H___hH",
    "H_______h_____hhC___________hH",
    "H_______hhH___C__h___H______CH",
    "HhhHHHHHHHH__h__________H___hH",
    "Hh____H__H____H___H_________hH",
    "Hh__________H_H_____H____h__hH",
    "Hh_H__C__H____Ch___H___H____CH",
    "Hh___H__H___hHhC___H________hH",
    "HhCChChhhhhCChH_hH______H___hH",
    "HCCCHHHHHHHHHHhH__h_____H___hH",
    "H______C_____Chh___hHC______hH",
    "H______C______hH_b__h___H___hH",
    "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHH",
};

Mouse mouse;
struct MenuModel model;
extern time_display time_info;

message msg;
int ipc_status;
int r;
int timer_interrupts_per_frame;
int timer_rtc_interrupts_counter;
int timer_interrupts_counter;
uint8_t irq_set_timer;
uint32_t irq_set_mouse;
uint32_t irq_set_kbc;
uint8_t bit_no_mouse;
uint8_t bit_no_kbc;
uint8_t bit_no_timer;
int fr_rate = 20;
extern int scan_code[2];
int i;

void (mouse_api_menu)() {
    if (mouse.x > 150 && mouse.x < 250 && mouse.y > 400 && mouse.y < 420) {
        model.selectedOption = 0;
    } else if (mouse.x > 150 && mouse.x < 224 && mouse.y > 500 && mouse.y < 518) {
        model.selectedOption = 1;
    }
}

void (kbc_api_menu)() {
    if (scan_code[0] == KBC_MAKE_CODE_W) {
        model.selectedOption--;
        if (model.selectedOption < 0) {
            model.selectedOption = model.noptions - 1;
        }
    } else if (scan_code[0] == KBC_MAKE_CODE_S) {
        model.selectedOption++;
        if (model.selectedOption > model.noptions - 1) {
            model.selectedOption = 0;
        }
    }
}

struct MenuModel (get_default_Menu)() {
    model.selectedOption = 0;
    model.nlevels = 1;
    model.noptions = 2;
    model.selectedLevel = 0;
    return model;
}

int (get_started_on_menu)() {
    mouse.x = 600;
    mouse.y = 300;
    mouse.height = 44;
    mouse.width = 44;

    if (map_vram(0x115)) {
        return 1;
    }
    if (vg_init_new(0x115) != OK) {
        return 1;
    }
    if (kbc_Subscribe(&bit_no_kbc) != OK) {
        return 1;
    }
    if (timer_subscribe_int(&bit_no_timer) != OK) {
        return 1;
    }
    if (mouse_Subscribe(&bit_no_mouse) != OK) {
        return 1;
    }

    i = 0;
    irq_set_timer = BIT(bit_no_timer);
    irq_set_kbc = BIT(bit_no_kbc);
    irq_set_mouse = BIT(bit_no_mouse);
    timer_interrupts_per_frame = 60 / fr_rate;
    timer_interrupts_counter = 0;
    timer_rtc_interrupts_counter = 0;
    return 0;
}

struct ArenaModel (Menu)(enum GameState* state, struct ArenaModel arena_model) {
    struct ArenaModel arenamodel = loadArena(arena);
    load_xpms(&arenamodel);
    arenamodel.nScores = arena_model.nScores;

    model = get_default_Menu();

    if (get_started_on_menu()) {
        struct ArenaModel arenamodel = loadArena(arena);
        return arenamodel;
    }

    while (*state == MENU) {
        if (timer_interrupts_counter % timer_interrupts_per_frame == 0) {
            timer_interrupts_counter = 1;
            draw_menu(model, mouse, time_info);
            if (vg_update() != OK) {
                printf("Screen didn't update");
            }
        }

        if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & irq_set_kbc) {
                        kbc_ih();
                        if (scan_code[i] == TWO_BYTES) {
                            i++;
                            continue;
                        }
                        kbc_api_menu();
                        i = 0;
                        if (scan_code[0] == BREAK_ESC || (model.selectedOption == 1 && scan_code[0] == KBC_MAKE_CODE_ENTER)) {
                            *state = EXIT;
                        }
                        if ((model.selectedOption == 0 && scan_code[0] == KBC_MAKE_CODE_ENTER)) {
                            *state = SELECTNAMES;
                        }
                    }

                    if (msg.m_notify.interrupts & irq_set_timer) {
                        timer_interrupts_counter++;
                        timer_rtc_interrupts_counter++;
                        if (timer_rtc_interrupts_counter % 60 == 0) {
                            timer_rtc_interrupts_counter = 1;
                            update_time_display(&time_info);
                        }
                    }

                    if (msg.m_notify.interrupts & irq_set_mouse) {
                        mouse_api_menu();
                        mouse_ih_new(&mouse);

                        if (mouse.left_click == true && mouse.x > 150 && mouse.x < 250 && mouse.y > 400 && mouse.y < 420) {
                            *state = SELECTNAMES;
                        } else if (mouse.left_click == true && mouse.x > 150 && mouse.x < 224 && mouse.y > 500 && mouse.y < 518) {
                            *state = EXIT;
                        }
                    }

                    break;
                default:
                    break;
            }
        } else {
            // Received a standard message, not a notification
            // Do nothing
        }
    }

    mouse_Unsubscribe();
    kbc_Unsubscribe();
    timer_unsubscribe_int();
    vg_exit();



    for (int loop = 0; loop < arena_model.nScores; loop++) {
        arenamodel.scores[loop] = arena_model.scores[loop];
    }

    return arenamodel;
}
