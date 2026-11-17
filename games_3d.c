// 3D knowledge games: Block Drop (tetris-like) and Code Cards.
#include "games_3d.h"
#include "theme.h"
#include "ui.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

static const Game3dInfo INFOS[] = {
    { GAME3D_BLOCK_DROP, "Block Drop 3D",
      "Guide falling blocks in 3D. Clear a line by answering a C/C++ question.", "3D · Tetris" },
    { GAME3D_CODE_CARDS, "Code Cards 3D",
      "Orbit and flip 3D cards to match programming terms with meanings.", "3D · Memory" }
};

int Game3dCount(void) { return ARRAY_SIZE(INFOS); }
const Game3dInfo *Game3dGet(int index)
{
    if (index < 0 || index >= Game3dCount()) return 0;
    return &INFOS[index];
}

static Game3dId active;
static Camera3D camera;
static bool camReady;

static bool BackBtn(void)
{
    return UiButton((Rectangle){ 30, 30, 120, 44 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE);
}

static void EnsureCamera(void)
{
    if (camReady) return;
    camera.position = (Vector3){ 8.0f, 10.0f, 12.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camReady = true;
}

// ===================== Block Drop =====================
#define BD_W 6
#define BD_H 10
#define BD_D 6

static int bdGrid[BD_H][BD_W]; // simplified 2D playfield extruded in 3D
static int bdX, bdY, bdKind;
static float bdFall;
static int bdScore;
static bool bdQuiz;
static int bdQuizQ;
static int bdLives;
static bool bdOver;

typedef struct { const char *q; const char *opts[3]; int correct; } BdQ;
static const BdQ BDQ[] = {
    { "Which frees memory from new[]?", { "delete", "delete[]", "free only" }, 1 },
    { "Valid index for a[5]?", { "5", "0..4", "-1" }, 1 },
    { "LIFO structure?", { "Queue", "Stack", "Tree" }, 1 },
    { "Binary search needs?", { "Sorted data", "Hashing", "GPU" }, 0 },
    { "== vs = ?", { "Both compare", "== compares, = assigns", "Both assign" }, 1 },
    { "O(log n) example?", { "Linear scan", "Binary search", "Bubble sort" }, 1 }
};

static void BDSpawn(void)
{
    bdX = BD_W / 2;
    bdY = BD_H - 1;
    bdKind = rand() % 5;
    bdFall = 0;
}

static void BDEnter(void)
{
    memset(bdGrid, 0, sizeof(bdGrid));
    bdScore = 0; bdLives = 3; bdOver = false; bdQuiz = false;
    BDSpawn();
    EnsureCamera();
    camera.position = (Vector3){ 7, 11, 14 };
    camera.target = (Vector3){ BD_W / 2.0f, BD_H / 3.0f, 0 };
}

static bool BDCollide(int x, int y)
{
    if (x < 0 || x >= BD_W || y < 0) return true;
    if (y >= BD_H) return false;
    return bdGrid[y][x] != 0;
}

static void BDLock(void)
{
    if (bdY >= 0 && bdY < BD_H && bdX >= 0 && bdX < BD_W)
        bdGrid[bdY][bdX] = bdKind + 1;
    // check full row
    for (int y = 0; y < BD_H; y++)
    {
        bool full = true;
        for (int x = 0; x < BD_W; x++) if (!bdGrid[y][x]) { full = false; break; }
        if (full)
        {
            bdQuiz = true;
            bdQuizQ = rand() % ARRAY_SIZE(BDQ);
            // clear row tentatively — restore if quiz failed
            for (int x = 0; x < BD_W; x++) bdGrid[y][x] = 0;
            for (int yy = y; yy < BD_H - 1; yy++)
                for (int x = 0; x < BD_W; x++) bdGrid[yy][x] = bdGrid[yy + 1][x];
            for (int x = 0; x < BD_W; x++) bdGrid[BD_H - 1][x] = 0;
            break;
        }
    }
    BDSpawn();
    if (BDCollide(bdX, bdY)) bdOver = true;
}

static Color BDColor(int k)
{
    if (k < 1 || k > 5) return COL_PANEL;
    switch (k)
    {
    case 1: return ThemeCyan();
    case 2: return ThemeAccent();
    case 3: return ThemeDone();
    case 4: return (Color){255,140,66,255};
    default: return (Color){170,120,255,255};
    }
}

static bool BDRun(float dt)
{
    EnsureCamera();
    UiDrawCenteredText("Block Drop 3D", SCREEN_W / 2, 24, 30, RAYWHITE);
    char hud[64];
    snprintf(hud, sizeof(hud), "Score %d   Lives %d   A/D move  S drop", bdScore, bdLives);
    UiDrawCenteredText(hud, SCREEN_W / 2, 60, 18, COL_MUTED);

    if (!bdOver && !bdQuiz)
    {
        bdFall += dt;
        float speed = 0.55f;
        if (IsKeyDown(KEY_S)) speed = 0.08f;
        if (bdFall >= speed)
        {
            bdFall = 0;
            if (!BDCollide(bdX, bdY - 1)) bdY--;
            else BDLock();
        }
        if (IsKeyPressed(KEY_A) && !BDCollide(bdX - 1, bdY)) bdX--;
        if (IsKeyPressed(KEY_D) && !BDCollide(bdX + 1, bdY)) bdX++;
    }

    // Orbit camera slowly
    float t = (float)GetTime() * 0.15f;
    camera.position = (Vector3){ BD_W / 2.0f + cosf(t) * 12.0f, 10.0f, sinf(t) * 12.0f };
    camera.target = (Vector3){ BD_W / 2.0f, 4.0f, 0.0f };

    BeginMode3D(camera);
    DrawGrid(16, 1.0f);
    for (int y = 0; y < BD_H; y++)
        for (int x = 0; x < BD_W; x++)
            if (bdGrid[y][x])
                DrawCube((Vector3){ (float)x, (float)y, 0 }, 0.9f, 0.9f, 0.9f, BDColor(bdGrid[y][x]));
    if (!bdOver)
        DrawCube((Vector3){ (float)bdX, (float)bdY, 0 }, 0.9f, 0.9f, 0.9f, COL_ACCENT);
    // board floor
    DrawCube((Vector3){ BD_W / 2.0f - 0.5f, -0.6f, 0 }, (float)BD_W, 0.2f, 2.0f, Fade(COL_LOCKED, 0.8f));
    EndMode3D();

    if (bdQuiz)
    {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.55f));
        const BdQ *q = &BDQ[bdQuizQ];
        DrawRectangleRounded((Rectangle){ 240, 160, 800, 360 }, 0.06f, 8, COL_PANEL);
        UiDrawCenteredText("Line clear quiz!", SCREEN_W / 2, 190, 28, COL_ACCENT);
        UiDrawWrappedText(q->q, (Rectangle){ 280, 240, 720, 60 }, 24, RAYWHITE);
        for (int i = 0; i < 3; i++)
        {
            if (UiButton((Rectangle){ 320, 320.0f + i * 60.0f, 640, 50 }, q->opts[i], true, 1.0f))
            {
                if (i == q->correct) { bdScore += 100; UiToast("+100"); }
                else { bdLives--; UiToast("Miss"); if (bdLives <= 0) bdOver = true; }
                bdQuiz = false;
            }
        }
    }

    if (bdOver)
    {
        UiDrawCenteredText("Board topped out", SCREEN_W / 2, 640, 24, COL_ERROR);
        if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 670, 240, 40 }, "Retry", true, 1.0f)) BDEnter();
    }
    return BackBtn();
}

// ===================== Code Cards 3D =====================
#define CC_N 6
static const char *CC_TERMS[CC_N] = { "int", "pointer", "stack", "queue", "class", "O(log n)" };
static const char *CC_MEAN[CC_N] = {
    "whole number type", "stores a memory address", "LIFO structure",
    "FIFO structure", "object blueprint", "binary search time"
};
static int ccOrder[CC_N * 2];
static bool ccMatched[CC_N * 2];
static int ccFirst, ccSecond;
static float ccFlipT;
static int ccMoves, ccFound;
static float ccAngle;

static void CCEnter(void)
{
    for (int i = 0; i < CC_N * 2; i++) ccOrder[i] = i;
    for (int i = CC_N * 2 - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int t = ccOrder[i]; ccOrder[i] = ccOrder[j]; ccOrder[j] = t;
    }
    memset(ccMatched, 0, sizeof(ccMatched));
    ccFirst = ccSecond = -1;
    ccFlipT = 0; ccMoves = 0; ccFound = 0; ccAngle = 0;
    EnsureCamera();
}

static int CCPair(int pos) { return ccOrder[pos] / 2; }
static int CCSide(int pos) { return ccOrder[pos] % 2; }

static bool CCRun(float dt)
{
    EnsureCamera();
    ccAngle += dt * 0.25f;
    camera.position = (Vector3){ cosf(ccAngle) * 14.0f, 9.0f, sinf(ccAngle) * 14.0f };
    camera.target = (Vector3){ 0, 0, 0 };

    UiDrawCenteredText("Code Cards 3D", SCREEN_W / 2, 24, 30, RAYWHITE);
    char st[64];
    snprintf(st, sizeof(st), "Moves %d   Pairs %d/%d   Click a card in the front view", ccMoves, ccFound, CC_N);
    UiDrawCenteredText(st, SCREEN_W / 2, 60, 18, COL_MUTED);

    if (ccSecond >= 0)
    {
        ccFlipT -= dt;
        if (ccFlipT <= 0)
        {
            if (CCPair(ccFirst) == CCPair(ccSecond))
            {
                ccMatched[ccFirst] = ccMatched[ccSecond] = true;
                ccFound++;
                if (ccFound >= CC_N) UiToast("All pairs matched!");
            }
            ccFirst = ccSecond = -1;
        }
    }

    BeginMode3D(camera);
    DrawGrid(20, 1.0f);
    for (int i = 0; i < CC_N * 2; i++)
    {
        int row = i / 4, col = i % 4;
        Vector3 pos = { (col - 1.5f) * 2.4f, 1.0f, (row - 1.0f) * 2.8f };
        bool up = ccMatched[i] || i == ccFirst || i == ccSecond;
        Color c = ccMatched[i] ? COL_DONE : (up ? COL_CYAN : COL_PANEL);
        DrawCube(pos, 2.0f, 0.2f, 1.4f, c);
        DrawCubeWires(pos, 2.0f, 0.2f, 1.4f, RAYWHITE);
    }
    EndMode3D();

    // 2D hit targets mapped to card indices for reliable picking
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < CC_N * 2; i++)
    {
        int row = i / 4, col = i % 4;
        Rectangle r = { 280.0f + col * 180.0f, 420.0f + row * 70.0f, 160, 58 };
        bool face = ccMatched[i] || i == ccFirst || i == ccSecond;
        DrawRectangleRounded(r, 0.15f, 6, face ? COL_PANEL : COL_LOCKED);
        DrawRectangleRoundedLinesEx(r, 0.15f, 6, 2, CheckCollisionPointRec(mouse, r) ? COL_ACCENT : COL_MUTED);
        if (face)
        {
            int p = CCPair(i);
            const char *txt = CCSide(i) == 0 ? CC_TERMS[p] : CC_MEAN[p];
            UiDrawCenteredText(txt, (int)(r.x + r.width / 2), (int)r.y + 16, 16, RAYWHITE);
        }
        else UiDrawCenteredText("?", (int)(r.x + r.width / 2), (int)r.y + 14, 22, COL_MUTED);

        if (ccSecond < 0 && !ccMatched[i] && CheckCollisionPointRec(mouse, r) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (ccFirst < 0) ccFirst = i;
            else if (ccFirst != i)
            {
                ccSecond = i;
                ccMoves++;
                ccFlipT = 0.85f;
            }
        }
    }

    if (ccFound >= CC_N)
    {
        UiDrawCenteredText("Deck cleared!", SCREEN_W / 2, 390, 24, COL_DONE);
        if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 660, 240, 40 }, "Shuffle again", true, 1.0f)) CCEnter();
    }
    return BackBtn();
}

void Game3dEnter(Game3dId id)
{
    active = id;
    srand((unsigned)time(NULL));
    camReady = false;
    if (id == GAME3D_BLOCK_DROP) BDEnter();
    else CCEnter();
}

bool Game3dRun(AppState *app, float dt)
{
    (void)app;
    if (active == GAME3D_BLOCK_DROP) return BDRun(dt);
    return CCRun(dt);
}
