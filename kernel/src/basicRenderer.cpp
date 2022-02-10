#include "basicRenderer.h"
#include "math.h"

void cursor::reset() {
  this->x = 0;
  this->y = 0;
}

void cursor::newLine() {
  if (this->y <= this->ym) {
    this->x = 0;
    this->y++;
  }
}

void cursor::back() {
  if (this->x != 0) {
    this->x--;
  } else if (this->y != 0) {
    this->y--;
  }
}

void cursor::up() {
  if (this->y != 0) {
    this->y--;
  }
}

void cursor::down() {
  if (this->y != this->ym) {
    this->y++;
  }
}

void cursor::left() {
  if (this->x != 0) {
    this->x--;
  } else {
    this->up();
    this->x = this->xm;
  }
}

void cursor::right() {
  if (this->x != this->xm) {
    this->x++;
  } else {
    this->down();
    this->x = 0;
  }
}

basicRenderer basicRender;
cursor* cur;

/*
** Plots a pixel at x, y.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
** Color is argb hex value.. (  aarrggbb)
**                           (0x00000000)
*/
void basicRenderer::plotPixel(unsigned int x, unsigned int y, unsigned int color) {
  if ((x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
    *(unsigned int*)(x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
  }
}

/*
** Draws a line from x1, y1 to x2, y2.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
** Color is argb hex value.. (  aarrggbb)
**                           (0x00000000)
*/
void basicRenderer::drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  int a = y2 - y1;
  int b = x2 - x1;
  int length = sqrt(b *  b + a * a);
  for (double i = 0; i < (unsigned int)(length * 2); i++) {
    if (((x1 + (unsigned int)(b * (i / (unsigned int)(length * 2)))) * 4 + ((y1 + (unsigned int)(a * (i / (unsigned int)(length * 2)))) * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
      *(unsigned int*)((x1 + (unsigned int)(b * (i / (unsigned int)(length * 2)))) * 4 + ((y1 + (unsigned int)(a * (i / (unsigned int)(length * 2)))) * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
    }
  }
}

/*
** Draws a solid rectangle from x1, y1 to x2, y2.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
** Color is argb hex value. (  aarrggbb)
**                          (0x00000000)
*/
void basicRenderer::rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      if ((x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(unsigned int*)(x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
      }
    }
  }
}

/*
** Clears the screen.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
*/
void basicRenderer::cls() {
  for (int x = 0; x <= buffer->Width - 1; x++) {
    for (int y = 0; y <= buffer->Height - 1; y++) {
      if ((x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(uint32_t*)(x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = 0x00000000;
      }
    }
  }
}

void basicRenderer::fill(uint32_t color) {
  for (int x = 0; x <= buffer->Width - 1; x++) {
    for (int y = 0; y <= buffer->Height - 1; y++) {
      if ((x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(uint32_t*)(x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
      }
    }
  }
}

/*
** Puts a character in the frame buffer at x, y.
** buffer is GOP frame buffer.
** Color is argb hex value. (  aarrggbb)
**                          (0x00000000)
*/
void basicRenderer::printChar(char chr, unsigned int x, unsigned int y, unsigned int color) {
  unsigned int* pixPtr = (unsigned int*)buffer->BaseAddr;
  char* fontPtr = (char*)font->buffer + (chr * font->header->charsize);
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      if ((*fontPtr & (0b10000000 >> (c - x))) > 0) {
        if ((c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
          *(unsigned int*)(c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
        }
      }
    }
    fontPtr++;
  }
}

void basicRenderer::printChar(char chr, unsigned int color) {
  if (cur->x > cur->xm) {
    cur->newLine();
  }
  printChar(chr, cur->x * 8, cur->y * 16, color);
  cur->x++;
}

void basicRenderer::putChar(char chr, unsigned int color) {
  if (cur->x > cur->xm) {
    cur->newLine();
  }
  printChar(chr, cur->x * 8, cur->y * 16, color);
}

void basicRenderer::delChar(unsigned int x, unsigned int y, unsigned int color) {
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      if ((c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(unsigned int*)(c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
      }
    }
  }
}

void basicRenderer::clChar(unsigned int x, unsigned int y, unsigned int color) {
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      if ((c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(unsigned int*)(c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
      }
    }
  }
}

void basicRenderer::printString(const char* str, unsigned int color) {
  char* chr = (char*)str;
  
  while (*chr != 0) {
    if (cur->x > cur->xm) {
      cur->newLine();
    }
    printChar(*chr, color);
    chr++;
  }

}

void basicRenderer::print(const char* str, unsigned int color) {
  char* chr = (char*)str;
  uint8_t length = 0;
  while (*chr != 0) {
    length++;
    chr++;
  }
  if (cur->xm - cur->x < length) {
    cur->newLine();
  }

  chr = (char*)str;
  while (*chr != 0) {
    if (cur->x > cur->xm) {
      cur->newLine();
    } 
    printChar(*chr, color);
    chr++;
  }

}