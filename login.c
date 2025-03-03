#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>
#include <execinfo.h>
#include <xgfx/window.h>
#include <xgfx/drawing.h>
#include <shadow.h>
#include <crypt.h>

void* trace[20];
void handleError(int error, const char* msg) {
    if (error) {
        fprintf(stderr, "Error: %s\n", msg);
        int traceLength = backtrace(trace, 20);
        backtrace_symbols_fd(trace, traceLength, 2);
        exit(1);
    }
}

FT_Face face;

void ftSetup() {
    FT_Library library;
    handleError(FT_Init_FreeType(&library), "Error initializing FreeType");
    handleError(FT_New_Face(library, "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 0, &face), "Error loading font face");
}

void bufferGlyph(int x, int y, int width, int height, unsigned char* buffer) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char shade = buffer[i * width + j];
            plot(x + j, y + i, shade | shade << 8 | shade << 16);
        }
    }
}

void drawText(int x, int y, int size, char* string) {
    handleError(FT_Set_Char_Size(face, 0, size*64, 300, 300), "Error setting character size");
    for (int i = 0; string[i] != 0; i++) {
        handleError(FT_Load_Char(face, string[i], FT_LOAD_RENDER), "Error loading/rendering glyph");
        bufferGlyph(x + face->glyph->bitmap_left, y - face->glyph->bitmap_top, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
        x += face->glyph->advance.x >> 6;
    }
}

void drawTextHidden(int x, int y, int size, char* string) {
    handleError(FT_Set_Char_Size(face, 0, size*64, 300, 300), "Error setting character size");
    for (int i = 0; string[i] != 0; i++) {
        handleError(FT_Load_Char(face, L'•', FT_LOAD_RENDER), "Error loading/rendering glyph");
        bufferGlyph(x + face->glyph->bitmap_left, y - face->glyph->bitmap_top, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
        x += face->glyph->advance.x >> 6;
    }
}

void validateCredentials(char* u, char* p) {
    struct spwd* shadowEntry = getspnam(u);
    if (!shadowEntry) {
        fprintf(stderr, "Failed to retrieve shadow entry!\n");
        exit(1);
    }
    char* hashed = crypt(p, shadowEntry->sp_pwdp);
    if (strcmp(hashed, shadowEntry->sp_pwdp) == 0) {
        printf("Username and password correct!\n");
    }
    else {
        fprintf(stderr, "Password Incorrect!\n");
        exit(1);
    }
}

char username[50];
char password[50];
char* charBuffer = username;
// the following keyboard code is modified from my-os keyboard.c
char *mapping_default = "\?\?1234567890-=??qwertyuiop[]??asdfghjkl;'`?\\zxcvbnm,./??? ???????????????????????????????";
char *mapping_shifted = "\?\?!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?|ZXCVBNM<>???? ???????????????????????????????";
int shifted = 0;
int i = 0;
void keyboardHander(unsigned char inbyte, int state) {
    if (inbyte == 0) {
        return;
    }
    if (state)
    {
        if ((inbyte == 0x2a) || (inbyte == 0x36))
        {
            shifted = 1;
        }
        else if (inbyte == 0x1c)
        {
            charBuffer[i] = 0;
            i = 0;
            if (charBuffer == username) {
                charBuffer = password;
            }
            else {
                validateCredentials(username, password);
            }
            return;
        }
        else if (inbyte == 0x0e)
        {
            if (i > 0)
            {
                i--;
                charBuffer[i] = 0;
            }
        }
        else
        {
            if (shifted)
            {
                charBuffer[i] = mapping_shifted[inbyte];
            }
            else
            {
                charBuffer[i] = mapping_default[inbyte];
            }
            i++;
        }
    }
    else if ((inbyte == 0x2a) || (inbyte == 0x36))
    {
        shifted = 0;
    }
}

int main() {
    ftSetup();
    initWindow(640, 480, "Login Greeter");

    while(1) {
        Event event;
        while(checkWindowEvent(&event)) {
            if (event.type == WINDOW_CLOSE) {
                return 0;
            }

            if (event.type == KEY_CHANGE) {
                keyboardHander(event.keychange.key, event.keychange.state);
            }
        }

        drawText(240, 100, 12, "Log In");
        drawText(50, 200, 5, "Username:");
        rectangle(200, 175, 400, 30, 0xffffffff);
        rectangle(202, 177, 396, 26, 0xff000000);
        drawText(204, 200, 5, username);
        drawText(50, 300, 5, "Password:");
        rectangle(200, 275, 400, 30, 0xffffffff);
        rectangle(202, 277, 396, 26, 0xff000000);
        drawTextHidden(204, 300, 5, password);
        updateWindow();
    }
}