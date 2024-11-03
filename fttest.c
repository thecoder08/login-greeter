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

int main() {
    handleError(FT_Init_FreeType(&library), "Error initializing FreeType"); // Initialize FreeType
    handleError(FT_New_Face(library, "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 0, &face), "Error loading font face"); // Load a font
    initWindow(640, 480, "Font Rendering");
    handleError(FT_Set_Char_Size(face, 0, 12*64, 300, 300), "Error setting character size"); // Set font size
    for (int i = 0; textString[i] != 0; i++) { // Loop through characters in string
        handleError(FT_Load_Char(face, textString[i], FT_LOAD_RENDER), "Error loading/rendering glyph"); // Load/render glyph
        buffer(x + face->glyph->bitmap_left, y - face->glyph->bitmap_top, face->glyph->bitmap.width, face->glyph->bitmap.rows, 4, face->glyph->bitmap.buffer); // Draw glyph to window
        x += face->glyph->advance.x >> 6; // Advance draw position
    }
    updateWindow();
    while(1);
}
