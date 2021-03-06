// generated by Fast Light User Interface Designer (fluid) version 1.00

#ifndef keyboard_ui_h
#define keyboard_ui_h
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
extern void key_cb(Fl_Button*, void*);
extern void shift_cb(Fl_Button*, void*);
#include <FL/Fl_Output.H>
extern Fl_Output *key_output;
#include <FL/Fl_Box.H>
extern Fl_Output *text_output;
Fl_Window* make_window();
#endif
