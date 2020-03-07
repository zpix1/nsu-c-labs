#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "notes.h"

int main() {
    printf("Testing notes generation\n");
    struct NotesList* list = new_list();

    for (int i = 0; i < 10; i++) {
        char text[100];
        sprintf(text, "note n.%d", i + 1);
        struct Note* note = new_note(text);
        add_note_to_list(list, note);
    }

    assert(list->length == 10);

    printf("Testing save_notes\n");

    FILE* f = fopen("tdata.bin", "wb");
    save_notes(list, f);
    fclose(f);

    free_list(list);


    printf("Testing load_notes\n");
    FILE* f1 = fopen("tdata.bin", "rb");
    struct NotesList* new_notes = load_notes(f1);
    struct LinkedListNode* node = new_notes->head;
    fclose(f1);
    
    while (node != NULL) {
        debug_note(node->note);
        node = node->next;
    }

    free_list(new_notes);

    return 0;
}