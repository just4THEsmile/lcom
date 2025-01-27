#include "Game.h"

extern Mouse mouse;
extern int fr_rate;
extern int scan_code[2];
extern int i;
extern time_display time_info;
double aftergamecountdown;

int timer_interrupts_per_frame;
int timer_interrupts_counter;
uint8_t irq_set_timer;
uint32_t irq_set_mouse;
uint32_t irq_set_kbc;
uint8_t bit_no_mouse;
uint8_t bit_no_kbc;
uint8_t bit_no_timer;

void (mouse_api_game)(struct ArenaModel* model, enum GameState* state) {
    struct Button button = model->returnButton;
    if (mouse.x > button.x && mouse.x < button.x + button.width && mouse.y > button.y && mouse.y < button.y + button.height) {
        model->returnButton.selected = true;
        if (mouse.left_click) {
            *state = MENU;
            aftergamecountdown = 0;
        }
    } else {
        model->returnButton.selected = false;
    }
}

int (handleInterrupts)() {
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
    if (kbc_Subscribe(&bit_no_kbc) != OK)
        return 1;

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
    return 0;
}

time_display (Game)(struct ArenaModel* model, enum GameState* state) {
    message msg;
    int ipc_status;
    int r;
    aftergamecountdown = 5;
    handleInterrupts();

    while (*state == GAME || aftergamecountdown > 0) {
        if (timer_interrupts_counter % timer_interrupts_per_frame == 0) {
            timer_interrupts_counter = 1;
            if (PlayersAreAlive(model, state) && *state == GAME) {
                CoinController(model);
                draw_players_info(*model);
                PlayersSpriteControllers(model);
                BombsSpriteControllers(model);
                ExplosionsController(model);
            } else if (*state == PLAYER1WON || *state == PLAYER2WON || *state == TIE) {
                draw_Game_over_report(*model, *state);
            }

            if (*state != GAME) {
                aftergamecountdown -= model->elapsedTime;
            }

            draw_game(*model, mouse, time_info);
            if (vg_update() != OK) {
                printf("Screen didn't update");
            }
        }

        if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & irq_set_kbc) {
                        /* subscribed interrupt */
                        kbc_ih();
                        if (scan_code[i] == TWO_BYTES) {
                            i++;
                            continue;
                        }
                        i = 0;
                        if (scan_code[0] == BREAK_ESC) {
                            *state = EXIT;
                        }
                        PlayerControllers(model);
                    }

                    if (msg.m_notify.interrupts & irq_set_timer) {
                        timer_interrupts_counter++;
                    }

                    if (msg.m_notify.interrupts & irq_set_mouse) {
                        mouse_api_game(model, state);
                        mouse_ih_new(&mouse);
                    }

                    break;

                default:
                    break; /* no other notifications expected: do nothing */
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    mouse_Unsubscribe();
    kbc_Unsubscribe();
    timer_unsubscribe_int();
    vg_exit();

    return time_info;
}
