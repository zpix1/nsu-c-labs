#include "notes.h"

struct NotesList* new_list() {
    struct NotesList* ans = (struct NotesList*)malloc(sizeof(struct NotesList));
    ans->end = NULL;
    ans->head = NULL;
    ans->length = 0;
    return ans;
}

void add_note_to_list(struct NotesList* list, struct Note* note) {
    struct LinkedListNode* node = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
    node->note = note;
    node->next = NULL;
    if (list->length == 0) {
        if (note->id == -1) {
            note->id = 0;
        }
        node->prev = NULL;

        list->head = node;
        list->end = node;
    } else {
        if (note->id == -1) {
            note->id = list->end->note->id + 1;
        }

        node->prev = list->end;
        list->end->next = node;
        list->end = node;        
    }

    list->length++;
}

void remove_note_from_list_by_id(struct NotesList* list, int note_id) {
    if (list->length == 0) {
        return;
    }
    struct LinkedListNode* t = list->head;
    while (t != NULL) {
        if (t->note->id == note_id) {
            if (t->prev != NULL) {
                t->prev->next = t->next;
            } else {
                list->head = t->next;
            }
            if (t->next != NULL) {
                t->next->prev = t->prev;
            } else {
                list->end = t->prev;
            }
            free(t);
            break;
        }
        t = t->next;
    }
    list->length--;
}


void free_list(struct NotesList* list) {
    struct LinkedListNode* cur = list->head;
    while (cur != NULL) {
        struct LinkedListNode* tmp = cur->next;
        free(cur->note);
        free(cur);
        cur = tmp;
    }
    free(list);
}

void free_only_list(struct NotesList* list) {
    struct LinkedListNode* cur = list->head;
    while (cur != NULL) {
        struct LinkedListNode* tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    free(list);
}


void debug_note(struct Note* note) {
    char time[100];
    struct tm * ptm = localtime(&note->date);
    strftime(time, 100, "%Y-%m-%d %H:%M:%S", ptm);
    printf("<note id=%d, date=\"%s\", text=\"%s\">\n", note->id, time, note->text);
}

void time_t_to_str(time_t t, char* str, int length) {
    struct tm * ptm = localtime(&t);
    strftime(str, length, "%Y-%m-%d %H:%M:%S", ptm);
}

struct Note* new_note(char* text) {
    struct Note* ans = (struct Note*)malloc(sizeof(struct Note));
    memset(ans, 0, sizeof(struct Note));
    ans->date = time(NULL);
    ans->id = -1;
    strcpy(ans->text, text);
    return ans;
}

int save_notes(struct NotesList* notes, FILE* f) {
    struct LinkedListNode* current_node = notes->head;
    int size = notes->length;
    
    fwrite(&size, sizeof(int), 1, f);
    while (current_node != NULL) {
        fwrite(current_node->note, sizeof(struct Note), 1, f);
        current_node = current_node->next;
    }
    return 0;
}

struct NotesList* load_notes(FILE* f) {
    int size = 0;
    int read = fread(&size, sizeof(int), 1, f);

    if (read == 0) {
        return NULL;
    }
    struct NotesList* list = new_list();
    for (int i = 0; i < size; i++) {

        struct Note* note = (struct Note*)malloc(sizeof(struct Note));
        fread(note, sizeof(struct Note), 1, f);
        add_note_to_list(list, note);
    }

    return list;
}
