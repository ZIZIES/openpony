#ifndef CONSOLE_H
#define CONSOLE_H

void serial_init(void);
void kclear(void);
void kset_cursor(int row, int col);
void kputc(char c);
void kputs(const char *s);
void knewline(void);

#endif