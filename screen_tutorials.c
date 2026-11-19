#include "screens.h"
#include "theme.h"
#include "ui.h"

typedef struct { const char *title,*detail,*url; } Tutorial;
static const Tutorial links[]={
 {"C Programming", "Learn C from fundamentals to practical examples · freeCodeCamp", "https://www.youtube.com/results?search_query=freeCodeCamp+C+programming+tutorial"},
 {"C++ Programming", "Modern C++ concepts, classes, STL and projects · freeCodeCamp", "https://www.youtube.com/results?search_query=freeCodeCamp+C%2B%2B+programming+tutorial"},
 {"Embedded Systems", "Microcontrollers, firmware and embedded C tutorials", "https://www.youtube.com/results?search_query=embedded+systems+beginner+tutorial"},
 {"C++ Reference", "Language reference and examples · cppreference", "https://en.cppreference.com/w/"},
 {"Arduino & Embedded", "Hands-on microcontroller lessons · Arduino", "https://www.youtube.com/results?search_query=Arduino+embedded+C+tutorial"}
};
void ScreenTutorialsEnter(AppState *app){(void)app;}
void ScreenTutorialsRun(AppState *app,float dt)
{
 (void)dt; UiDrawCenteredText("Video Tutorials",SCREEN_W/2,42,40,RAYWHITE);
 UiDrawCenteredText("Choose a topic to open learning resources in your browser",SCREEN_W/2,94,18,COL_MUTED);
 for(int i=0;i<5;i++){
   int y=150+i*88;
   DrawRectangleRounded((Rectangle){220,y,840,72},.12f,8,COL_PANEL);
   UiDrawText(links[i].title,245,y+10,22,RAYWHITE);
   UiDrawText(links[i].detail,245,y+42,15,COL_MUTED);
   if(UiButton((Rectangle){890,y+14,145,44},"Open resource",true,1)) OpenURL(links[i].url);
 }
 if(UiButton((Rectangle){30,30,120,44},"Back",true,1)||IsKeyPressed(KEY_ESCAPE)) AppGoTo(app,SCREEN_MENU);
}
