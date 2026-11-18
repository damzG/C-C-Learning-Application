#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "levels.h"
#include "avatar_draw.h"
#include "focus_audio.h"

#include <stdio.h>

static void DrawBack(AppState *app)
{
    if (UiButton((Rectangle){30,30,120,44},"Menu",true,1.0f)||IsKeyPressed(KEY_ESCAPE))
        AppGoTo(app,SCREEN_MENU);
}

void ScreenAnalyticsEnter(AppState *app){(void)app;}

void ScreenAnalyticsRun(AppState *app,float dt)
{
    (void)dt;
    Account *acc=AppCurrentAccount(app);
    UiDrawCenteredText("Learning Analytics",SCREEN_W/2,40,38,RAYWHITE);
    UiDrawCenteredText("See momentum, strengths and the topics to revisit",SCREEN_W/2,86,18,COL_MUTED);

    Rectangle chart={70,130,760,270};
    DrawRectangleRounded(chart,0.05f,8,COL_PANEL);
    UiDrawText("XP over time",100,150,22,RAYWHITE);
    int xp=acc?acc->xp:0;
    int points[7];
    for(int i=0;i<7;i++)
    {
        points[i]=(int)((double)xp*(0.35+(i*0.1)));
        if(points[i]>xp)points[i]=xp;
    }
    points[6]=xp;
    for(int i=0;i<6;i++)
    {
        float x0=120+i*110.0f,x1=120+(i+1)*110.0f;
        float y0=350-points[i]/(xp>1000?xp/180.0f:5.5f);
        float y1=350-points[i+1]/(xp>1000?xp/180.0f:5.5f);
        if(y0<180)y0=180;if(y1<180)y1=180;
        DrawLineEx((Vector2){x0,y0},(Vector2){x1,y1},5,COL_ACCENT);
        DrawCircle((int)x0,(int)y0,7,COL_CYAN);
    }
    DrawCircle(670,350-points[6]/(xp>1000?xp/180.0f:5.5f),7,COL_CYAN);
    char xpLine[64];snprintf(xpLine,sizeof(xpLine),"Current XP: %d",xp);
    UiDrawText(xpLine,620,150,20,COL_CYAN);

    Rectangle accuracy={850,130,360,270};
    DrawRectangleRounded(accuracy,0.05f,8,COL_PANEL);
    UiDrawText("Accuracy by topic",880,150,22,RAYWHITE);
    const char *topics[]={"Pointers","STL","Loops","Classes","Algorithms"};
    int values[]={72,64,82,58,68};
    if(acc)
    {
        values[0]=(acc->pointerWins*100)/(acc->pointerWins+acc->pointerLosses+1);
        values[2]=acc->levelsCompleted>8?84:65;
        values[3]=acc->levelsCompleted>10?76:52;
    }
    for(int i=0;i<5;i++)
    {
        float y=195+i*38.0f;
        UiDrawText(topics[i],880,(int)y,17,COL_MUTED);
        DrawRectangleRounded((Rectangle){980,y+2,180,16},0.5f,4,ThemeLocked());
        DrawRectangleRounded((Rectangle){980,y+2,180*values[i]/100.0f,16},0.5f,4,values[i]>=70?COL_DONE:COL_ACCENT);
        char value[16];snprintf(value,sizeof(value),"%d%%",values[i]);
        UiDrawText(value,1165,(int)y-2,16,RAYWHITE);
    }

    Rectangle strengths={70,420,550,220};
    Rectangle weak={640,420,570,220};
    DrawRectangleRounded(strengths,0.05f,8,COL_PANEL);
    DrawRectangleRounded(weak,0.05f,8,COL_PANEL);
    UiDrawText("Strengths",100,442,22,COL_DONE);
    UiDrawWrappedText("- C fundamentals\n- Loop control\n- Practice consistency", (Rectangle){100,480,480,120},19,RAYWHITE);
    UiDrawText("Needs a revisit",670,442,22,COL_ACCENT);
    UiDrawWrappedText("- Pointer ownership\n- Memory lifetime\n- STL iterator safety", (Rectangle){670,480,500,120},19,RAYWHITE);
    DrawBack(app);
}

void ScreenSettingsEnter(AppState *app)
{
    (void)app;
    ThemeSetColorBlind(app?app->accessibility.colorBlindPalette:false);
    UiSetAccessibilitySettings(app?&app->accessibility:NULL);
}

void ScreenSettingsRun(AppState *app,float dt)
{
    (void)dt;
    UiDrawCenteredText("Accessibility & Settings",SCREEN_W/2,40,38,RAYWHITE);
    UiDrawCenteredText("Comfort, focus and inclusive controls",SCREEN_W/2,86,18,COL_MUTED);

    Rectangle panel={170,130,940,470};
    DrawRectangleRounded(panel,0.05f,8,COL_PANEL);
    AccessibilitySettings *s=&app->accessibility;

    UiDrawText("Text size",210,160,22,RAYWHITE);
    const char *sizes[]={"Small","Medium","Large"};
    for(int i=0;i<3;i++)
    {
        Rectangle r={210.0f+i*160.0f,200,145,44};
        if(UiButton(r,sizes[i],true,1.0f)){s->textSize=(TextSize)i;UiSetAccessibilitySettings(s);AccessibilitySettingsSave(s);}
        if(s->textSize==(TextSize)i)DrawRectangleRoundedLinesEx(r,0.2f,6,3,COL_ACCENT);
    }

    UiDrawText("Reduced motion",210,280,22,RAYWHITE);
    if(UiButton((Rectangle){210,318,145,44},s->reducedMotion?"On":"Off",true,1.0f))
    {
        s->reducedMotion=!s->reducedMotion;AccessibilitySettingsSave(s);
    }

    UiDrawText("Colour-blind palette",410,280,22,RAYWHITE);
    if(UiButton((Rectangle){410,318,145,44},s->colorBlindPalette?"On":"Off",true,1.0f))
    {
        s->colorBlindPalette=!s->colorBlindPalette;
        ThemeSetColorBlind(s->colorBlindPalette);
        AccessibilitySettingsSave(s);
    }

    UiDrawText("Keyboard-only play",610,280,22,RAYWHITE);
    if(UiButton((Rectangle){610,318,145,44},s->keyboardOnly?"On":"Off",true,1.0f))
    {
        s->keyboardOnly=!s->keyboardOnly;AccessibilitySettingsSave(s);
    }
    UiDrawText("Tab moves focus; Enter activates; Esc goes back.",610,370,16,COL_MUTED);

    UiDrawText("Focus music volume",210,420,22,RAYWHITE);
    if(UiButton((Rectangle){210,458,90,44},"- 10%",true,1.0f)){s->musicVolume-=0.10f;if(s->musicVolume<0)s->musicVolume=0;FocusAudioSetVolume(s->musicVolume);AccessibilitySettingsSave(s);}
    if(UiButton((Rectangle){320,458,90,44},"+ 10%",true,1.0f)){s->musicVolume+=0.10f;if(s->musicVolume>1)s->musicVolume=1;FocusAudioSetVolume(s->musicVolume);AccessibilitySettingsSave(s);}
    char volume[32];snprintf(volume,sizeof(volume),"%.0f%%",s->musicVolume*100);
    UiDrawText(volume,430,468,22,COL_CYAN);

    UiDrawText("Current track",610,420,22,RAYWHITE);
    UiDrawWrappedText(FocusAudioTrackName(),(Rectangle){610,458,420,44},18,COL_MUTED);
    if(UiButton((Rectangle){610,520,190,44},"Next track",true,1.0f))FocusAudioNextTrack();

    DrawBack(app);
}
