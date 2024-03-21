#define BUFFER_SIZE 16

typedef struct {
    int n;
} item_t;

typedef struct {
    item_t buffer[BUFFER_SIZE];
    int in;
    int out;
} shared_t;
