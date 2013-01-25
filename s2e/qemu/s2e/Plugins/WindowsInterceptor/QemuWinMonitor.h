#ifndef QEMU_WIN_MONITOR_H
#define QEMU_WIN_MONITOR_H
#include <stdint.h>

void print_process_info(void);
void win_monitor_print_process_info();
/*
void print_module_info(void);
void win_monitor_print_module_info();

void print_thread_info(uint64_t processaddr);
void win_monitor_print_thread_info(uint64_t processaddr);

void print_functioncall_info(void);
void win_monitor_print_functioncall_info();
*/
#endif
