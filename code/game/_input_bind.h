#if 0
#define EVAL_EXTEND_CONSTANTS \
   for( int vk = input_enum(id), *once = &vk; once; once = 0) \
      if( vk >= 0 ) push(ev, vk);
#define EVAL_EXTEND_FUNCTIONS \
   /**/ if(!strcmp(id, "input") && nargs ==1) push(ev, input(pop(ev))); \
   else if(!strcmp(id, "down")  && nargs ==1) push(ev, input_down(pop(ev))); \
   else if(!strcmp(id, "held")  && nargs ==1) push(ev, input_held(pop(ev))); \
   else if(!strcmp(id, "up")    && nargs ==1) push(ev, input_up(pop(ev))); \
   else if(!strcmp(id, "idle")  && nargs ==1) push(ev, input_idle(pop(ev)));
#endif
#define expr expr2 // 3rd_lua.h
#include "3rd_eval.h" // atof1
