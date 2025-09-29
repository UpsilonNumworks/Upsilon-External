#include "inc/sprite_buffer.h"


void pushSprite(sprite_buffer_t** head, obm_t* sprite, int x, int y, double angle) {
    sprite_buffer_t* newNode = (sprite_buffer_t*)malloc(sizeof(sprite_buffer_t));
    assert(newNode != NULL);
    newNode->sprite = sprite;
    newNode->x = x;
    newNode->y = y;
    newNode->angle = angle;
    newNode->c_a = cos(angle);
    newNode->s_a = sin(angle);
    newNode->next = *head;
    *head = newNode;
}

void freeSpriteBuffer(sprite_buffer_t* head) {
    sprite_buffer_t* current = head;
    while (current != NULL) {
        sprite_buffer_t* next = current->next;
        free(current);
        current = next;
    }
}

void displaySpriteBuffer(sprite_buffer_t** head) {
    if (!head || !*head) {
        static uint16_t line[SCREEN_WIDTH];
        for (int x = 0; x < SCREEN_WIDTH; x++) line[x] = 0xFFFF;
        for (int y = 0; y < SCREEN_HEIGHT; y++) extapp_pushRect(0, y, SCREEN_WIDTH, 1, line);
        return;
    }

    int count = 0;
    for (sprite_buffer_t* n = *head; n; n = n->next) count++;

    typedef struct {
        const uint16_t* pixels;
        int w, h;
        int minX, maxX, minY, maxY;
        float Au, Bu, Cu;
        float Av, Bv, Cv; 
    } PreSprite;

    PreSprite ps[count];
    int i = 0;
    for (sprite_buffer_t* n = *head; n; n = n->next, i++) {
        obm_t* s = n->sprite;
        int w = s->width;
        int h = s->height;

        float cx = (float)n->x + 0.5f * (float)w;
        float cy = (float)n->y + 0.5f * (float)h;
        float ca = (float)n->c_a;
        float sa = (float)n->s_a;
        float hw = 0.5f * (float)w;
        float hh = 0.5f * (float)h;

        float rx = fabsf(hw * ca) + fabsf(hh * sa);
        float ry = fabsf(hw * sa) + fabsf(hh * ca);
        int x0 = (int)(cx - rx); if (x0 < 0) x0 = 0;
        int x1 = (int)(cx + rx); if (x1 > SCREEN_WIDTH  - 1) x1 = SCREEN_WIDTH  - 1;
        int y0 = (int)(cy - ry); if (y0 < 0) y0 = 0;
        int y1 = (int)(cy + ry); if (y1 > SCREEN_HEIGHT - 1) y1 = SCREEN_HEIGHT - 1;

       
        float Au = ca;
        float Bu = sa;
        float Cu = hw - ca * cx - sa * cy;
        float Av = -sa;
        float Bv = ca;
        float Cv = hh + sa * cx - ca * cy;

        ps[i].pixels = (const uint16_t*)s->pixels;
        ps[i].w = w;
        ps[i].h = h;
        ps[i].minX = x0;
        ps[i].maxX = x1;
        ps[i].minY = y0;
        ps[i].maxY = y1;
        ps[i].Au = Au; ps[i].Bu = Bu; ps[i].Cu = Cu;
        ps[i].Av = Av; ps[i].Bv = Bv; ps[i].Cv = Cv;
    }

    static uint16_t line[SCREEN_WIDTH];

    for (int y = 0; y < SCREEN_HEIGHT; y++) {

        for (int x = 0; x < SCREEN_WIDTH; x++) line[x] = 0xFFFF;
        
        for (int j = count - 1; j >= 0; j--) {
            PreSprite* pr = &ps[j];
            if (y < pr->minY || y > pr->maxY) continue;

            int x_start = pr->minX;
            int x_end   = pr->maxX;
            if (x_start > x_end) continue;

            int w = pr->w;
            int h = pr->h;
            const uint16_t* spx = pr->pixels;

            float y_f = (float)y;
            float u = pr->Au * (float)x_start + pr->Bu * y_f + pr->Cu;
            float v = pr->Av * (float)x_start + pr->Bv * y_f + pr->Cv;
            float du = pr->Au;
            float dv = pr->Av;

            for (int x = x_start; x <= x_end; x++) {
                int iu = (int)u;
                int iv = (int)v;
                if (iu >= 0 && iu < w && iv >= 0 && iv < h) {
                    uint16_t c = spx[iv * w + iu];
                    if (c != 0xF81F) line[x] = c;
                }
                u += du;
                v += dv;
            }
        }

        extapp_pushRect(0, y, SCREEN_WIDTH, 1, line);
    }

    freeSpriteBuffer(*head);
    *head = NULL;
}