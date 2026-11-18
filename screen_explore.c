// Explore hub: DSA visualisers, CS fundamentals, system design and AI topics.
#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "explore_content.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    EX_HUB,
    EX_TOPICS,
    EX_TOPIC_DETAIL,
    EX_VIZ_PICK,
    EX_VIZ_RUN
} ExMode;

#define VIZ_N 12
#define GRID 8

static ExMode mode;
static ExploreCategory category;
static int topicIndex;
static float topicScroll;
static int vizIndex;

// Shared visualiser state
static int arr[VIZ_N];
static int arrN;
static int vizStep;
static bool vizPlaying;
static float vizTimer;
static float vizSpeed;
static int hiA, hiB, hiPivot;
static int searchTarget;
static int searchLo, searchHi, searchMid;
static bool searchFound;
static int stackData[VIZ_N];
static int stackTop;
static int queueData[VIZ_N];
static int qHead, qTail, qCount;
static int listVals[VIZ_N];
static int listN;
static int bstKeys[VIZ_N];
static int bstN;
static int gridCell[GRID * GRID];
static int bfsQ[GRID * GRID];
static int bfsQh, bfsQt;
static int dfsStack[GRID * GRID];
static int dfsTop;
static int visitOrder[GRID * GRID];
static int visitCount;
static int curCell;

static bool BackBtn(void)
{
    return UiButton((Rectangle){ 30, 30, 120, 44 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE);
}

static void ShuffleArr(void)
{
    arrN = VIZ_N;
    for (int i = 0; i < arrN; i++) arr[i] = i + 1;
    for (int i = arrN - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int t = arr[i]; arr[i] = arr[j]; arr[j] = t;
    }
    vizStep = 0;
    hiA = hiB = hiPivot = -1;
}

static void ResetSearch(void)
{
    arrN = VIZ_N;
    for (int i = 0; i < arrN; i++) arr[i] = (i + 1) * 3;
    searchTarget = arr[rand() % arrN];
    searchLo = 0;
    searchHi = arrN - 1;
    searchMid = -1;
    searchFound = false;
    vizStep = 0;
}

static void ResetStackQueue(void)
{
    stackTop = 0;
    qHead = qTail = qCount = 0;
    listN = 0;
    bstN = 0;
    for (int i = 0; i < 5; i++)
    {
        listVals[listN++] = (i + 1) * 10;
        bstKeys[bstN++] = 0; // filled on demand
    }
    bstN = 0;
    vizStep = 0;
}

static void ResetGraph(void)
{
    memset(gridCell, 0, sizeof(gridCell));
    // walls
    for (int i = 0; i < GRID * GRID; i++)
        if (rand() % 5 == 0) gridCell[i] = -1;
    gridCell[0] = 0;
    gridCell[GRID * GRID - 1] = 0;
    bfsQh = bfsQt = 0;
    dfsTop = 0;
    visitCount = 0;
    curCell = 0;
    bfsQ[bfsQt++] = 0;
    dfsStack[dfsTop++] = 0;
    gridCell[0] = 1; // visited frontier
    vizStep = 0;
}

static void VizReset(VizId id)
{
    vizPlaying = false;
    vizTimer = 0;
    vizSpeed = 0.45f;
    hiA = hiB = hiPivot = -1;
    switch (id)
    {
    case VIZ_BUBBLE_SORT:
    case VIZ_SELECTION_SORT:
    case VIZ_INSERTION_SORT:
        ShuffleArr();
        break;
    case VIZ_BINARY_SEARCH:
        ResetSearch();
        break;
    case VIZ_STACK:
    case VIZ_QUEUE:
    case VIZ_LINKED_LIST:
    case VIZ_BST:
        ResetStackQueue();
        break;
    case VIZ_BFS:
    case VIZ_DFS:
        ResetGraph();
        break;
    default:
        break;
    }
}

void ScreenExploreEnter(AppState *app)
{
    (void)app;
    mode = EX_HUB;
    category = EXPLORE_DSA;
    topicIndex = 0;
    topicScroll = 0;
    vizIndex = 0;
    srand((unsigned)time(NULL));
}

// ---- Bubble / selection / insertion one micro-step ----
static bool StepBubble(void)
{
    static int i, j;
    if (vizStep == 0) { i = 0; j = 0; }
    if (i >= arrN - 1) { hiA = hiB = -1; return false; }
    hiA = j; hiB = j + 1;
    if (arr[j] > arr[j + 1])
    {
        int t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
    }
    j++;
    if (j >= arrN - 1 - i) { j = 0; i++; }
    vizStep++;
    return true;
}

static bool StepSelection(void)
{
    static int i, j, minI;
    if (vizStep == 0) { i = 0; j = 1; minI = 0; }
    if (i >= arrN - 1) { hiA = hiB = -1; return false; }
    hiA = minI; hiB = j;
    if (j < arrN)
    {
        if (arr[j] < arr[minI]) minI = j;
        j++;
        vizStep++;
        return true;
    }
    int t = arr[i]; arr[i] = arr[minI]; arr[minI] = t;
    i++;
    minI = i;
    j = i + 1;
    vizStep++;
    return i < arrN - 1;
}

static bool StepInsertion(void)
{
    static int i, j, key;
    if (vizStep == 0) { i = 1; j = 0; key = arr[1]; }
    if (i >= arrN) { hiA = hiB = -1; return false; }
    if (vizStep == 0 || j < 0 || arr[j] <= key)
    {
        if (vizStep > 0 && (j < 0 || arr[j] <= key))
        {
            arr[j + 1] = key;
            i++;
            if (i >= arrN) return false;
            key = arr[i];
            j = i - 1;
        }
    }
    hiA = j; hiB = i;
    if (j >= 0 && arr[j] > key)
    {
        arr[j + 1] = arr[j];
        j--;
        vizStep++;
        return true;
    }
    arr[j + 1] = key;
    i++;
    if (i >= arrN) { hiA = hiB = -1; return false; }
    key = arr[i];
    j = i - 1;
    vizStep++;
    return true;
}

static bool StepBinary(void)
{
    if (searchFound || searchLo > searchHi) return false;
    searchMid = (searchLo + searchHi) / 2;
    hiA = searchMid; hiB = searchLo; hiPivot = searchHi;
    if (arr[searchMid] == searchTarget) { searchFound = true; return false; }
    if (arr[searchMid] < searchTarget) searchLo = searchMid + 1;
    else searchHi = searchMid - 1;
    vizStep++;
    return searchLo <= searchHi;
}

static bool StepBfs(void)
{
    if (bfsQh >= bfsQt) return false;
    curCell = bfsQ[bfsQh++];
    visitOrder[visitCount++] = curCell;
    gridCell[curCell] = 2;
    int r = curCell / GRID, c = curCell % GRID;
    const int dr[4] = { -1, 1, 0, 0 };
    const int dc[4] = { 0, 0, -1, 1 };
    for (int k = 0; k < 4; k++)
    {
        int nr = r + dr[k], nc = c + dc[k];
        if (nr < 0 || nc < 0 || nr >= GRID || nc >= GRID) continue;
        int ni = nr * GRID + nc;
        if (gridCell[ni] != 0) continue;
        gridCell[ni] = 1;
        bfsQ[bfsQt++] = ni;
    }
    vizStep++;
    return bfsQh < bfsQt;
}

static bool StepDfs(void)
{
    if (dfsTop <= 0) return false;
    curCell = dfsStack[--dfsTop];
    if (gridCell[curCell] == 2) return dfsTop > 0;
    visitOrder[visitCount++] = curCell;
    gridCell[curCell] = 2;
    int r = curCell / GRID, c = curCell % GRID;
    const int dr[4] = { 0, 0, 1, -1 };
    const int dc[4] = { 1, -1, 0, 0 };
    for (int k = 0; k < 4; k++)
    {
        int nr = r + dr[k], nc = c + dc[k];
        if (nr < 0 || nc < 0 || nr >= GRID || nc >= GRID) continue;
        int ni = nr * GRID + nc;
        if (gridCell[ni] != 0) continue;
        gridCell[ni] = 1;
        dfsStack[dfsTop++] = ni;
    }
    vizStep++;
    return dfsTop > 0;
}

static bool VizStepOnce(VizId id)
{
    switch (id)
    {
    case VIZ_BUBBLE_SORT:    return StepBubble();
    case VIZ_SELECTION_SORT: return StepSelection();
    case VIZ_INSERTION_SORT: return StepInsertion();
    case VIZ_BINARY_SEARCH:  return StepBinary();
    case VIZ_BFS:            return StepBfs();
    case VIZ_DFS:            return StepDfs();
    default: return false;
    }
}

static void DrawBars(void)
{
    float baseX = 140, baseY = 520, bw = 70, gap = 12;
    int maxV = 1;
    for (int i = 0; i < arrN; i++) if (arr[i] > maxV) maxV = arr[i];
    for (int i = 0; i < arrN; i++)
    {
        float h = (arr[i] / (float)maxV) * 280.0f;
        Rectangle r = { baseX + i * (bw + gap), baseY - h, bw, h };
        Color fill = COL_CYAN;
        if (i == hiA || i == hiB) fill = COL_ACCENT;
        if (i == hiPivot) fill = COL_DONE;
        DrawRectangleRounded(r, 0.1f, 6, fill);
        char t[8];
        snprintf(t, sizeof(t), "%d", arr[i]);
        UiDrawCenteredText(t, (int)(r.x + r.width / 2), (int)baseY + 8, 18, RAYWHITE);
    }
}

static void DrawGridViz(void)
{
    float size = 54, gap = 6;
    float ox = (SCREEN_W - GRID * (size + gap)) / 2.0f;
    float oy = 180;
    for (int i = 0; i < GRID * GRID; i++)
    {
        int r = i / GRID, c = i % GRID;
        Rectangle cell = { ox + c * (size + gap), oy + r * (size + gap), size, size };
        Color fill = COL_PANEL;
        if (gridCell[i] == -1) fill = COL_LOCKED;
        else if (gridCell[i] == 1) fill = Fade(COL_ACCENT, 0.5f);
        else if (gridCell[i] == 2) fill = COL_DONE;
        if (i == curCell) fill = COL_CYAN;
        DrawRectangleRounded(cell, 0.15f, 4, fill);
    }
}

static void DrawStack(void)
{
    float x = SCREEN_W / 2.0f - 80, y = 560;
    for (int i = 0; i < stackTop; i++)
    {
        Rectangle r = { x, y - (i + 1) * 48.0f, 160, 44 };
        DrawRectangleRounded(r, 0.15f, 6, COL_CYAN);
        char t[16];
        snprintf(t, sizeof(t), "%d", stackData[i]);
        UiDrawCenteredText(t, (int)(r.x + 80), (int)r.y + 10, 22, COL_BG);
    }
    DrawRectangleLinesEx((Rectangle){ x - 4, y - 12 * 48.0f, 168, 12 * 48.0f }, 2, COL_MUTED);
    UiDrawCenteredText("STACK (top grows up)", SCREEN_W / 2, 600, 20, COL_MUTED);
}

static void DrawQueue(void)
{
    float x = 160, y = 320;
    for (int i = 0; i < qCount; i++)
    {
        int idx = (qHead + i) % VIZ_N;
        Rectangle r = { x + i * 90.0f, y, 80, 80 };
        DrawRectangleRounded(r, 0.15f, 6, COL_ACCENT);
        char t[16];
        snprintf(t, sizeof(t), "%d", queueData[idx]);
        UiDrawCenteredText(t, (int)(r.x + 40), (int)r.y + 26, 24, COL_BG);
    }
    UiDrawText("front", (int)x, (int)y + 100, 18, COL_MUTED);
    UiDrawCenteredText("QUEUE (FIFO)", SCREEN_W / 2, 460, 20, COL_MUTED);
}

static void DrawList(void)
{
    float x = 100, y = 300;
    for (int i = 0; i < listN; i++)
    {
        Rectangle r = { x + i * 150.0f, y, 100, 70 };
        DrawRectangleRounded(r, 0.15f, 6, COL_PANEL);
        DrawRectangleRoundedLinesEx(r, 0.15f, 6, 2, COL_CYAN);
        char t[16];
        snprintf(t, sizeof(t), "%d", listVals[i]);
        UiDrawCenteredText(t, (int)(r.x + 50), (int)r.y + 20, 24, RAYWHITE);
        if (i + 1 < listN)
        {
            DrawLineEx((Vector2){ r.x + 100, r.y + 35 }, (Vector2){ r.x + 140, r.y + 35 }, 3, COL_ACCENT);
            DrawTriangle((Vector2){ r.x + 140, r.y + 35 },
                         (Vector2){ r.x + 128, r.y + 28 },
                         (Vector2){ r.x + 128, r.y + 42 }, COL_ACCENT);
        }
    }
    UiDrawCenteredText("SINGLY LINKED LIST", SCREEN_W / 2, 420, 20, COL_MUTED);
}

static void DrawBstRecursive(int idx, float x, float y, float span)
{
    if (idx < 0 || idx >= bstN) return;
    DrawCircleV((Vector2){ x, y }, 26, COL_CYAN);
    char t[16];
    snprintf(t, sizeof(t), "%d", bstKeys[idx]);
    UiDrawCenteredText(t, (int)x, (int)y - 10, 20, COL_BG);
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    if (left < bstN)
    {
        DrawLineEx((Vector2){ x, y + 26 }, (Vector2){ x - span, y + 90 }, 2, COL_MUTED);
        DrawBstRecursive(left, x - span, y + 100, span * 0.55f);
    }
    if (right < bstN)
    {
        DrawLineEx((Vector2){ x, y + 26 }, (Vector2){ x + span, y + 90 }, 2, COL_MUTED);
        DrawBstRecursive(right, x + span, y + 100, span * 0.55f);
    }
}

static void BstInsert(int key)
{
    if (bstN >= VIZ_N) return;
    if (bstN == 0) { bstKeys[bstN++] = key; return; }
    // store as heap-like complete tree array by successive insert into next slot
    // for teaching, place by BST rule into first open child slot walking from root
    int idx = 0;
    while (idx < bstN)
    {
        if (key < bstKeys[idx])
        {
            int left = 2 * idx + 1;
            if (left >= bstN) { bstKeys[bstN++] = key; return; }
            // if left slot conceptually empty in sparse sense â€” keep array packed:
            // simpler approach: append and note it's a demo tree built by inserts in order
            idx = left;
            if (idx >= bstN) { bstKeys[bstN++] = key; return; }
        }
        else
        {
            int right = 2 * idx + 2;
            if (right >= bstN) { bstKeys[bstN++] = key; return; }
            idx = right;
            if (idx >= bstN) { bstKeys[bstN++] = key; return; }
        }
        if (idx >= VIZ_N) break;
    }
    if (bstN < VIZ_N) bstKeys[bstN++] = key;
}

static void DrawCategoryDiagram(int categoryIndex, Rectangle card, Color tint)
{
    const char *diagrams[3][4]={{"CPU","RAM","CACHE",""},{"Client","API","DB",""},{"DATA","TRAIN","MODEL","TEST"}};
    int row=categoryIndex==EXPLORE_CS?0:categoryIndex==EXPLORE_SYS?1:2;
    int count=categoryIndex==EXPLORE_AI?4:3;
    float gap=10,width=(card.width-72-gap*(count-1))/count,x=card.x+36,y=card.y+150;
    for(int i=0;i<count;i++)
    {
        Rectangle box={x+i*(width+gap),y,width,32};
        DrawRectangleRounded(box,0.2f,4,Fade(tint,0.22f));
        DrawRectangleRoundedLinesEx(box,0.2f,4,1.5f,tint);
        UiDrawCenteredText(diagrams[row][i],(int)(box.x+box.width/2),(int)box.y+7,14,RAYWHITE);
        if(i+1<count){float ax=box.x+box.width+2,bx=ax+gap-4,cy=y+16;DrawLineEx((Vector2){ax,cy},(Vector2){bx,cy},2,tint);DrawTriangle((Vector2){bx,cy},(Vector2){bx-6,cy-4},(Vector2){bx-6,cy+4},tint);}
    }
}
static void DrawTopicDiagram(ExploreCategory cat,int topicIndex,Rectangle panel,Color tint)
{
    static const char *labels[3][8][4]={
        {{"Fetch","Decode","Execute","Write back"},{"Registers","Cache","RAM","Disk"},{"Process A","Scheduler","Process B","Shared OS"},{"Ready","Running","Blocked","Ready"},{"DNS","IP","TCP","Port"},{"Path","Metadata","Blocks","Disk"},{"Source","Compiler","Object","Linker"},{"Bits","Hex","Signed","Integer"}},
        {{"Client","Request","Server","Response"},{"Users","Balancer","API","Nodes"},{"Request","Cache","Service","Store"},{"App","Query","Index","Database"},{"Key","Shard A","Shard B","Replicas"},{"Producer","Queue","Worker","Result"},{"Partition","Consistency","Availability","Policy"},{"Request","Logs","Metrics","Trace"}},
        {{"Examples","Features","Model","Prediction"},{"Train","Validate","Tune","Test"},{"Simple","Fit","Generalise","Complex"},{"Input","Layers","Weights","Output"},{"Prediction","Loss","Gradient","Update"},{"Data","Features","Algorithm","Result"},{"Data","Review","Model","Monitor"},{"C/C++","Runtime","Inference","Device"}}
    };
    if(cat<EXPLORE_CS||cat>EXPLORE_AI)return;
    int row=(int)cat-EXPLORE_CS;if(topicIndex<0||topicIndex>=8)return;
    int count=4;float gap=14,width=(panel.width-72-gap*(count-1))/count;
    float x=panel.x+36,y=panel.y+310;
    for(int i=0;i<count;i++)
    {
        Rectangle box={x+i*(width+gap),y,width,38};
        DrawRectangleRounded(box,0.16f,5,Fade(tint,0.2f));
        DrawRectangleRoundedLinesEx(box,0.16f,5,1.5f,tint);
        UiDrawCenteredText(labels[row][topicIndex][i],(int)(box.x+box.width/2),(int)box.y+10,15,RAYWHITE);
        if(i+1<count){float ax=box.x+box.width+2,bx=ax+gap-4,cy=y+19;DrawLineEx((Vector2){ax,cy},(Vector2){bx,cy},2,tint);DrawTriangle((Vector2){bx,cy},(Vector2){bx-6,cy-4},(Vector2){bx-6,cy+4},tint);}
    }
}

static void RunHub(AppState *app)
{
    UiDrawCenteredText("Explore", SCREEN_W / 2, 40, 44, RAYWHITE);
    UiDrawCenteredText("Inspired by algorithm visualisers â€” built for C/C++ learners", SCREEN_W / 2, 92, 20, COL_MUTED);

    const Color colors[4] = { COL_ACCENT, COL_CYAN, COL_DONE, (Color){ 255, 140, 66, 255 } };
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < EXPLORE_CAT_COUNT; i++)
    {
        int col = i % 2, row = i / 2;
        Rectangle r = { 100.0f + col * 540.0f, 140.0f + row * 240.0f, 500, 210 };
        bool h = CheckCollisionPointRec(mouse, r);
        DrawRectangleRounded(r, 0.07f, 8, COL_PANEL);
        DrawRectangleRoundedLinesEx(r, 0.07f, 8, 3.0f, h ? colors[i] : Fade(COL_MUTED, 0.35f));
        DrawCircle((int)(r.x + 50), (int)(r.y + 50), 28, colors[i]);
        char num[2] = { (char)('1' + i), 0 };
        UiDrawText(num, (int)(r.x + 42), (int)(r.y + 38), 26, COL_BG);
        UiDrawText(ExploreCategoryName((ExploreCategory)i), (int)r.x + 100, (int)r.y + 36, 26, RAYWHITE);
        UiDrawWrappedText(ExploreCategoryBlurb((ExploreCategory)i),
                          (Rectangle){ r.x + 36, r.y + 90, r.width - 72, 70 }, 20, COL_MUTED);
        if (i >= EXPLORE_CS) DrawCategoryDiagram(i, r, colors[i]);
        char count[40];
        snprintf(count, sizeof(count), "%d topics%s", ExploreTopicCount((ExploreCategory)i),
                 i == EXPLORE_DSA ? " + visualisers" : "");
        UiDrawText(count, (int)r.x + 36, (int)r.y + (i >= EXPLORE_CS ? 185 : 170), 18, colors[i]);
        if (h && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            category = (ExploreCategory)i;
            topicIndex = 0;
            topicScroll = 0;
            mode = EX_TOPICS;
        }
    }

    if (BackBtn()) AppGoTo(app, SCREEN_MENU);
}

static void RunTopics(AppState *app)
{
    (void)app;
    UiDrawCenteredText(ExploreCategoryName(category), SCREEN_W / 2, 36, 34, RAYWHITE);

    if (category == EXPLORE_DSA)
    {
        if (UiButtonPrimary((Rectangle){ SCREEN_W - 280.0f, 30, 240, 44 }, "Open Visualisers", true, 1.0f))
            mode = EX_VIZ_PICK;
    }

    topicScroll -= GetMouseWheelMove() * 40.0f;
    if (topicScroll < 0) topicScroll = 0;

    int n = ExploreTopicCount(category);
    Vector2 mouse = GetMousePosition();
    float y = 100 - topicScroll;
    for (int i = 0; i < n; i++)
    {
        const ExploreTopic *t = ExploreTopicGet(category, i);
        Rectangle r = { 120, y, SCREEN_W - 240.0f, 86 };
        if (r.y + r.height > 90 && r.y < SCREEN_H - 40)
        {
            bool h = CheckCollisionPointRec(mouse, r);
            DrawRectangleRounded(r, 0.12f, 8, COL_PANEL);
            DrawRectangleRoundedLinesEx(r, 0.12f, 8, 2.0f, h ? COL_ACCENT : Fade(COL_MUTED, 0.3f));
            UiDrawText(t->title, (int)r.x + 24, (int)r.y + 16, 24, RAYWHITE);
            UiDrawText(t->blurb, (int)r.x + 24, (int)r.y + 50, 18, COL_MUTED);
            if (h && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                topicIndex = i;
                mode = EX_TOPIC_DETAIL;
            }
        }
        y += 98;
    }
    float maxScroll = n * 98.0f - 500.0f;
    if (maxScroll < 0) maxScroll = 0;
    if (topicScroll > maxScroll) topicScroll = maxScroll;

    if (BackBtn()) mode = EX_HUB;
}

static void RunTopicDetail(AppState *app)
{
    (void)app;
    const ExploreTopic *t = ExploreTopicGet(category, topicIndex);
    if (!t) { mode = EX_TOPICS; return; }

    UiDrawCenteredText(t->title, SCREEN_W / 2, 40, 36, RAYWHITE);
    UiDrawCenteredText(t->blurb, SCREEN_W / 2, 90, 20, COL_MUTED);

    Rectangle panel = { 100, 130, SCREEN_W - 200.0f, 460 };
    DrawRectangleRounded(panel, 0.06f, 8, COL_PANEL);
    bool hasDiagram=category>=EXPLORE_CS;
    UiDrawWrappedText(t->body, (Rectangle){ panel.x + 36, panel.y + 30, panel.width - 72, hasDiagram?245:360 }, 22, RAYWHITE);
    if(hasDiagram) DrawTopicDiagram(category,topicIndex,panel,category==EXPLORE_CS?COL_CYAN:category==EXPLORE_SYS?COL_DONE:(Color){255,140,66,255});
    if (t->complexity)
    {
        UiDrawText(t->complexity, (int)panel.x + 36, (int)panel.y + 410, 20, COL_ACCENT);
    }

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 260, 620, 240, 48 }, "Prev", topicIndex > 0, 1.0f))
        topicIndex--;
    if (UiButton((Rectangle){ SCREEN_W / 2.0f + 20, 620, 240, 48 }, "Next",
                 topicIndex + 1 < ExploreTopicCount(category), 1.0f))
        topicIndex++;

    if (BackBtn()) mode = EX_TOPICS;
}

static void RunVizPick(AppState *app)
{
    (void)app;
    UiDrawCenteredText("Algorithm Visualisers", SCREEN_W / 2, 36, 36, RAYWHITE);
    UiDrawCenteredText("Step through classic DSA animations", SCREEN_W / 2, 84, 20, COL_MUTED);

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < VizCount(); i++)
    {
        const VizInfo *v = VizGet(i);
        int col = i % 5, row = i / 5;
        Rectangle r = { 60.0f + col * 240.0f, 130.0f + row * 240.0f, 220, 210 };
        bool h = CheckCollisionPointRec(mouse, r);
        DrawRectangleRounded(r, 0.08f, 8, COL_PANEL);
        DrawRectangleRoundedLinesEx(r, 0.08f, 8, 2.5f, h ? COL_CYAN : Fade(COL_MUTED, 0.35f));
        UiDrawCenteredText(v->title, (int)(r.x + r.width / 2), (int)r.y + 50, 22, RAYWHITE);
        UiDrawWrappedText(v->blurb, (Rectangle){ r.x + 16, r.y + 100, r.width - 32, 80 }, 16, COL_MUTED);
        if (h && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            vizIndex = i;
            VizReset(v->id);
            mode = EX_VIZ_RUN;
        }
    }
    if (BackBtn()) mode = EX_TOPICS;
}

static void RunViz(AppState *app, float dt)
{
    (void)app;
    const VizInfo *v = VizGet(vizIndex);
    if (!v) { mode = EX_VIZ_PICK; return; }

    UiDrawCenteredText(v->title, SCREEN_W / 2, 30, 32, RAYWHITE);
    UiDrawCenteredText(v->blurb, SCREEN_W / 2, 72, 18, COL_MUTED);

    if (vizPlaying)
    {
        vizTimer += dt;
        if (vizTimer >= vizSpeed)
        {
            vizTimer = 0;
            if (!VizStepOnce(v->id)) vizPlaying = false;
        }
    }

    VizId id = v->id;
    if (id == VIZ_BUBBLE_SORT || id == VIZ_SELECTION_SORT || id == VIZ_INSERTION_SORT || id == VIZ_BINARY_SEARCH)
        DrawBars();
    else if (id == VIZ_BFS || id == VIZ_DFS)
        DrawGridViz();
    else if (id == VIZ_STACK)
        DrawStack();
    else if (id == VIZ_QUEUE)
        DrawQueue();
    else if (id == VIZ_LINKED_LIST)
        DrawList();
    else if (id == VIZ_BST)
        DrawBstRecursive(0, SCREEN_W / 2.0f, 160, 220);

    if (id == VIZ_BINARY_SEARCH)
    {
        char msg[80];
        snprintf(msg, sizeof(msg), "Target: %d   lo=%d hi=%d mid=%s",
                 searchTarget, searchLo, searchHi,
                 searchMid >= 0 ? TextFormat("%d", searchMid) : "-");
        UiDrawCenteredText(msg, SCREEN_W / 2, 120, 20, COL_CYAN);
        if (searchFound) UiDrawCenteredText("Found!", SCREEN_W / 2, 145, 22, COL_DONE);
    }

    // Interactive buttons for stack/queue/list/bst
    if (id == VIZ_STACK)
    {
        if (UiButton((Rectangle){ 200, 640, 160, 44 }, "Push", stackTop < VIZ_N, 1.0f))
            stackData[stackTop++] = rand() % 90 + 1;
        if (UiButton((Rectangle){ 380, 640, 160, 44 }, "Pop", stackTop > 0, 1.0f))
            stackTop--;
    }
    else if (id == VIZ_QUEUE)
    {
        if (UiButton((Rectangle){ 200, 640, 160, 44 }, "Enqueue", qCount < VIZ_N, 1.0f))
        {
            queueData[qTail] = rand() % 90 + 1;
            qTail = (qTail + 1) % VIZ_N;
            qCount++;
        }
        if (UiButton((Rectangle){ 380, 640, 160, 44 }, "Dequeue", qCount > 0, 1.0f))
        {
            qHead = (qHead + 1) % VIZ_N;
            qCount--;
        }
    }
    else if (id == VIZ_LINKED_LIST)
    {
        if (UiButton((Rectangle){ 200, 640, 160, 44 }, "Append", listN < VIZ_N, 1.0f))
            listVals[listN++] = rand() % 90 + 1;
        if (UiButton((Rectangle){ 380, 640, 160, 44 }, "Pop front", listN > 0, 1.0f))
        {
            for (int i = 1; i < listN; i++) listVals[i - 1] = listVals[i];
            listN--;
        }
    }
    else if (id == VIZ_BST)
    {
        if (UiButton((Rectangle){ 200, 640, 200, 44 }, "Insert random", bstN < VIZ_N, 1.0f))
            BstInsert(rand() % 90 + 1);
        if (UiButton((Rectangle){ 420, 640, 160, 44 }, "Clear", true, 1.0f))
            bstN = 0;
    }
    else
    {
        if (UiButton((Rectangle){ 200, 640, 140, 44 }, vizPlaying ? "Pause" : "Play", true, 1.0f))
            vizPlaying = !vizPlaying;
        if (UiButton((Rectangle){ 360, 640, 140, 44 }, "Step", !vizPlaying, 1.0f))
            VizStepOnce(id);
        if (UiButton((Rectangle){ 520, 640, 140, 44 }, "Reset", true, 1.0f))
            VizReset(id);
        if (UiButton((Rectangle){ 680, 640, 140, 44 }, "Faster", true, 1.0f))
            vizSpeed = fmaxf(0.12f, vizSpeed - 0.08f);
        if (UiButton((Rectangle){ 840, 640, 140, 44 }, "Slower", true, 1.0f))
            vizSpeed = fminf(1.2f, vizSpeed + 0.08f);
    }

    if (BackBtn()) { vizPlaying = false; mode = EX_VIZ_PICK; }
}

void ScreenExploreRun(AppState *app, float dt)
{
    switch (mode)
    {
    case EX_HUB:          RunHub(app); break;
    case EX_TOPICS:       RunTopics(app); break;
    case EX_TOPIC_DETAIL: RunTopicDetail(app); break;
    case EX_VIZ_PICK:     RunVizPick(app); break;
    case EX_VIZ_RUN:      RunViz(app, dt); break;
    }
}
