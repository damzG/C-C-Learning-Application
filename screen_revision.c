#include "screens.h"
#include "theme.h"
#include "ui.h"
#include <stdio.h>

typedef struct { const char *topic, *prompt, *options[4]; int answer; } RevisionQuestion;
static const RevisionQuestion questions[] = {
 {"C fundamentals", "What does sizeof(char) return in C?", {"1", "2", "4", "It varies"}, 0},
 {"Pointers", "Which operator gets the value pointed to by p?", {"&p", "*p", "->p", "p*"}, 1},
 {"Memory", "Which function releases memory from malloc?", {"delete", "free", "remove", "clear"}, 1},
 {"C++ OOP", "Which keyword enables runtime polymorphism?", {"virtual", "static", "friend", "inline"}, 0},
 {"C++ classes", "A constructor has which return type?", {"void", "class type", "int", "No return type"}, 3},
 {"Embedded", "What is volatile commonly used for?", {"Faster math", "Memory mapped registers", "Dynamic allocation", "Function overloading"}, 1}
};
static int indexQ, score, selected, finished;
static float remaining;
void ScreenRevisionEnter(AppState *app) { (void)app; indexQ=score=selected=finished=0; remaining=300.0f; }
void ScreenRevisionRun(AppState *app, float dt)
{
 char buf[128];
 UiDrawCenteredText("Exam Revision", SCREEN_W/2, 36, 40, RAYWHITE);
 if (!finished && remaining>0) remaining-=dt;
 if (remaining<=0) finished=1;
 snprintf(buf,sizeof(buf),"%02d:%02d",(int)remaining/60,(int)remaining%60);
 DrawRectangleRounded((Rectangle){1010,28,220,58},.25f,8,COL_PANEL);
 UiDrawCenteredText(buf,1120,40,30,COL_CYAN);
 if (!finished) {
   UiDrawCenteredText(TextFormat("Question %d of %d",indexQ+1,(int)(sizeof(questions)/sizeof(questions[0]))),SCREEN_W/2,112,22,COL_MUTED);
   UiDrawCenteredText(questions[indexQ].topic,SCREEN_W/2,154,22,COL_ACCENT);
   UiDrawWrappedText(questions[indexQ].prompt,(Rectangle){180,205,920,70},25,RAYWHITE);
   for(int i=0;i<4;i++) if(UiButton((Rectangle){270,300+i*68,740,52},questions[indexQ].options[i],true,1)) selected=i+1;
   if(selected) {
     if(selected-1==questions[indexQ].answer) score++;
     selected=0;
     if(++indexQ >= (int)(sizeof(questions)/sizeof(questions[0]))) finished=1;
   }
 } else {
   snprintf(buf,sizeof(buf),"Score: %d / %d",score,(int)(sizeof(questions)/sizeof(questions[0])));
   UiDrawCenteredText("Session complete",SCREEN_W/2,160,34,RAYWHITE);
   UiDrawCenteredText(buf,SCREEN_W/2,220,30,COL_CYAN);
   UiDrawWrappedText("Review pointers and memory management, C++ virtual functions and constructors, and embedded volatile registers. Retry to practise again.",(Rectangle){250,300,780,120},22,COL_MUTED);
   if(UiButtonPrimary((Rectangle){430,455,200,54},"Retry",true,1)) ScreenRevisionEnter(app);
 }
 if(UiButton((Rectangle){30,30,120,44},"Back",true,1)||IsKeyPressed(KEY_ESCAPE)) AppGoTo(app,SCREEN_MENU);
}
