#include "chess_game.h"
#include "theme.h"
#include "ui.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Piece encoding: >0 White, <0 Black
// 1: Pawn, 2: Knight, 3: Bishop, 4: Rook, 5: Queen, 6: King
enum { P_EMPTY = 0, P_PAWN = 1, P_KNIGHT = 2, P_BISHOP = 3, P_ROOK = 4, P_QUEEN = 5, P_KING = 6 };

typedef struct { int r1, c1, r2, c2; } Move;

static int board[8][8];
static bool whiteTurn;
static bool is3DMode;
static int selR, selC;
static bool validMoves[8][8];
static bool gameOver;
static char statusMsg[96];
static float aiDelayTimer;
static Camera3D cam3d;

static const int PIECE_VALS[7] = { 0, 100, 320, 330, 500, 900, 20000 };

static void Draw2DPiece(int piece, Rectangle square)
{
    float cx = square.x + square.width * 0.5f;
    float baseY = square.y + square.height - 8.0f;
    bool white = piece > 0;
    Color fill = white ? (Color){ 246, 248, 252, 255 } : (Color){ 30, 41, 59, 255 };
    Color edge = white ? (Color){ 25, 39, 62, 255 } : (Color){ 226, 234, 246, 255 };
    Color detail = white ? (Color){ 35, 151, 205, 255 } : (Color){ 76, 190, 226, 255 };

    DrawEllipse((int)cx, (int)baseY, 19.0f, 5.0f, edge);
    DrawEllipse((int)cx, (int)baseY - 1, 16.0f, 3.0f, fill);

    switch (abs(piece))
    {
        case P_PAWN:
            DrawRectangleRounded((Rectangle){ cx - 8, baseY - 30, 16, 22 }, 0.3f, 4, fill);
            DrawRectangleRoundedLinesEx((Rectangle){ cx - 8, baseY - 30, 16, 22 }, 0.3f, 4, 1.5f, edge);
            DrawCircle((int)cx, (int)baseY - 39, 8.0f, fill);
            DrawCircleLines((int)cx, (int)baseY - 39, 8.0f, edge);
            break;
        case P_ROOK:
            DrawRectangleRec((Rectangle){ cx - 13, baseY - 31, 26, 24 }, fill);
            DrawRectangleLinesEx((Rectangle){ cx - 13, baseY - 31, 26, 24 }, 1.5f, edge);
            DrawRectangleRec((Rectangle){ cx - 16, baseY - 36, 32, 7 }, fill);
            DrawRectangleLinesEx((Rectangle){ cx - 16, baseY - 36, 32, 7 }, 1.5f, edge);
            DrawRectangleRec((Rectangle){ cx - 15, baseY - 44, 8, 9 }, fill);
            DrawRectangleRec((Rectangle){ cx - 4, baseY - 44, 8, 9 }, fill);
            DrawRectangleRec((Rectangle){ cx + 7, baseY - 44, 8, 9 }, fill);
            DrawLineEx((Vector2){ cx - 15, baseY - 44 }, (Vector2){ cx - 7, baseY - 44 }, 2, edge);
            DrawLineEx((Vector2){ cx - 4, baseY - 44 }, (Vector2){ cx + 4, baseY - 44 }, 2, edge);
            DrawLineEx((Vector2){ cx + 7, baseY - 44 }, (Vector2){ cx + 15, baseY - 44 }, 2, edge);
            break;
        case P_KNIGHT:
            DrawTriangle((Vector2){ cx - 15, baseY - 8 }, (Vector2){ cx + 15, baseY - 8 }, (Vector2){ cx + 4, baseY - 29 }, fill);
            DrawTriangle((Vector2){ cx - 3, baseY - 25 }, (Vector2){ cx + 7, baseY - 46 }, (Vector2){ cx + 13, baseY - 24 }, fill);
            DrawTriangle((Vector2){ cx + 7, baseY - 43 }, (Vector2){ cx + 17, baseY - 39 }, (Vector2){ cx + 10, baseY - 34 }, fill);
            DrawLineEx((Vector2){ cx - 15, baseY - 8 }, (Vector2){ cx + 4, baseY - 29 }, 2, edge);
            DrawLineEx((Vector2){ cx + 4, baseY - 29 }, (Vector2){ cx + 7, baseY - 46 }, 2, edge);
            DrawLineEx((Vector2){ cx + 7, baseY - 46 }, (Vector2){ cx + 17, baseY - 39 }, 2, edge);
            DrawLineEx((Vector2){ cx + 17, baseY - 39 }, (Vector2){ cx + 10, baseY - 34 }, 2, edge);
            DrawCircle((int)(cx + 8), (int)(baseY - 36), 1.8f, detail);
            break;
        case P_BISHOP:
            DrawTriangle((Vector2){ cx - 13, baseY - 8 }, (Vector2){ cx + 13, baseY - 8 }, (Vector2){ cx, baseY - 40 }, fill);
            DrawLineEx((Vector2){ cx - 13, baseY - 8 }, (Vector2){ cx, baseY - 40 }, 1.8f, edge);
            DrawLineEx((Vector2){ cx + 13, baseY - 8 }, (Vector2){ cx, baseY - 40 }, 1.8f, edge);
            DrawLineEx((Vector2){ cx - 7, baseY - 20 }, (Vector2){ cx + 7, baseY - 20 }, 1.4f, edge);
            DrawCircle((int)cx, (int)baseY - 42, 5.0f, fill);
            DrawCircleLines((int)cx, (int)baseY - 42, 5.0f, edge);
            DrawLineEx((Vector2){ cx - 2, baseY - 46 }, (Vector2){ cx + 2, baseY - 39 }, 1.5f, detail);
            break;
        case P_QUEEN:
            DrawTriangle((Vector2){ cx - 15, baseY - 8 }, (Vector2){ cx + 15, baseY - 8 }, (Vector2){ cx, baseY - 34 }, fill);
            DrawLineEx((Vector2){ cx - 15, baseY - 8 }, (Vector2){ cx, baseY - 34 }, 1.8f, edge);
            DrawLineEx((Vector2){ cx + 15, baseY - 8 }, (Vector2){ cx, baseY - 34 }, 1.8f, edge);
            DrawLineEx((Vector2){ cx - 13, baseY - 19 }, (Vector2){ cx + 13, baseY - 19 }, 1.5f, edge);
            DrawCircle((int)(cx - 11), (int)baseY - 39, 4.0f, detail);
            DrawCircle((int)cx, (int)baseY - 42, 4.0f, detail);
            DrawCircle((int)(cx + 11), (int)baseY - 39, 4.0f, detail);
            break;
        case P_KING:
            DrawTriangle((Vector2){ cx - 14, baseY - 8 }, (Vector2){ cx + 14, baseY - 8 }, (Vector2){ cx, baseY - 34 }, fill);
            DrawLineEx((Vector2){ cx - 14, baseY - 8 }, (Vector2){ cx, baseY - 34 }, 1.8f, edge);
            DrawLineEx((Vector2){ cx + 14, baseY - 8 }, (Vector2){ cx, baseY - 34 }, 1.8f, edge);
            DrawLineEx((Vector2){ cx - 13, baseY - 19 }, (Vector2){ cx + 13, baseY - 19 }, 1.5f, edge);
            DrawRectangleRounded((Rectangle){ cx - 2.5f, baseY - 49, 5, 20 }, 0.25f, 3, detail);
            DrawRectangleRounded((Rectangle){ cx - 8, baseY - 44, 16, 5 }, 0.25f, 3, detail);
            break;
    }
}

static bool InBounds(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }
static int Sign(int v) { return (v > 0) - (v < 0); }

static bool IsPseudoLegal(const int b[8][8], int r1, int c1, int r2, int c2)
{
    if (!InBounds(r1, c1) || !InBounds(r2, c2) || (r1 == r2 && c1 == c2)) return false;
    int p = b[r1][c1];
    int target = b[r2][c2];
    if (p == 0 || Sign(p) == Sign(target)) return false;

    int side = Sign(p);
    int type = abs(p);
    int dr = r2 - r1;
    int dc = c2 - c1;
    int adr = abs(dr), adc = abs(dc);

    switch (type)
    {
        case P_PAWN:
        {
            int dir = -side; // White moves up (-1), Black moves down (+1)
            int startRow = (side == 1) ? 6 : 1;
            if (dc == 0 && target == 0)
            {
                if (dr == dir) return true;
                if (r1 == startRow && dr == 2 * dir && b[r1 + dir][c1] == 0) return true;
            }
            if (adc == 1 && dr == dir && target != 0) return true;
            return false;
        }
        case P_KNIGHT:
            return (adr == 2 && adc == 1) || (adr == 1 && adc == 2);
        case P_KING:
            return adr <= 1 && adc <= 1;
        case P_ROOK:
        case P_BISHOP:
        case P_QUEEN:
        {
            if (type == P_ROOK && dr != 0 && dc != 0) return false;
            if (type == P_BISHOP && adr != adc) return false;
            if (type == P_QUEEN && dr != 0 && dc != 0 && adr != adc) return false;
            int stepR = Sign(dr), stepC = Sign(dc);
            int cr = r1 + stepR, cc = c1 + stepC;
            while (cr != r2 || cc != c2)
            {
                if (b[cr][cc] != 0) return false;
                cr += stepR;
                cc += stepC;
            }
            return true;
        }
    }
    return false;
}

static int GenerateMoves(const int b[8][8], int side, Move out[256])
{
    int count = 0;
    for (int r1 = 0; r1 < 8; r1++)
        for (int c1 = 0; c1 < 8; c1++)
            if (Sign(b[r1][c1]) == side)
                for (int r2 = 0; r2 < 8; r2++)
                    for (int c2 = 0; c2 < 8; c2++)
                        if (IsPseudoLegal(b, r1, c1, r2, c2) && count < 256)
                            out[count++] = (Move){ r1, c1, r2, c2 };
    return count;
}

static int EvaluateBoard(const int b[8][8])
{
    int score = 0;
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            int p = b[r][c];
            if (p == 0) continue;
            int val = PIECE_VALS[abs(p)];
            // Center control positional bonus
            int centerBonus = (3 - abs(3 - r)) + (3 - abs(3 - c));
            score += Sign(p) * (val + centerBonus * 4);
        }
    }
    return score;
}

static int Minimax(int b[8][8], int depth, int alpha, int beta, bool maximizingWhite)
{
    if (depth == 0) return EvaluateBoard(b);
    Move moves[256];
    int n = GenerateMoves(b, maximizingWhite ? 1 : -1, moves);
    if (n == 0) return EvaluateBoard(b);

    if (maximizingWhite)
    {
        int best = -1000000;
        for (int i = 0; i < n; i++)
        {
            int captured = b[moves[i].r2][moves[i].c2];
            int moving = b[moves[i].r1][moves[i].c1];
            b[moves[i].r2][moves[i].c2] = moving;
            b[moves[i].r1][moves[i].c1] = 0;

            int eval = Minimax(b, depth - 1, alpha, beta, false);

            b[moves[i].r1][moves[i].c1] = moving;
            b[moves[i].r2][moves[i].c2] = captured;

            if (eval > best) best = eval;
            if (best > alpha) alpha = best;
            if (beta <= alpha) break;
        }
        return best;
    }
    else
    {
        int best = 1000000;
        for (int i = 0; i < n; i++)
        {
            int captured = b[moves[i].r2][moves[i].c2];
            int moving = b[moves[i].r1][moves[i].c1];
            b[moves[i].r2][moves[i].c2] = moving;
            b[moves[i].r1][moves[i].c1] = 0;

            int eval = Minimax(b, depth - 1, alpha, beta, true);

            b[moves[i].r1][moves[i].c1] = moving;
            b[moves[i].r2][moves[i].c2] = captured;

            if (eval < best) best = eval;
            if (best < beta) beta = best;
            if (beta <= alpha) break;
        }
        return best;
    }
}

static void ExecuteMove(Move m)
{
    int captured = board[m.r2][m.c2];
    int moving = board[m.r1][m.c1];
    board[m.r2][m.c2] = moving;
    board[m.r1][m.c1] = 0;

    // Auto-promote pawn to Queen
    if (moving == P_PAWN && m.r2 == 0) board[m.r2][m.c2] = P_QUEEN;
    if (moving == -P_PAWN && m.r2 == 7) board[m.r2][m.c2] = -P_QUEEN;

    if (abs(captured) == P_KING)
    {
        gameOver = true;
        snprintf(statusMsg, sizeof(statusMsg), "%s wins by King capture!", captured < 0 ? "You (White)" : "AI (Black)");
    }
}

static void RunAIMove(void)
{
    Move moves[256];
    int n = GenerateMoves(board, -1, moves);
    if (n == 0)
    {
        gameOver = true;
        snprintf(statusMsg, sizeof(statusMsg), "Stalemate! Draw game.");
        return;
    }

    int bestScore = 1000000;
    Move bestMove = moves[0];
    for (int i = 0; i < n; i++)
    {
        int captured = board[moves[i].r2][moves[i].c2];
        int moving = board[moves[i].r1][moves[i].c1];
        board[moves[i].r2][moves[i].c2] = moving;
        board[moves[i].r1][moves[i].c1] = 0;

        int score = Minimax(board, 2, -1000000, 1000000, true);

        board[moves[i].r1][moves[i].c1] = moving;
        board[moves[i].r2][moves[i].c2] = captured;

        if (score < bestScore)
        {
            bestScore = score;
            bestMove = moves[i];
        }
    }
    ExecuteMove(bestMove);
    whiteTurn = true;
    if (!gameOver) snprintf(statusMsg, sizeof(statusMsg), "Your Turn (White)");
}

static void ComputeValidForSelected(void)
{
    memset(validMoves, 0, sizeof(validMoves));
    if (selR < 0 || selC < 0) return;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            validMoves[r][c] = IsPseudoLegal(board, selR, selC, r, c);
}

static void Draw3DChessPiece(int piece, Vector3 pos)
{
    if (piece == 0) return;
    bool isWhite = piece > 0;
    Color body = isWhite ? (Color){ 242, 244, 248, 255 } : (Color){ 36, 40, 52, 255 };
    Color trim = isWhite ? (Color){ 185, 195, 210, 255 } : (Color){ 90, 105, 130, 255 };

    // Base pedestal grounded directly at tile surface (pos.y = 0.0f)
    DrawCylinder(pos, 0.34f, 0.38f, 0.14f, 16, trim);
    Vector3 stem = { pos.x, pos.y + 0.14f, pos.z };

    switch (abs(piece))
    {
        case P_PAWN:
            DrawCylinder(stem, 0.22f, 0.28f, 0.32f, 14, body);
            DrawSphere((Vector3){ pos.x, pos.y + 0.54f, pos.z }, 0.18f, body);
            break;
        case P_ROOK:
            DrawCylinder(stem, 0.26f, 0.30f, 0.50f, 14, body);
            DrawCylinder((Vector3){ pos.x, pos.y + 0.64f, pos.z }, 0.32f, 0.28f, 0.16f, 14, trim);
            break;
        case P_KNIGHT:
            DrawCylinder(stem, 0.24f, 0.30f, 0.42f, 14, body);
            DrawCube((Vector3){ pos.x, pos.y + 0.65f, pos.z + (isWhite ? -0.06f : 0.06f) }, 0.24f, 0.34f, 0.40f, body);
            break;
        case P_BISHOP:
            DrawCylinder(stem, 0.20f, 0.29f, 0.58f, 14, body);
            DrawSphere((Vector3){ pos.x, pos.y + 0.78f, pos.z }, 0.17f, trim);
            break;
        case P_QUEEN:
            DrawCylinder(stem, 0.22f, 0.32f, 0.75f, 16, body);
            DrawSphere((Vector3){ pos.x, pos.y + 0.94f, pos.z }, 0.20f, trim);
            DrawSphere((Vector3){ pos.x, pos.y + 1.15f, pos.z }, 0.08f, COL_ACCENT);
            break;
        case P_KING:
            DrawCylinder(stem, 0.24f, 0.34f, 0.85f, 16, body);
            DrawCube((Vector3){ pos.x, pos.y + 1.12f, pos.z }, 0.09f, 0.28f, 0.09f, COL_ACCENT);
            DrawCube((Vector3){ pos.x, pos.y + 1.12f, pos.z }, 0.24f, 0.09f, 0.09f, COL_ACCENT);
            break;
    }
}

void ChessGameEnter(void)
{
    int backRank[8] = { P_ROOK, P_KNIGHT, P_BISHOP, P_QUEEN, P_KING, P_BISHOP, P_KNIGHT, P_ROOK };
    memset(board, 0, sizeof(board));
    for (int c = 0; c < 8; c++)
    {
        board[0][c] = -backRank[c];
        board[1][c] = -P_PAWN;
        board[6][c] =  P_PAWN;
        board[7][c] =  backRank[c];
    }
    whiteTurn = true;
    selR = selC = -1;
    memset(validMoves, 0, sizeof(validMoves));
    gameOver = false;
    aiDelayTimer = 0.0f;
    snprintf(statusMsg, sizeof(statusMsg), "Your Turn (White) — Click a piece to move");

    cam3d.position = (Vector3){ 0.0f, 8.2f, 7.6f };
    cam3d.target   = (Vector3){ 0.0f, 0.0f, 0.2f };
    cam3d.up       = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam3d.fovy     = 44.0f;
    cam3d.projection = CAMERA_PERSPECTIVE;
}

bool ChessGameRun(AppState *app, float dt)
{
    // Top Header Bar (Google UX Hierarchy & 48dp Touch Targets)
    UiDrawCenteredText("C\xC2\xB2 Chess vs AI", SCREEN_W / 2, 22, 30, RAYWHITE);
    UiDrawCenteredText(statusMsg, SCREEN_W / 2, 58, 18, gameOver ? COL_DONE : COL_ACCENT);

    if (UiButton((Rectangle){ 30, 26, 120, 48 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE))
        return true;

    if (UiButton((Rectangle){ SCREEN_W - 310, 26, 140, 48 }, is3DMode ? "2D View" : "3D View", true, 1.0f))
        is3DMode = !is3DMode;

    if (UiButton((Rectangle){ SCREEN_W - 160, 26, 130, 48 }, "New Game", true, 1.0f))
        ChessGameEnter();

    // AI Turn Execution
    if (!whiteTurn && !gameOver)
    {
        aiDelayTimer -= dt;
        if (aiDelayTimer <= 0.0f)
        {
            RunAIMove();
            if (gameOver && app && AppCurrentAccount(app))
            {
                AccountAwardBadge(AppCurrentAccount(app), BADGE_CHESS);
                AppSaveAccounts(app);
            }
        }
    }

    if (!is3DMode)
    {
        // 2D Chessboard
        float tile = 68.0f;
        float boardPx = tile * 8.0f;
        float startX = (SCREEN_W - boardPx) * 0.5f;
        float startY = 105.0f;

        DrawRectangleRounded((Rectangle){ startX - 12, startY - 12, boardPx + 24, boardPx + 24 }, 0.03f, 8, COL_PANEL);
        Vector2 mouse = GetMousePosition();

        for (int r = 0; r < 8; r++)
        {
            for (int c = 0; c < 8; c++)
            {
                Rectangle sq = { startX + c * tile, startY + r * tile, tile, tile };
                bool light = (r + c) % 2 == 0;
                Color tileCol = light ? (Color){ 226, 232, 240, 255 } : (Color){ 71, 85, 105, 255 };
                if (r == selR && c == selC) tileCol = (Color){ 56, 189, 248, 255 };
                DrawRectangleRec(sq, tileCol);

                if (validMoves[r][c])
                {
                    DrawCircle((int)(sq.x + tile * 0.5f), (int)(sq.y + tile * 0.5f),
                               board[r][c] != 0 ? 28.0f : 11.0f, Fade(COL_DONE, 0.55f));
                }

                int p = board[r][c];
                if (p != 0) Draw2DPiece(p, sq);

                if (whiteTurn && !gameOver && CheckCollisionPointRec(mouse, sq) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    if (selR >= 0 && validMoves[r][c])
                    {
                        ExecuteMove((Move){ selR, selC, r, c });
                        selR = selC = -1;
                        memset(validMoves, 0, sizeof(validMoves));
                        if (!gameOver)
                        {
                            whiteTurn = false;
                            aiDelayTimer = 0.35f;
                            snprintf(statusMsg, sizeof(statusMsg), "AI (Black) is thinking...");
                        }
                    }
                    else if (board[r][c] > 0)
                    {
                        selR = r; selC = c;
                        ComputeValidForSelected();
                    }
                    else
                    {
                        selR = selC = -1;
                        memset(validMoves, 0, sizeof(validMoves));
                    }
                }
            }
        }
    }
    else
    {
        // 3D Grounded Chessboard & Interactive Raycast Picking
        if (IsKeyDown(KEY_LEFT))  cam3d.position.x -= 6.0f * dt;
        if (IsKeyDown(KEY_RIGHT)) cam3d.position.x += 6.0f * dt;

        Ray mouseRay = GetScreenToWorldRayEx(GetMousePosition(), cam3d, SCREEN_W, SCREEN_H);

        BeginMode3D(cam3d);
        // Grounded Table Plane + Base Pedestal (no floating board!)
        DrawPlane((Vector3){ 0.0f, -0.36f, 0.0f }, (Vector2){ 24.0f, 24.0f }, (Color){ 18, 22, 32, 255 });
        DrawCube((Vector3){ 0.0f, -0.20f, 0.0f }, 8.6f, 0.32f, 8.6f, (Color){ 42, 48, 62, 255 });
        DrawCubeWires((Vector3){ 0.0f, -0.20f, 0.0f }, 8.6f, 0.32f, 8.6f, COL_MUTED);

        for (int r = 0; r < 8; r++)
        {
            for (int c = 0; c < 8; c++)
            {
                float wx = (c - 3.5f) * 1.0f;
                float wz = (r - 3.5f) * 1.0f;
                bool light = (r + c) % 2 == 0;
                Color sqCol = light ? (Color){ 226, 232, 240, 255 } : (Color){ 71, 85, 105, 255 };
                if (r == selR && c == selC) sqCol = COL_CYAN;
                else if (validMoves[r][c]) sqCol = COL_DONE;

                DrawCube((Vector3){ wx, -0.02f, wz }, 0.98f, 0.04f, 0.98f, sqCol);

                if (board[r][c] != 0)
                    Draw3DChessPiece(board[r][c], (Vector3){ wx, 0.0f, wz });

                // Raycast click on 3D square
                if (whiteTurn && !gameOver && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    BoundingBox box = {
                        (Vector3){ wx - 0.49f, -0.04f, wz - 0.49f },
                        (Vector3){ wx + 0.49f,  0.90f, wz + 0.49f }
                    };
                    if (GetRayCollisionBox(mouseRay, box).hit)
                    {
                        if (selR >= 0 && validMoves[r][c])
                        {
                            ExecuteMove((Move){ selR, selC, r, c });
                            selR = selC = -1;
                            memset(validMoves, 0, sizeof(validMoves));
                            if (!gameOver)
                            {
                                whiteTurn = false;
                                aiDelayTimer = 0.35f;
                                snprintf(statusMsg, sizeof(statusMsg), "AI (Black) is thinking...");
                            }
                        }
                        else if (board[r][c] > 0)
                        {
                            selR = r; selC = c;
                            ComputeValidForSelected();
                        }
                    }
                }
            }
        }
        EndMode3D();

        UiDrawCenteredText("Click tiles directly in 3D to select and move  |  Left/Right arrows rotate view",
                           SCREEN_W / 2, SCREEN_H - 36, 16, COL_MUTED);
    }

    return false;
}