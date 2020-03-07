#include <curses.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "notes.h"

#define T_ESC_KEY 27
#define T_KEY_ENTER 10
#define true 1
#define false 0
#define SAVES_FILE "data.bin"
#define MAX_SEARCH_STR_LENGTH 30
#define GOOD_STR_SIZE 120
#define NOTE_PREVIEW_SIZE 69
enum Mode {
    NewNoteScreen,
    MenuScreen,
    ListNotesScreen
};

int width;
int height;

enum Mode mode = MenuScreen;

int keypos = 0;
int to_rerender = 0;

struct NotesList* all_notes;

char input_text[MAX_TEXT_SIZE] = "";
struct Note* current_note = NULL;

int is_delete_key(int ch) {
    if (ch == KEY_DC || ch == KEY_BACKSPACE || ch == 127) {
        return 1;
    } else {
        return 0;
    }
}

void exit_all() {
    FILE* f = fopen(SAVES_FILE, "wb");
    save_notes(all_notes, f);
    fclose(f);
    
    free_list(all_notes);

    endwin();
    exit(0);
}

void render_centered_str(const char* str, int y, int x, int attributes) {
    wmove(stdscr, y, x - strlen(str)/2);
    for (int i = 0; i < strlen(str); i++) {
        waddch(stdscr, str[i] | attributes);
    }
}

void render_rect_text(char* text, int s_y, int s_x, int n_row, int n_col) {
    int c_row = 0, c_col = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] != '\n') {
            if (c_col < n_col) {
                mvwaddch(stdscr, c_row + s_y, c_col + s_x, text[i]);
            }
            c_col++;
        } else {
            if (c_row < n_row) {
                c_row++;
                c_col = 0;
            }
        }
    }
    wmove(stdscr, c_row + s_y, c_col + s_x);
}

void move_keypos(int ch, int max_keypos) {
    if (max_keypos == 0) {
        return;
    }
    switch (ch) {
        case KEY_UP:
            keypos = (keypos + max_keypos - 1)  % max_keypos;
            break;
        case KEY_DOWN:
            keypos = (keypos + 1)  % max_keypos;
            break;
    }
}

void render_menu_screen(int ch) {
    static const char* menu_items[] = {
        "new note",
        "list my notes",
        "exit"
    };
    static const int count_of_menu_items = 3;

    if (ch != ERR) {
        move_keypos(ch, count_of_menu_items);
    }
    
    for (int i = 0; i < count_of_menu_items; i++) {
        render_centered_str(menu_items[i], height/2 + i - count_of_menu_items/2, width/2, keypos == i ? A_UNDERLINE : 0);
    }

    if (ch == T_KEY_ENTER) {
        if (!strcmp(menu_items[keypos], "exit")) {
            exit_all();
        } else if (!strcmp(menu_items[keypos], "new note")) {
            clear();
            keypos = 0;
            mode = NewNoteScreen;
            return;
        } else if (!strcmp(menu_items[keypos], "list my notes")) {
            clear();
            keypos = 0;
            mode = ListNotesScreen;
            return;
        } 
    }
}

void insert_char(char* text, int length, char ch) {
    char text_buffer[MAX_TEXT_SIZE];
    strcpy(text_buffer, text + length);
    text[length] = ch;
    strcpy(text + length + 1, text_buffer);
}

void remove_char(char* text, int length) {
    int i;
    for (i = length; text[i] != '\0'; i++) {
        text[i - 1] = text[i]; 
    }
    text[i - 1] = '\0';
}

int get_length_by_row_col_length(char* text, int row_length, int col_length) {
    int row_count = 0;
    int i;
    for (i = 0; text[i] != '\0' && row_length != row_count; i++) {
        if (text[i] == '\n') {
            row_count++;
        }
    }
    int tmp = i;
    for (; text[i] != '\n' && text[i] != '\0' && i < col_length + tmp; i++);
    
    return i;
}

void get_row_col_length(char* text, int length, int* row_length_ptr, int* col_length_ptr) {
    int row_length = 0;
    int col_length = 0;
    for (int i = 0; i < length && text[i] != '\0'; i++) {
        if (text[i] != '\n') {
            col_length++;
        } else {
            row_length++;
            col_length = 0;
        }
    }
    *row_length_ptr = row_length;
    *col_length_ptr = col_length;
}

void get_max_row_col_length(char* text, int current_row, int* max_row_length_ptr, int* max_col_length_ptr) {
    int row_length = 0;
    int col_length = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if (current_row == row_length) {
            *max_col_length_ptr = col_length;
        }
        if (text[i] != '\n') {
            col_length++;
        } else {
            row_length++;
            col_length = 0;
        }
    }
    
    *max_row_length_ptr = row_length;
}

void render_new_note_screen(int ch) {
    static int length = 0;
    static int row_length = 0;
    static int col_length = 0;
    static int input_text_strlen = 0;
    int max_row_length = height - 4;
    int max_col_length = width - 4;

    static int c_max_row = 0;
    static int c_max_col = 0;
    
    if (ch == KEY_F(5)) {
        clear();
        keypos = 0;
        mode = MenuScreen;

        current_note = NULL;
        row_length = col_length = length = 0;
        memset(input_text, 0, MAX_TEXT_SIZE);

        return;
    } else if (ch == KEY_F(6)) {
        if (current_note == NULL) {
            struct Note* note = new_note(input_text);
            add_note_to_list(all_notes, note);
        } else {
            strcpy(current_note->text, input_text);
            current_note->date = time(NULL);
            current_note = NULL;
        }
        row_length = col_length = length = 0;
        memset(input_text, 0, MAX_TEXT_SIZE);
        clear();
        keypos = 0;
        mode = MenuScreen;
        return;
    } else if (is_delete_key(ch)) {
        if (length > 0) {
            remove_char(input_text, length);
            length--;
        }
    } else if (ch != ERR) {
        if (ch == KEY_LEFT) {
            if (length > 0) {
                length--;
            }
        } else if (ch == KEY_RIGHT) {
            if (length < input_text_strlen) {
                length++;
            }
        } else if (ch == KEY_UP) {
            if (row_length > 0) {
                row_length--;
                length = get_length_by_row_col_length(input_text, row_length, col_length);
            }
        } else if (ch == KEY_DOWN) {
            if (row_length < max_row_length) {
                row_length++;
                length = get_length_by_row_col_length(input_text, row_length, col_length);
            }
        } else if (length < MAX_TEXT_SIZE) {
            if (ch == T_KEY_ENTER) {
                if (c_max_row < max_row_length - 1) {
                    insert_char(input_text, length, '\n');
                    length++;
                }
            } else {
                if (c_max_col < max_col_length - 1 && c_max_row < max_row_length) {
                    insert_char(input_text, length, ch);
                    length++;
                }
            }
        }
    }
    

    get_max_row_col_length(input_text, row_length, &c_max_row, &c_max_col);
    get_row_col_length(input_text, length, &row_length, &col_length);

    input_text_strlen = strlen(input_text);

    render_centered_str("discard changes and exit (F5)", height - 1, width/2, 0);
    render_centered_str("save and exit (F6)", height - 1, width - 10, 0);

    char status_str[GOOD_STR_SIZE];
    sprintf(status_str, "%d/%d [%d, %d] m[%d, %d]                ", input_text_strlen, MAX_TEXT_SIZE, col_length + 1, row_length + 1, c_max_col + 1, c_max_row + 1);

    if (current_note == NULL) {
        render_centered_str("your note (anon)", 1, width/2, 0);
    } else {
        char note_str[GOOD_STR_SIZE * 2];
        char time[GOOD_STR_SIZE];
        time_t_to_str(current_note->date, time, GOOD_STR_SIZE);
        sprintf(note_str, "your note (id=%d, date=%s)", current_note->id, time);
        render_centered_str(note_str, 1, width/2, 0);
    }
    

    mvwaddstr(stdscr, height - 1, 1, status_str);
    for (int i = 0; i < max_row_length; i++) {
        wmove(stdscr, i + 2, 0);
        wclrtoeol(stdscr);
    }
    render_rect_text(input_text, 2, 2, max_row_length, max_col_length);

    wmove(stdscr, 2 + row_length, 2 + col_length);
}

void generate_note_entry(struct Note* note, char* entry) {
    char time[GOOD_STR_SIZE];
    char note_preview[NOTE_PREVIEW_SIZE];

    int j = 0;
    for (j = 0; note->text[j] != '\0' && note->text[j] != '\n' && j < NOTE_PREVIEW_SIZE - 1; j++) {
        note_preview[j] = note->text[j];
    }
    note_preview[j] = '\0';

    time_t_to_str(note->date, time, GOOD_STR_SIZE);
    sprintf(entry, "%d:%s %.69s", note->id, time, note_preview);
}

void render_list_notes_screen(int ch) {
    static int search_str_length = 0;
    static char search_str[MAX_SEARCH_STR_LENGTH] = "";

    for (int i = 0; i < all_notes->length; i++) {
        wmove(stdscr, 2 + i, 1);
        wclrtoeol(stdscr);
    }

    if (is_delete_key(ch)) {
        if (search_str_length > 0) {
            search_str[search_str_length - 1] = '\0';
            search_str_length--;
            wmove(stdscr, 1, 1);
            wclrtoeol(stdscr);
        }
    } else if (ch != ERR && ch != KEY_UP && ch != KEY_DOWN && ch != T_KEY_ENTER && ch != KEY_DC && ch != KEY_F(4) && ch != KEY_F(5)) {
        if (search_str_length < MAX_SEARCH_STR_LENGTH - 1) {
            search_str[search_str_length] = ch;
            search_str[search_str_length + 1] = '\0';
            search_str_length++;
        }
    }

    // Filter notes by search_str
    struct NotesList* filtered_notes = new_list();
    struct LinkedListNode* cur = all_notes->head;
    while (cur != NULL) {
        char note_status[GOOD_STR_SIZE];
        generate_note_entry(cur->note, note_status);
        if (strlen(search_str) == 0 ||strstr(note_status, search_str) != NULL || strstr(cur->note->text, search_str) != NULL) {
            add_note_to_list(filtered_notes, cur->note);
        }
        cur = cur->next;
    }

    // Handle keys
    int length = filtered_notes->length;
    if (ch == KEY_UP || ch == KEY_DOWN) {
        move_keypos(ch, length);
    } else if (ch == KEY_F(5)) {
        clear();
        memset(search_str, 0, MAX_SEARCH_STR_LENGTH);
        free_only_list(filtered_notes);
        search_str_length = 0;
        keypos = 0;
        mode = MenuScreen;
        return;
    }
    
    // Display notes
    if (length != 0) {
        int i = 0;
        cur = filtered_notes->head;
        while (cur != NULL) {
            if (keypos == i && ch == T_KEY_ENTER) {
                clear();
                current_note = cur->note;
                strcpy(input_text, cur->note->text);
                keypos = 0;
                mode = NewNoteScreen;
                memset(search_str, 0, MAX_SEARCH_STR_LENGTH);
                free_only_list(filtered_notes);
                return;
            } else if (keypos == i && ch == KEY_F(4)) {
                remove_note_from_list_by_id(all_notes, cur->note->id);
                free(cur->note);
                clear();
                to_rerender = 1;
                free_only_list(filtered_notes);
                return;
            }

            char note_status[GOOD_STR_SIZE];
            generate_note_entry(cur->note, note_status);
            mvwaddstr(stdscr, 2 + i, 2, note_status);

            mvwaddch(stdscr,  2 + i, 1,  keypos == i ? '*' : ' ');
            
            cur = cur->next;
            i++;
        }
    } else {
        mvwaddstr(stdscr, 2, 2, "no notes yet");
    }

    render_centered_str("back (F5)", height - 1, width/2, 0);
    render_centered_str("delete note (F4)", height - 1, width - 9, 0);

    mvwaddstr(stdscr, 1, 1, search_str);
    free_only_list(filtered_notes);
}

int main(int argc, char** argv) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE); 
    getmaxyx(stdscr, height, width);

    int ch;

    if (access(SAVES_FILE, F_OK) != -1) {
        FILE* f = fopen(SAVES_FILE, "rb");
        all_notes = load_notes(f);
        fclose(f);
    } else {
        all_notes = new_list();
    }

    ch = ERR;

    while (true) {
        enum Mode old_mode = mode;
        render_centered_str("nsnotebook v1.0", 1, width/2, 0);
        if (mode == MenuScreen) {
            render_menu_screen(ch);
        } else if (mode == NewNoteScreen) {
            render_new_note_screen(ch);
        } else if (mode == ListNotesScreen) {
            render_list_notes_screen(ch);
        }

        if (old_mode != mode || to_rerender) {
            ch = ERR;
            to_rerender = 0;
            continue;
        }
        ch = getch();
    }

    endwin();
}