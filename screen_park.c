#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "avatar_draw.h"
#include "panda.h"
#include "park_bg.h"
#include "levels.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
    PARK_DASHBOARD,
    PARK_PLAYGROUND,
    PARK_RESUME,
    PARK_CAREERS,
    PARK_POINTER,
    PARK_GAMES,
    PARK_CHALLENGES,
    PARK_COMMUNITY
} ParkMode;

static ParkMode mode;
static UiTextBox playgroundCode;
static UiTextBox communityPost;
static int pointerRound;
static int pointerSelected;
static bool pointerAnswer;
static int chessPiece[64];
static int chessSelected;
static int chessTurn;
static int chessMoves;
static int chessWinner;
static int scrabbleScore;

static const char *MOTIVATION[] = {
    "The function of good software is to make the complex appear simple. - Grady Booch",
    "Talk is cheap. Show me the code. - Linus Torvalds",
    "First, solve the problem. Then, write the code. - John Johnson",
    "Computers are fast; developers keep them from going too fast. - Grace Hopper",
    "Simplicity is the soul of efficiency. - Austin Freeman"
};

static const char *POINTER_QUESTIONS[] = {
    "Which expression gets the address of x?",
    "Which operator dereferences a pointer?",
    "What frees memory allocated by new[]?",
    "What is a dangling pointer?"
};

static const char *POINTER_OPTIONS[][4] = {
    {"&x", "*x", "x*", "nullptr"},
    {"*", "&", "->", "[]"},
    {"delete[] p", "delete p", "free(p)", "malloc(p)"},
    {"A pointer to freed memory", "A null pointer", "A function pointer", "A pointer to const"}
};
static const int POINTER_CORRECT[] = {0, 0, 0, 0};

static int BadgeBitsCount(const Account *acc)
{
    int count = 0;
    for (int i = 0; i < BadgeCount(); i++)
        if (AccountHasBadge(acc, BadgeBitAt(i))) count++;
    return count;
}

void ScreenParkEnter(AppState *app)
{
    (void)app;
    mode = PARK_DASHBOARD;
    pointerRound = 0;
    pointerSelected = -1;
    pointerAnswer = false;
    chessSelected = -1;
    chessTurn = 1;
    chessMoves = 0;
    chessWinner = 0;
    scrabbleScore = 0;
    memset(chessPiece, 0, sizeof(chessPiece));
    for (int c = 0; c < 8; c++)
    {
        chessPiece[8 + c] = 1;
        chessPiece[48 + c] = -1;
    }
    chessPiece[0] = 2; chessPiece[7] = 2;
    chessPiece[56] = -2; chessPiece[63] = -2;
    chessPiece[4] = 3;
    chessPiece[60] = -3;
    UiTextBoxInit(&playgroundCode, (Rectangle){100, 170, 720, 430}, "Mini project: C++ playground",
                  "// Write code here, then Build & Run");
    UiTextBoxInit(&communityPost, (Rectangle){100, 560, 760, 44}, "Share a question or joke",
                  "How did this C++ bug happen?");
}

static void DrawTabs(void)
{
    const char *tabs[] = {"Dashboard","PlayGround","Resume","Careers","Pointer","Games","Challenges","Community"};
    for (int i = 0; i < 8; i++)
    {
        Rectangle r = {30.0f + i * 152.0f, 18, 144, 42};
        if (UiButton(r, tabs[i], true, 1.0f)) mode = (ParkMode)i;
        if (i == (int)mode) DrawRectangleRoundedLinesEx(r, 0.2f, 6, 3.0f, COL_ACCENT);
    }
}

static void DrawDashboard(AppState *app)
{
    Account *acc = AppCurrentAccount(app);
    UiDrawCenteredText("C^2 Park Dashboard", SCREEN_W / 2, 76, 36, RAYWHITE);

    if (!acc)
    {
        UiDrawCenteredText("Create a profile to collect badges and streaks", SCREEN_W / 2, 200, 22, COL_MUTED);
        if (UiButtonPrimary((Rectangle){480, 260, 320, 52}, "Create Profile", true, 1.0f))
            AppGoTo(app, SCREEN_CREATE);
        return;
    }

    AvatarDraw(acc->username, 80, 130, 110, 1.0f);
    UiDrawText(acc->fullName, 210, 138, 28, RAYWHITE);
    char stats[128];
    snprintf(stats, sizeof(stats), "@%s  |  %d XP  |  %d day streak", acc->username, acc->xp, acc->streakDays);
    UiDrawText(stats, 210, 178, 20, COL_CYAN);
    if (acc->streakDays > 0)
    {
        float fx = 590.0f, fy = 176.0f;
        DrawTriangle((Vector2){fx,fy+18},(Vector2){fx-10,fy+4},(Vector2){fx-3,fy+5},(Color){255,140,66,255});
        DrawTriangle((Vector2){fx,fy+18},(Vector2){fx+10,fy+5},(Vector2){fx+3,fy+6},(Color){255,211,0,255});
        DrawTriangle((Vector2){fx,fy+18},(Vector2){fx-4,fy},(Vector2){fx+4,fy},(Color){255,99,110,255});
        UiDrawText("FIRE STREAK", (int)fx + 18, (int)fy - 2, 16, COL_ACCENT);
    }

    Rectangle cards[] = {{210,220,300,112},{530,220,300,112},{850,220,300,112}};
    const char *labels[] = {"ROADMAP","POINTER RECORD","BADGES"};
    char values[3][64];
    snprintf(values[0], sizeof(values[0]), "%d / %d levels", acc->levelsCompleted, LevelCount());
    snprintf(values[1], sizeof(values[1]), "%d wins  |  %d losses", acc->pointerWins, acc->pointerLosses);
    snprintf(values[2], sizeof(values[2]), "%d earned", BadgeBitsCount(acc));

    for (int i = 0; i < 3; i++)
    {
        DrawRectangleRounded(cards[i], 0.08f, 8, COL_PANEL);
        UiDrawText(labels[i], (int)cards[i].x + 20, (int)cards[i].y + 18, 16, COL_MUTED);
        UiDrawText(values[i], (int)cards[i].x + 20, (int)cards[i].y + 52, 24, RAYWHITE);
    }

    UiDrawCenteredText("Daily quests", SCREEN_W / 2, 370, 24, RAYWHITE);
    const char *questNames[] = {"Complete a lesson","Solve a practice problem","Build in PlayGround","Win a Pointer round"};
    for (int i = 0; i < 4; i++)
    {
        Rectangle r = {80.0f + i * 290.0f, 410, 270, 64};
        bool done = AccountQuestDone(acc, i);
        DrawRectangleRounded(r, 0.08f, 8, done ? (Color){30,100,70,255} : COL_PANEL);
        UiDrawCenteredText(done ? "Completed" : questNames[i], (int)(r.x+r.width/2), (int)r.y+20, 18, done ? COL_DONE : RAYWHITE);
        if (!done && i == 2 && UiButton((Rectangle){r.x+55,r.y+38,160,22},"Claim +50 XP",true,1.0f))
        {
            AccountCompleteQuest(acc, i, 50);
            AccountAwardBadge(acc, BADGE_PLAYGROUND);
            AppSaveAccounts(app);
        }
    }

    // Locked certificate preview stays visible before the roadmap is complete.
    Rectangle cert={80,520,720,92};
    bool graduated=acc->levelsCompleted >= LevelCount();
    DrawRectangleRounded(cert,0.05f,8,graduated?(Color){65,55,105,255}:Fade(COL_LOCKED,0.55f));
    DrawRectangleRoundedLinesEx(cert,0.05f,8,2,graduated?COL_ACCENT:Fade(COL_MUTED,0.25f));
    UiDrawCenteredText(graduated?"Certificate: C^2 C/C++ Roadmap Graduate":"LOCKED: C^2 Roadmap Graduate Certificate",
                      (int)(cert.x+cert.width/2),536,22,graduated?COL_ACCENT:Fade(COL_MUTED,0.65f));
    char certLine[128]; char initials[8];
    AccountInitials(acc, initials);
    if(graduated) snprintf(certLine,sizeof(certLine),"Awarded to %s  |  Initials %s  |  Official C^2 Seal",acc->fullName,initials);
    else snprintf(certLine,sizeof(certLine),"%d / %d levels complete - finish the rollercoaster to unlock",acc->levelsCompleted,LevelCount());
    UiDrawCenteredText(certLine,(int)(cert.x+cert.width/2),578,17,graduated?RAYWHITE:Fade(COL_MUTED,0.55f));

    // Badge cabinet: earned badges are bright; upcoming badges remain clearly visible and locked.
    Rectangle badgeCab={820,520,380,92};
    DrawRectangleRounded(badgeCab,0.05f,8,Fade(COL_LOCKED,0.45f));
    UiDrawText("BADGE CABINET",840,532,16,Fade(COL_MUTED,0.75f));
    for(int i=0;i<6;i++)
    {
        unsigned bit=BadgeBitAt(i);
        bool earned=AccountHasBadge(acc,bit);
        Vector2 c={850.0f+i*55.0f,578.0f};
        DrawCircleV(c,18,earned?COL_ACCENT:Fade(COL_LOCKED,0.7f));
        DrawCircleLines((int)c.x,(int)c.y,18,earned?COL_ACCENT:Fade(COL_MUTED,0.35f));
        char num[4];snprintf(num,sizeof(num),"%d",i+1);
        UiDrawCenteredText(num,(int)c.x,(int)c.y-10,16,earned?COL_BG:Fade(COL_MUTED,0.45f));
    }

    UiDrawWrappedText(MOTIVATION[acc->xp % 5], (Rectangle){80,632,1120,60}, 18, COL_MUTED);
}

static void DrawPlayground(AppState *app)
{
    UiDrawCenteredText("PlayGround", SCREEN_W / 2, 78, 36, RAYWHITE);
    UiDrawCenteredText("Write, build and test mini C++ projects", SCREEN_W / 2, 118, 20, COL_MUTED);
    UiTextBoxUpdate(&playgroundCode);
    UiTextBoxDraw(&playgroundCode, true);

    if (UiButtonPrimary((Rectangle){850,170,330,52},"Build & Run",true,1.0f))
    {
        UiToast("PlayGround build check: code saved for your project");
        Account *acc = AppCurrentAccount(app);
        if (acc)
        {
            acc->playgroundDone++;
            acc->xp += 25;
            AccountTouchActivity(acc);
            AccountAwardBadge(acc, BADGE_PLAYGROUND);
            AccountCompleteQuest(acc, 2, 50);
            AppSaveAccounts(app);
        }
    }
    DrawRectangleRounded((Rectangle){850,240,330,360},0.06f,8,COL_PANEL);
    UiDrawText("Console",870,260,22,COL_CYAN);
    UiDrawMono("Build: ready",870,310,18,COL_DONE);
    UiDrawMono("Run:   no errors",870,340,18,COL_DONE);
    UiDrawWrappedText("Tip: use std::vector, std::string and RAII.\n\nCompleted projects award XP and decorate your profile.",
                      (Rectangle){870,390,290,180},18,COL_MUTED);
    PandaDraw(1080,520,82,PANDA_THINK,1.0f);
}

static void DrawResume(AppState *app)
{
    Account *acc = AppCurrentAccount(app);
    UiDrawCenteredText("C/C++ & Embedded Resume Builder",SCREEN_W/2,78,34,RAYWHITE);
    UiDrawCenteredText("ATS preview and developer score",SCREEN_W/2,118,20,COL_MUTED);

    if (!acc)
    {
        UiDrawCenteredText("Create a profile first",SCREEN_W/2,300,24,COL_MUTED);
        return;
    }

    int score = 48;
    if (acc->fullName[0]) score += 12;
    if (acc->email[0]) score += 10;
    if (acc->course[0]) score += 12;
    if (acc->levelsCompleted > 5) score += 18;
    if (acc->levelsCompleted >= LevelCount()) score += 20;
    if (score > 100) score = 100;

    DrawRectangleRounded((Rectangle){100,160,360,420},0.06f,8,COL_PANEL);
    UiDrawCenteredText("ATS SCORE",280,190,20,COL_MUTED);
    DrawRing((Vector2){280,300},72,82,0,360,64,(Color){65,74,110,255});
    DrawRing((Vector2){280,300},72,82,270,270+score*3.6f,64,score>=75?COL_DONE:COL_ACCENT);
    UiDrawCenteredText(TextFormat("%d%%",score),280,278,44,RAYWHITE);

    Rectangle resume={500,160,680,420};
    DrawRectangleRounded(resume,0.06f,8,RAYWHITE);
    UiDrawText(acc->fullName,530,190,30,(Color){28,30,40,255});
    char contact[180]; snprintf(contact,sizeof(contact),"%s | %s",acc->email,acc->course);
    UiDrawText(contact,530,230,16,(Color){70,75,95,255});
    UiDrawText("C/C++ DEVELOPER",530,290,24,(Color){28,30,40,255});
    UiDrawWrappedText("Strong fundamentals in C/C++, pointers, memory management, data structures and embedded concepts.\n"
                      "Roadmap XP: + completed lesson and coding challenge milestones.",
                      (Rectangle){530,330,600,120},18,(Color){70,75,95,255});
    UiDrawText("SKILLS",530,470,20,(Color){28,30,40,255});
    UiDrawText("C  |  C++  |  STL  |  Embedded  |  Git  |  Debugging",530,500,18,(Color){70,75,95,255});

    UiDrawCenteredText(score>=75?"ATS ready: strong keyword coverage":"Add projects and completed lessons to improve your ATS score",
                      640,610,18,score>=75?COL_DONE:COL_ACCENT);
}

static void DrawCareers(void)
{
    UiDrawCenteredText("C/C++ Career Portals",SCREEN_W/2,78,36,RAYWHITE);
    UiDrawCenteredText("Student opportunities, societies and C/C++ employers",SCREEN_W/2,118,20,COL_MUTED);
    const char *names[]={"Tech Nation","GitHub Careers","Embedded.com Jobs","CPlusPlus Jobs","Google Students","MLH Fellowships"};
    const char *urls[]={"https://www.technation.io/","https://github.com/about/careers","https://www.embedded.com/jobs/",
                        "https://cplusplusjob.com/","https://careers.google.com/students/","https://fellowship.mlh.io/"};
    for(int i=0;i<6;i++)
    {
        int col=i%3,row=i/3;
        Rectangle r={110.0f+col*390.0f,170.0f+row*190.0f,350,150};
        bool h=CheckCollisionPointRec(GetMousePosition(),r);
        DrawRectangleRounded(r,0.08f,8,COL_PANEL);
        DrawRectangleRoundedLinesEx(r,0.08f,8,3.0f,h?COL_ACCENT:Fade(COL_MUTED,0.35f));
        UiDrawCenteredText(names[i],(int)(r.x+r.width/2),(int)r.y+38,22,RAYWHITE);
        UiDrawCenteredText("Open careers portal",(int)(r.x+r.width/2),(int)r.y+92,18,COL_CYAN);
        if(h && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) OpenURL(urls[i]);
    }
}

static void DrawPointer(AppState *app)
{
    Account *acc=AppCurrentAccount(app);
    UiDrawCenteredText("Pointer Rounds",SCREEN_W/2,78,36,RAYWHITE);
    UiDrawCenteredText("One coder vs the AI - answer quickly and protect your rank",SCREEN_W/2,118,20,COL_MUTED);
    if(pointerRound>=4) pointerRound=0;

    DrawRectangleRounded((Rectangle){180,170,920,400},0.06f,8,COL_PANEL);
    UiDrawCenteredText(POINTER_QUESTIONS[pointerRound],640,200,24,RAYWHITE);
    for(int i=0;i<4;i++)
    {
        Rectangle r={260.0f+(i%2)*400.0f,280.0f+(i/2)*90.0f,360,64};
        bool selected=pointerSelected==i;
        if(UiButton(r,POINTER_OPTIONS[pointerRound][i],true,1.0f) && !pointerAnswer)
        {
            pointerSelected=i;
            pointerAnswer=true;
            if(acc)
            {
                if(i==POINTER_CORRECT[pointerRound])
                {
                    acc->pointerWins++;
                    acc->xp+=30;
                    if(acc->pointerWins>acc->pointerBest) acc->pointerBest=acc->pointerWins;
                    AccountAwardBadge(acc,BADGE_POINTER);
                    AccountCompleteQuest(acc,3,50);
                    UiToast("Pointer Round won! +30 XP");
                }
                else
                {
                    acc->pointerLosses++;
                    UiToast("AI countered that answer");
                }
                AccountTouchActivity(acc);
                AppSaveAccounts(app);
            }
        }
        if(selected) DrawRectangleRoundedLinesEx(r,0.2f,6,3.0f, i==POINTER_CORRECT[pointerRound]?COL_DONE:COL_ERROR);
    }
    if(pointerAnswer)
    {
        UiDrawCenteredText(pointerSelected==POINTER_CORRECT[pointerRound]?"Correct - you beat the AI!":"The AI wins this round",
                           640,500,22,pointerSelected==POINTER_CORRECT[pointerRound]?COL_DONE:COL_ERROR);
        if(UiButtonPrimary((Rectangle){500,530,280,52},"Next Round",true,1.0f))
        {
            pointerRound=(pointerRound+1)%4;pointerSelected=-1;pointerAnswer=false;
        }
    }
    if(acc)
    {
        char rec[100];snprintf(rec,sizeof(rec),"Record: %d wins - %d losses  |  Best: %d",acc->pointerWins,acc->pointerLosses,acc->pointerBest);
        UiDrawCenteredText(rec,640,620,20,COL_CYAN);
    }
}

static void ChessReset(void)
{
    memset(chessPiece,0,sizeof(chessPiece));
    for(int c=0;c<8;c++){chessPiece[8+c]=1;chessPiece[48+c]=-1;}
    chessPiece[0]=2;chessPiece[7]=2;chessPiece[56]=-2;chessPiece[63]=-2;
    chessPiece[4]=3;chessPiece[60]=-3;
    chessSelected=-1;chessTurn=1;chessMoves=0;chessWinner=0;
}

static bool ChessIsLegal(int from,int to)
{
    int p=chessPiece[from];
    if(p==0||((p>0)!=(chessTurn>0))) return false;
    int fr=from/8,fc=from%8,tr=to/8,tc=to%8;
    int target=chessPiece[to];
    if(target!=0&&((target>0)==(p>0))) return false;
    int kind=p<0?-p:p;
    if(kind==1)
    {
        int dir=p>0?-1:1;
        if(fc==tc&&target==0)
        {
            if(tr==fr+dir) return true;
            int start=p>0?6:1;
            return fr==start&&tr==fr+dir*2&&chessPiece[(fr+dir)*8+fc]==0;
        }
        if(abs(tc-fc)==1&&tr==fr+dir) return target!=0;
        return false;
    }
    if(kind==2)
    {
        if(fr!=tr&&fc!=tc) return false;
        int dr=(tr>fr)-(tr<fr),dc=(tc>fc)-(tc<fc);
        int r=fr+dr,c=fc+dc;
        while(r!=tr||c!=tc){if(chessPiece[r*8+c]!=0)return false;r+=dr;c+=dc;}
        return true;
    }
    if(kind==3) return abs(tr-fr)<=1&&abs(tc-fc)<=1;
    return false;
}

static const char *ChessPieceText(int piece)
{
    int kind=piece<0?-piece:piece;
    if(kind==1)return "Pawn";
    if(kind==2)return "Rook";
    if(kind==3)return "King";
    return "";
}

static void DrawGames(AppState *app)
{
    UiDrawCenteredText("Amusement Park Games",SCREEN_W/2,78,36,RAYWHITE);
    Rectangle chess={70,150,700,470};
    Rectangle scrabble={800,150,380,470};
    DrawRectangleRounded(chess,0.05f,8,COL_PANEL);
    UiDrawCenteredText("Chess - learn while you relax",410,165,24,RAYWHITE);
    float side=44,x0=155,y0=210;
    Vector2 mouse=GetMousePosition();
    for(int row=0;row<8;row++) for(int col=0;col<8;col++)
    {
        int idx=row*8+col;
        Rectangle r={x0+col*side,y0+row*side,side,side};
        DrawRectangleRec(r,(row+col)%2?(Color){70,82,125,255}: (Color){220,220,205,255});
        if(chessPiece[idx]>0)
            UiDrawCenteredText(ChessPieceText(chessPiece[idx]),(int)(r.x+r.width/2),(int)r.y+10,14,(Color){35,40,65,255});
        if(chessPiece[idx]<0)
            UiDrawCenteredText(ChessPieceText(chessPiece[idx]),(int)(r.x+r.width/2),(int)r.y+10,14,COL_ERROR);
        if(chessSelected==idx) DrawRectangleRoundedLinesEx(r,0.05f,4,3,COL_ACCENT);
        else if(chessSelected>=0&&ChessIsLegal(chessSelected,idx))
            DrawCircle((int)(r.x+r.width/2),(int)(r.y+r.height/2),6,Fade(COL_DONE,0.75f));
        if(CheckCollisionPointRec(mouse,r)&&IsMouseButtonReleased(MOUSE_BUTTON_LEFT)&&chessWinner==0)
        {
            if(chessSelected<0)
            {
                if(chessPiece[idx]!=0&&((chessPiece[idx]>0)==(chessTurn>0))) chessSelected=idx;
            }
            else
            {
                if(ChessIsLegal(chessSelected,idx))
                {
                    int captured=chessPiece[idx];
                    chessPiece[idx]=chessPiece[chessSelected];
                    chessPiece[chessSelected]=0;
                    chessMoves++;
                    if(captured<0&&-captured==3) chessWinner=1;
                    else if(captured>0&&captured==3) chessWinner=-1;
                    if(chessWinner==0) chessTurn=-chessTurn;
                    Account *acc=AppCurrentAccount(app);
                    if(acc&&chessMoves==1)
                    {
                        AccountAwardBadge(acc,BADGE_CHESS);
                        AppSaveAccounts(app);
                    }
                }
                else if(chessPiece[idx]!=0&&((chessPiece[idx]>0)==(chessTurn>0))) chessSelected=idx;
                else chessSelected=-1;
            }
        }
    }
    char turnText[80];
    if(chessWinner!=0) snprintf(turnText,sizeof(turnText),chessWinner>0?"White wins!":"Black wins!");
    else snprintf(turnText,sizeof(turnText),"Turn: %s  |  Moves: %d",chessTurn>0?"White":"Black",chessMoves);
    UiDrawCenteredText(turnText,410,596,20,chessWinner!=0?COL_DONE:COL_CYAN);
    if(UiButton((Rectangle){230,630,360,44},"Reset Chess Board",true,1.0f)) ChessReset();
    DrawRectangleRounded(scrabble,0.05f,8,COL_PANEL);
    UiDrawCenteredText("Scrabble Sprint",990,165,24,RAYWHITE);
    UiDrawWrappedText("Use the letters:\nC  O  D  E  R\n\nBuild a developer word!",(Rectangle){840,220,300,180},24,RAYWHITE);
    if(UiButtonPrimary((Rectangle){850,430,280,52},"Build WORDS = +72",true,1.0f))
    {
        scrabbleScore+=72;UiToast("Scrabble Star points added!");
        Account *acc=AppCurrentAccount(app);
        if(acc){AccountAwardBadge(acc,BADGE_SCRABBLE);acc->xp+=20;AppSaveAccounts(app);}
    }
    char score[64];snprintf(score,sizeof(score),"Score: %d",scrabbleScore);
    UiDrawCenteredText(score,990,540,24,COL_CYAN);
}

static void DrawChallenges(AppState *app)
{
    UiDrawCenteredText("Daily & Special Challenges",SCREEN_W/2,78,36,RAYWHITE);
    Rectangle cards[]={ {80,150,350,430},{465,150,350,430},{850,150,330,430}};
    const char *titles[]={"Project Euler","Joker Daily","Math Teasers"};
    const char *bodies[]={
        "Challenge 1:\nFind the sum of multiples\nof 3 and 5 below 1000.\n\nAnswer: 233168",
        "Today's Joker code riddle:\nA pointer waves hello:\n*p = 42;\n\nWhat does p store?",
        "1, 1, 2, 3, 5, 8, ?\n\nNext Fibonacci value: 13"
    };
    for(int i=0;i<3;i++)
    {
        DrawRectangleRounded(cards[i],0.06f,8,COL_PANEL);
        UiDrawCenteredText(titles[i],(int)(cards[i].x+cards[i].width/2),(int)cards[i].y+24,24,RAYWHITE);
        UiDrawWrappedText(bodies[i],(Rectangle){cards[i].x+24,cards[i].y+72,cards[i].width-48,280},20,COL_MUTED);
        if(UiButtonPrimary((Rectangle){cards[i].x+50,cards[i].y+360,cards[i].width-100,50},"Claim Badge",true,1.0f))
        {
            Account *acc=AppCurrentAccount(app);
            if(acc)
            {
                acc->xp+=40;
                if(i==0) AccountAwardBadge(acc,BADGE_EULER);
                if(i==1) AccountAwardBadge(acc,BADGE_JOKER);
                if(i==2) AccountAwardBadge(acc,BADGE_MATH);
                AccountTouchActivity(acc);AppSaveAccounts(app);
                UiToast("Challenge complete! +40 XP");
            }
        }
    }
}

static void DrawCommunity(void)
{
    UiDrawCenteredText("C^2 Community Corner",SCREEN_W/2,78,36,RAYWHITE);
    UiDrawCenteredText("Oyindamola Olaosun + Owen Nathanael - two students with a dream to double the C learning experience",
                      SCREEN_W/2,118,18,COL_ACCENT);
    DrawRectangleRounded((Rectangle){80,160,1120,380},0.05f,8,COL_PANEL);
    UiDrawText("Pinned: Welcome to C^2!",110,190,24,RAYWHITE);
    UiDrawWrappedText("A kind place for C/C++ questions, jokes, referrals and progress.\n"
                      "@AdaPointer: What is the safest way to free a linked list?\n"
                      "@HeapHero: Thanks for yesterday's DSA advice!\n"
                      "@CS50Coder: Looking for an embedded systems study partner.",
                      (Rectangle){110,230,1060,240},20,COL_MUTED);
    UiTextBoxUpdate(&communityPost);
    UiTextBoxDraw(&communityPost,true);
    if(UiButtonPrimary((Rectangle){880,560,320,44},"Post to community",true,1.0f))
    {
        if(communityPost.len>0){UiToast("Comment posted to this session");UiTextBoxClear(&communityPost);}
    }
}

void ScreenParkRun(AppState *app,float dt)
{
    (void)dt;
    ParkBgDraw(0.28f);
    DrawTabs();
    if(mode==PARK_DASHBOARD) DrawDashboard(app);
    else if(mode==PARK_PLAYGROUND) DrawPlayground(app);
    else if(mode==PARK_RESUME) DrawResume(app);
    else if(mode==PARK_CAREERS) DrawCareers();
    else if(mode==PARK_POINTER) DrawPointer(app);
    else if(mode==PARK_GAMES) DrawGames(app);
    else if(mode==PARK_CHALLENGES) DrawChallenges(app);
    else DrawCommunity();
    if(UiButton((Rectangle){1140,670,120,38},"Menu",true,1.0f)||IsKeyPressed(KEY_ESCAPE))
        AppGoTo(app,SCREEN_MENU);
}
