#define STRINGS_IMPLEMENTATION
#define ARENA_IMPLEMENTATION
#define LOGGING_IMPLEMENTATION

#include "utils/macros.h"
#include "utils/strings.h"
#include "utils/arena.h"
#include "utils/logging.h"


int main(void) {
    Arena a = {0};
    String_Builder sb = arena_sb_from_cstr(&a,"foo");
    arena_sb_append_cstr(&sb, &a,"bar");
    sb_to_uppercase(&sb);
    append(&sb, 'c');
    arena_sb_to_cstr(&sb, &a);
    log_info("%s", sb.data);
    arena_free(&a);
}