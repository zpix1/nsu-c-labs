#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

struct LinkedListNode {
    struct LinkedListNode* next;
    struct LinkedListNode* prev;
    
    struct Note* note;
};

struct NotesList {
    struct LinkedListNode* head;
    struct LinkedListNode* end;
    int length;
};

#define MAX_TEXT_SIZE 1000

struct Note {
    char text[MAX_TEXT_SIZE];
    time_t date;
    int id;
};


// Uitls

void time_t_to_str(time_t time, char* str, int length);

// NotesList

// Free list and notes
struct NotesList* new_list();

void add_note_to_list(struct NotesList* list, struct Note* note);

void remove_note_from_list_by_id(struct NotesList* list, int note_id);

void free_list(struct NotesList* list);
void free_only_list(struct NotesList* list);


// Note

void debug_note(struct Note* note);

// Create a note with given text
struct Note* new_note(char* text);

// Save given notes to binary file `f`
int save_notes(struct NotesList* list, FILE* f);

// Load notes from binary file `f`
struct NotesList* load_notes(FILE* f);

