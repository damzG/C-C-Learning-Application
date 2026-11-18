#include "explore_diagrams.h"
#include "theme.h"
#include "ui.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *tag;
    const char *label;
    float nx, ny; // Normalized 0.0..1.0 coordinates inside canvas
    Color accent;
    const char *detail;
} ArchNode;

typedef struct {
    int from, to;
    const char *protocol;
    int activeStep; // Step at which packet animates along this edge
} ArchEdge;

typedef struct {
    const char *title;
    const char *subtitle;
    int nodeCount;
    ArchNode nodes[8];
    int edgeCount;
    ArchEdge edges[10];
    int totalSteps;
    const char *stepNotes[6];
} ArchDiagram;

static const ArchDiagram DIAGRAMS[DIAG_CAT_COUNT] = {
    // 1. Computer Science: CPU Instruction Pipeline & Cache Hierarchy
    {
        "CPU Pipeline & L1/L2 Cache",
        "Fetch, Decode, Execute, Memory Access & Writeback",
        5,
        {
            { "FETCH",   "Instruction Fetch", 0.12f, 0.50f, { 59, 130, 246, 255 }, "Reads PC address from L1 Instruction Cache." },
            { "CACHE",   "L1 / L2 Cache",     0.34f, 0.22f, { 20, 184, 166, 255 }, "SRAM cache hit in ~1-4 cycles; avoids 100ns DRAM stall." },
            { "DECODE",  "Control Unit",      0.34f, 0.72f, { 168, 85, 247, 255 }, "Decodes opcode into micro-ops and register operands." },
            { "ALU",     "Execution ALU",     0.62f, 0.50f, { 245, 158, 11, 255 }, "Performs integer/bitwise arithmetic and branch checks." },
            { "REG",     "Register File",     0.86f, 0.50f, { 34, 197, 94, 255 },  "Commits computed result back to architectural registers." }
        },
        5,
        {
            { 0, 1, "PC Addr",   0 },
            { 1, 2, "32-bit Op", 1 },
            { 0, 2, "Pipeline",  1 },
            { 2, 3, "Micro-ops", 2 },
            { 3, 4, "Writeback", 3 }
        },
        4,
        {
            "Step 0/3: Program Counter requests next instruction from L1 I-Cache.",
            "Step 1/3: Cache hit returns machine word to Decoder unit.",
            "Step 2/3: Decoder dispatches operands into Arithmetic Logic Unit.",
            "Step 3/3: ALU writes result to Register File in a single clock edge."
        }
    },
    // 2. Artificial Intelligence: Transformer Self-Attention & Inference
    {
        "Transformer Inference Flow",
        "Tokenization, Embeddings, Multi-Head Attention & Softmax",
        5,
        {
            { "INPUT",   "Tokenizer",         0.12f, 0.50f, { 59, 130, 246, 255 }, "Splits prompt text into subword token IDs (BPE)." },
            { "EMBED",   "Vector Embed + PE", 0.33f, 0.50f, { 20, 184, 166, 255 }, "Maps tokens to d_model vectors + positional encoding." },
            { "ATTN",    "Q·K^T Self-Attn",   0.56f, 0.26f, { 168, 85, 247, 255 }, "Computes scaled dot-product attention across context." },
            { "FFN",     "Feed-Forward MLP",  0.56f, 0.74f, { 245, 158, 11, 255 }, "Applies GELU non-linearity and residual LayerNorm." },
            { "OUTPUT",  "Logits & Softmax",  0.84f, 0.50f, { 34, 197, 94, 255 },  "Projects hidden state to vocabulary probability distribution." }
        },
        5,
        {
            { 0, 1, "Token IDs", 0 },
            { 1, 2, "Q, K, V",   1 },
            { 2, 3, "Context",   2 },
            { 1, 3, "Residual",  2 },
            { 3, 4, "Next Tok",  3 }
        },
        4,
        {
            "Step 0/3: Raw input prompt is encoded into integer token IDs.",
            "Step 1/3: Embedding matrix produces Query, Key, and Value tensors.",
            "Step 2/3: Multi-head attention weights context tokens & passes FFN.",
            "Step 3/3: Softmax samples the highest-probability next token."
        }
    },
    // 3. System Design: Video Streaming Architecture (Matching Reference Screenshot)
    {
        "Video Streaming Pipeline",
        "Upload, transcode, Edge CDN delivery & playback",
        6,
        {
            { "CLIENT",  "Player",            0.12f, 0.35f, { 59, 130, 246, 255 }, "Adaptive bitrate HLS/DASH player requesting 5 Mbps chunks." },
            { "QUEUE",   "Job Queue",         0.12f, 0.72f, { 225, 29, 72, 255 },  "Kafka/RabbitMQ queue decoupling raw video uploads." },
            { "CDN",     "Edge CDN Nodes",    0.42f, 0.35f, { 13, 148, 136, 255 }, "30 geographically distributed PoPs serving cached segments." },
            { "WORKER",  "Transcode Worker",  0.42f, 0.72f, { 147, 51, 234, 255 }, "FFmpeg GPU cluster encoding 1080p/4K ABR renditions." },
            { "LB",      "Origin LB",         0.72f, 0.35f, { 79, 70, 229, 255 },  "Routes Edge cache misses to Origin Object Storage." },
            { "STORAGE", "Object Store & DB", 0.88f, 0.62f, { 217, 119, 6, 255 },  "S3 blob storage for chunks + Catalog DB for manifests." }
        },
        6,
        {
            { 0, 2, "GET .m3u8",   0 },
            { 2, 4, "Cache Miss",  1 },
            { 4, 5, "Fetch Chunk", 2 },
            { 1, 3, "Raw Video",   2 },
            { 3, 5, "Store ABR",   3 },
            { 2, 0, "5 Mbps Stream", 3 }
        },
        4,
        {
            "Step 0/3: Player requests video manifest at 5 Mbps from nearest Edge CDN.",
            "Step 1/3: Edge Cache Miss forwards request through Origin Load Balancer.",
            "Step 2/3: Origin fetches segments from Object Store while Workers transcode.",
            "Step 3/3: Edge CDN caches chunks and streams smoothly to 10,000 viewers."
        }
    }
};

static int currentStep = 0;
static bool isPlaying = false;
static float playTimer = 0.0f;
static float playSpeed = 1.0f;
static int selectedNode = -1;

void ExploreDiagramsInit(void)
{
    currentStep = 0;
    isPlaying = false;
    playTimer = 0.0f;
    playSpeed = 1.0f;
    selectedNode = -1;
}

void ExploreDiagramsDraw(Rectangle bounds, DiagramCategory category, float dt)
{
    if (category < 0 || category >= DIAG_CAT_COUNT) category = DIAG_CAT_CS;
    const ArchDiagram *d = &DIAGRAMS[category];

    if (isPlaying)
    {
        playTimer += dt * playSpeed;
        if (playTimer >= 1.4f)
        {
            playTimer = 0.0f;
            currentStep = (currentStep + 1) % d->totalSteps;
        }
    }

    // Main Card Container
    DrawRectangleRounded(bounds, 0.03f, 8, COL_PANEL);
    DrawRectangleRoundedLinesEx(bounds, 0.03f, 8, 1.5f, Fade(COL_MUTED, 0.4f));

    // Header
    DrawText(d->title, (int)bounds.x + 20, (int)bounds.y + 14, 22, RAYWHITE);
    DrawText(d->subtitle, (int)bounds.x + 20, (int)bounds.y + 40, 14, COL_MUTED);

    // Interactive Canvas Area
    Rectangle canvas = { bounds.x + 16, bounds.y + 66, bounds.width - 32, bounds.height - 220 };
    DrawRectangleRounded(canvas, 0.03f, 8, (Color){ 15, 23, 42, 255 });
    DrawRectangleRoundedLinesEx(canvas, 0.03f, 8, 1.0f, Fade(COL_MUTED, 0.3f));

    // Compute Node Centers
    Vector2 centers[8];
    for (int i = 0; i < d->nodeCount; i++)
    {
        centers[i].x = canvas.x + d->nodes[i].nx * canvas.width;
        centers[i].y = canvas.y + d->nodes[i].ny * canvas.height;
    }

    // Draw Directed Edges + Animated Packet Flow
    for (int i = 0; i < d->edgeCount; i++)
    {
        const ArchEdge *e = &d->edges[i];
        Vector2 a = centers[e->from];
        Vector2 b = centers[e->to];
        bool active = (e->activeStep == currentStep);

        Color lineCol = active ? COL_CYAN : Fade(COL_MUTED, 0.45f);
        DrawLineEx(a, b, active ? 3.0f : 1.8f, lineCol);

        // Protocol pill midpoint
        Vector2 mid = { (a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f };
        DrawText(e->protocol, (int)mid.x - 28, (int)mid.y - 16, 12, active ? RAYWHITE : COL_MUTED);

        // Moving packet dot when active
        if (active)
        {
            float t = fmodf((float)GetTime() * playSpeed, 1.0f);
            Vector2 pkt = { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
            DrawCircleV(pkt, 6.0f, COL_ACCENT);
        }
    }

    // Draw Architecture Nodes (Styled like reference screenshot cards)
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < d->nodeCount; i++)
    {
        const ArchNode *n = &d->nodes[i];
        Rectangle nr = { centers[i].x - 68, centers[i].y - 26, 136, 52 };
        bool hover = CheckCollisionPointRec(mouse, nr);
        bool sel = (selectedNode == i);

        DrawRectangleRounded(nr, 0.18f, 6, (Color){ 30, 41, 59, 255 });
        DrawRectangleRoundedLinesEx(nr, 0.18f, 6, sel || hover ? 2.5f : 1.5f, sel ? COL_ACCENT : n->accent);

        // Colored category badge strip on top
        DrawRectangle((int)nr.x + 8, (int)nr.y + 6, 48, 12, n->accent);
        DrawText(n->tag, (int)nr.x + 11, (int)nr.y + 7, 10, RAYWHITE);
        DrawText(n->label, (int)nr.x + 8, (int)nr.y + 24, 13, RAYWHITE);

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            selectedNode = (selectedNode == i) ? -1 : i;
    }

    // Bottom Step Scrubber & Playback Controls (Matching Reference UI)
    float ctrlY = canvas.y + canvas.height + 12;
    char stepBadge[32];
    snprintf(stepBadge, sizeof(stepBadge), "step %d / %d", currentStep, d->totalSteps - 1);
    DrawRectangleRounded((Rectangle){ bounds.x + 20, ctrlY, 110, 28 }, 0.4f, 6, (Color){ 30, 58, 138, 255 });
    DrawText(stepBadge, (int)bounds.x + 34, (int)ctrlY + 6, 15, COL_CYAN);

    // Step Explanation or Node Inspector
    const char *desc = (selectedNode >= 0 && selectedNode < d->nodeCount)
                       ? d->nodes[selectedNode].detail
                       : d->stepNotes[currentStep];
    UiDrawWrappedText(desc, (Rectangle){ bounds.x + 20, ctrlY + 34, bounds.width - 40, 44 }, 16, RAYWHITE);

    // Playback Buttons (Play/Pause, Prev, Next, Reset)
    float btnY = bounds.y + bounds.height - 58;
    if (UiButton((Rectangle){ bounds.x + 20, btnY, 115, 44 }, isPlaying ? "Pause" : "Play", true, 1.0f))
        isPlaying = !isPlaying;

    if (UiButton((Rectangle){ bounds.x + 145, btnY, 54, 44 }, "|<", true, 1.0f))
    {
        isPlaying = false;
        currentStep = (currentStep - 1 + d->totalSteps) % d->totalSteps;
    }
    if (UiButton((Rectangle){ bounds.x + 207, btnY, 54, 44 }, ">|", true, 1.0f))
    {
        isPlaying = false;
        currentStep = (currentStep + 1) % d->totalSteps;
    }
    if (UiButton((Rectangle){ bounds.x + 269, btnY, 75, 44 }, "Reset", true, 1.0f))
    {
        isPlaying = false;
        currentStep = 0;
        selectedNode = -1;
    }

    // Speed Toggle Pill
    char spdLbl[24];
    snprintf(spdLbl, sizeof(spdLbl), "Speed: %.1fx", playSpeed);
    if (UiButton((Rectangle){ bounds.x + bounds.width - 150, btnY, 130, 44 }, spdLbl, true, 1.0f))
        playSpeed = (playSpeed >= 2.0f) ? 0.5f : playSpeed + 0.5f;
}