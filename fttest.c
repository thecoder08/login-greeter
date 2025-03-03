// FreeType Test: A basic example to show how to render text with libxgfx using FreeType.

#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>
#include <execinfo.h>
#include <xgfx/window.h>
#include <xgfx/drawing.h>

// Error handling: If an error occurred, print an error message, then a backtrace, then exit the program
void* trace[20];
void handleError(int error, const char* msg) {
    if (error) {
        fprintf(stderr, "Error: %s\n", msg);
        int traceLength = backtrace(trace, 20);
        backtrace_symbols_fd(trace, traceLength, 2);
        exit(1);
    }
}

FT_Library library;
FT_Face face;
char* textString = "Hello World!";
int x = 100;
int y = 100;

void bufferGlyph(int x, int y, int width, int height, unsigned char* buffer) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char shade = buffer[i * width + j];
            plot(x + j, y + i, (0xff - shade) | ((0xff - shade) << 8) | ((0xff - shade) << 16));
        }
    }
}

int main() {
    handleError(FT_Init_FreeType(&library), "Error initializing FreeType"); // Initialize FreeType
    handleError(FT_New_Face(library, "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 0, &face), "Error loading font face"); // Load a font
    initWindow(640, 480, "Font Rendering");
    rectangle(0, 0, 640, 480, 0xffffffff);
    handleError(FT_Set_Char_Size(face, 0, 12*64, 300, 300), "Error setting character size"); // Set font size
    for (int i = 0; textString[i] != 0; i++) { // Loop through characters in string
        handleError(FT_Load_Char(face, textString[i], FT_LOAD_RENDER), "Error loading/rendering glyph"); // Load/render glyph
        bufferGlyph(x + face->glyph->bitmap_left, y - face->glyph->bitmap_top, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer); // Draw glyph to window
        x += face->glyph->advance.x >> 6; // Advance draw position
    }
    while(1) {
    updateWindow();
    }
}
