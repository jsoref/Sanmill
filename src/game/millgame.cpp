﻿/*****************************************************************************
 * Copyright (C) 2018-2019 MillGame authors
 *
 * Authors: liuweilhy <liuweilhy@163.com>
 *          Calcitem <calcitem@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include <algorithm>
#include "millgame.h"
#include "search.h"
#include <QDebug>

// 对静态常量数组的定义要放在类外，不要放在头文件
// 预定义的4套规则
const struct MillGame::Rule MillGame::RULES[N_RULES] = {
{
    "成三棋",   // 成三棋
    // 规则说明
    "1. 双方各9颗子，开局依次摆子；\n"
    "2. 凡出现三子相连，就提掉对手一子；\n"
    "3. 不能提对手的“三连”子，除非无子可提；\n"
    "4. 同时出现两个“三连”只能提一子；\n"
    "5. 摆完后依次走子，每次只能往相邻位置走一步；\n"
    "6. 把对手棋子提到少于3颗时胜利；\n"
    "7. 走棋阶段不能行动（被“闷”）算负。",
    9,          // 双方各9子
    3,          // 赛点子数为3
    false,      // 没有斜线
    false,      // 没有禁点，摆棋阶段被提子的点可以再摆子
    false,      // 先摆棋者先行棋
    true,       // 可以重复成三
    false,      // 多个“三连”只能提一子
    false,      // 不能提对手的“三连”子，除非无子可提；
    true,       // 摆棋满子（闷棋，只有12子棋才出现）算先手负
    true,       // 走棋阶段不能行动（被“闷”）算负
    false,      // 剩三子时不可以飞棋
    0,          // 不计步数
    0           // 不计时
},
{
    "打三棋(12连棋)",           // 打三棋
    // 规则说明
    "1. 双方各12颗子，棋盘有斜线；\n"
    "2. 摆棋阶段被提子的位置不能再摆子，直到走棋阶段；\n"
    "3. 摆棋阶段，摆满棋盘算先手负；\n"
    "4. 走棋阶段，后摆棋的一方先走；\n"
    "5. 同时出现两个“三连”只能提一子；\n"
    "6. 其它规则与成三棋基本相同。",
    12,          // 双方各12子
    3,          // 赛点子数为3
    true,       // 有斜线
    true,       // 有禁点，摆棋阶段被提子的点不能再摆子
    true,       // 后摆棋者先行棋
    true,       // 可以重复成三
    false,      // 多个“三连”只能提一子
    true,       // 可以提对手的“三连”子
    true,       // 摆棋满子（闷棋，只有12子棋才出现）算先手负
    true,       // 走棋阶段不能行动（被“闷”）算负
    false,      // 剩三子时不可以飞棋
    50,          // 不计步数
    0           // 不计时
},
{
    "九连棋",   // 九连棋
    // 规则说明
    "1. 规则与成三棋基本相同，只是它的棋子有序号，\n"
    "2. 相同序号、位置的“三连”不能重复提子；\n"
    "3. 走棋阶段不能行动（被“闷”），则由对手继续走棋；\n"
    "4. 一步出现几个“三连”就可以提几个子。",
    9,          // 双方各9子
    3,          // 赛点子数为3
    false,      // 没有斜线
    false,      // 没有禁点，摆棋阶段被提子的点可以再摆子
    false,      // 先摆棋者先行棋
    false,      // 不可以重复成三
    true,       // 出现几个“三连”就可以提几个子
    false,      // 不能提对手的“三连”子，除非无子可提；
    true,       // 摆棋满子（闷棋，只有12子棋才出现）算先手负
    false,      // 走棋阶段不能行动（被“闷”），则由对手继续走棋
    false,      // 剩三子时不可以飞棋
    0,          // 不计步数
    0           // 不计时
},
{
    "莫里斯九子棋",      // 莫里斯九子棋
    // 规则说明
    "规则与成三棋基本相同，只是在走子阶段，当一方仅剩3子时，他可以飞子到任意空位。",
    9,          // 双方各9子
    3,          // 赛点子数为3
    false,      // 没有斜线
    false,      // 没有禁点，摆棋阶段被提子的点可以再摆子
    false,      // 先摆棋者先行棋
    true,       // 可以重复成三
    false,      // 多个“三连”只能提一子
    false,      // 不能提对手的“三连”子，除非无子可提；
    true,       // 摆棋满子（闷棋，只有12子棋才出现）算先手负
    true,       // 走棋阶段不能行动（被“闷”）算负
    true,       // 剩三子时可以飞棋
    0,          // 不计步数
    0           // 不计时
}
};

// 名义上是个数组，实际上相当于一个判断是否在棋盘上的函数
const int MillGame::onBoard[N_POINTS] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 着法表
int MillGame::moveTable[N_POINTS][N_MOVE_DIRECTIONS] = {{0}};

// 成三表
int MillGame::millTable[N_POINTS][N_DIRECTIONS][N_RINGS - 1] = {{{0}}};

MillGame::MillGame()
{
    // 单独提出 board 等数据，免得每次都写 context.board;
    board_ = context.board;

 #if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
    // 创建哈希数据
    constructHash();
#endif

#ifdef BOOK_LEARNING
    // TODO: 开局库文件被加载了多次
    MillGameAi_ab::loadOpeningBookFileToHashMap();
#endif

    // 默认选择第1号规则，即“打三棋”
    setContext(&RULES[1]);

    // 比分归零
    score_1 = score_2 = score_draw = 0;
}

MillGame::~MillGame() = default;

MillGame::MillGame(const MillGame &chess)
{  
    *this = chess;
}

MillGame &MillGame::operator = (const MillGame &chess)
{
    if (this == &chess)
        return *this;

    currentRule = chess.currentRule;
    context = chess.context;
    currentStep = chess.currentStep;
    moveStep = chess.moveStep;
    board_ = context.board;
    currentPos = chess.currentPos;
    winner = chess.winner;
    startTime = chess.startTime;
    currentTime = chess.currentTime;
    elapsedSeconds_1 = chess.elapsedSeconds_1;
    elapsedSeconds_2 = chess.elapsedSeconds_2;
    move_ = chess.move_;
    memcpy(cmdline, chess.cmdline, sizeof(cmdline));
    cmdlist = chess.cmdlist;
    tips = chess.tips;

    return *this;
}

int MillGame::playerToId(enum Player player)
{
    if (player == MillGame::PLAYER1)
        return 1;
    else if (player == MillGame::PLAYER2)
        return 2;

    return 0;
}

MillGame::Player MillGame::getOpponent(MillGame::Player player)
{
    switch (player)
    {
    case PLAYER1:
        return PLAYER2;
    case PLAYER2:
        return PLAYER1;
    default:
        break;
    }

    return NOBODY;
}

void MillGame::createMoveTable()
{
#ifdef CONST_MOVE_TABLE
#ifdef MOVE_PRIORITY_TABLE_SUPPORT
    const int moveTable_obliqueLine[MillGame::N_POINTS][MillGame::N_MOVE_DIRECTIONS] = {
        /*  0 */ {0, 0, 0, 0},
        /*  1 */ {0, 0, 0, 0},
        /*  2 */ {0, 0, 0, 0},
        /*  3 */ {0, 0, 0, 0},
        /*  4 */ {0, 0, 0, 0},
        /*  5 */ {0, 0, 0, 0},
        /*  6 */ {0, 0, 0, 0},
        /*  7 */ {0, 0, 0, 0},

        /*  8 */ {9, 15, 16, 0},
        /*  9 */ {17, 8, 10, 0},
        /* 10 */ {9, 11, 18, 0},
        /* 11 */ {19, 10, 12, 0},
        /* 12 */ {11, 13, 20, 0},
        /* 13 */ {21, 12, 14, 0},
        /* 14 */ {13, 15, 22, 0},
        /* 15 */ {23, 8, 14, 0},

        /* 16 */ {17, 23, 8, 24},
        /* 17 */ {9, 25, 16, 18},
        /* 18 */ {17, 19, 10, 26},
        /* 19 */ {11, 27, 18, 20},
        /* 20 */ {19, 21, 12, 28},
        /* 21 */ {13, 29, 20, 22},
        /* 22 */ {21, 23, 14, 30},
        /* 23 */ {15, 31, 16, 22},

        /* 24 */ {25, 31, 16, 0},
        /* 25 */ {17, 24, 26, 0},
        /* 26 */ {25, 27, 18, 0},
        /* 27 */ {19, 26, 28, 0},
        /* 28 */ {27, 29, 20, 0},
        /* 29 */ {21, 28, 30, 0},
        /* 30 */ {29, 31, 22, 0},
        /* 31 */ {23, 24, 30, 0},

        /* 32 */ {0, 0, 0, 0},
        /* 33 */ {0, 0, 0, 0},
        /* 34 */ {0, 0, 0, 0},
        /* 35 */ {0, 0, 0, 0},
        /* 36 */ {0, 0, 0, 0},
        /* 37 */ {0, 0, 0, 0},
        /* 38 */ {0, 0, 0, 0},
        /* 39 */ {0, 0, 0, 0},
    };

    const int moveTable_noObliqueLine[MillGame::N_POINTS][MillGame::N_MOVE_DIRECTIONS] = {
        /*  0 */ {0, 0, 0, 0},
        /*  1 */ {0, 0, 0, 0},
        /*  2 */ {0, 0, 0, 0},
        /*  3 */ {0, 0, 0, 0},
        /*  4 */ {0, 0, 0, 0},
        /*  5 */ {0, 0, 0, 0},
        /*  6 */ {0, 0, 0, 0},
        /*  7 */ {0, 0, 0, 0},

        /*  8 */ {16, 9, 15, 0},
        /*  9 */ {10, 8, 0, 0},
        /* 10 */ {18, 11, 9, 0},
        /* 11 */ {12, 10, 0, 0},
        /* 12 */ {20, 13, 11, 0},
        /* 13 */ {14, 12, 0, 0},
        /* 14 */ {22, 15, 13, 0},
        /* 15 */ {8, 14, 0, 0},

        /* 16 */ {8, 24, 17, 23},
        /* 17 */ {18, 16, 0, 0},
        /* 18 */ {10, 26, 19, 17},
        /* 19 */ {20, 18, 0, 0},
        /* 20 */ {12, 28, 21, 19},
        /* 21 */ {22, 20, 0, 0},
        /* 22 */ {14, 30, 23, 21},
        /* 23 */ {16, 22, 0, 0},

        /* 24 */ {16, 25, 31, 0},
        /* 25 */ {26, 24, 0, 0},
        /* 26 */ {18, 27, 25, 0},
        /* 27 */ {28, 26, 0, 0},
        /* 28 */ {20, 29, 27, 0},
        /* 29 */ {30, 28, 0, 0},
        /* 30 */ {22, 31, 29, 0},
        /* 31 */ {24, 30, 0, 0},

        /* 32 */ {0, 0, 0, 0},
        /* 33 */ {0, 0, 0, 0},
        /* 34 */ {0, 0, 0, 0},
        /* 35 */ {0, 0, 0, 0},
        /* 36 */ {0, 0, 0, 0},
        /* 37 */ {0, 0, 0, 0},
        /* 38 */ {0, 0, 0, 0},
        /* 39 */ {0, 0, 0, 0},
    };
#else
    const int moveTable_obliqueLine[MillGame::N_POINTS][MillGame::N_MOVE_DIRECTIONS] = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},

        {9, 15, 0, 16},
        {10, 8, 0, 17},
        {11, 9, 0, 18},
        {12, 10, 0, 19},
        {13, 11, 0, 20},
        {14, 12, 0, 21},
        {15, 13, 0, 22},
        {8, 14, 0, 23},

        {17, 23, 8, 24},
        {18, 16, 9, 25},
        {19, 17, 10, 26},
        {20, 18, 11, 27},
        {21, 19, 12, 28},
        {22, 20, 13, 29},
        {23, 21, 14, 30},
        {16, 22, 15, 31},

        {25, 31, 16, 0},
        {26, 24, 17, 0},
        {27, 25, 18, 0},
        {28, 26, 19, 0},
        {29, 27, 20, 0},
        {30, 28, 21, 0},
        {31, 29, 22, 0},
        {24, 30, 23, 0},

        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };

    const int moveTable_noObliqueLine[MillGame::N_POINTS][MillGame::N_MOVE_DIRECTIONS] = {
        /*  0 */ {0, 0, 0, 0},
        /*  1 */ {0, 0, 0, 0},
        /*  2 */ {0, 0, 0, 0},
        /*  3 */ {0, 0, 0, 0},
        /*  4 */ {0, 0, 0, 0},
        /*  5 */ {0, 0, 0, 0},
        /*  6 */ {0, 0, 0, 0},
        /*  7 */ {0, 0, 0, 0},

        /*  8 */ {9, 15, 0, 16},
        /*  9 */ {10, 8, 0, 0},
        /* 10 */ {11, 9, 0, 18},
        /* 11 */ {12, 10, 0, 0},
        /* 12 */ {13, 11, 0, 20},
        /* 13 */ {14, 12, 0, 0},
        /* 14 */ {15, 13, 0, 22},
        /* 15 */ {8, 14, 0, 0},

        /* 16 */ {17, 23, 8, 24},
        /* 17 */ {18, 16, 0, 0},
        /* 18 */ {19, 17, 10, 26},
        /* 19 */ {20, 18, 0, 0},
        /* 20 */ {21, 19, 12, 28},
        /* 21 */ {22, 20, 0, 0},
        /* 22 */ {23, 21, 14, 30},
        /* 23 */ {16, 22, 0, 0},

        /* 24 */ {25, 31, 16, 0},
        /* 25 */ {26, 24, 0, 0},
        /* 26 */ {27, 25, 18, 0},
        /* 27 */ {28, 26, 0, 0},
        /* 28 */ {29, 27, 20, 0},
        /* 29 */ {30, 28, 0, 0},
        /* 30 */ {31, 29, 22, 0},
        /* 31 */ {24, 30, 0, 0},

        /* 32 */ {0, 0, 0, 0},
        /* 33 */ {0, 0, 0, 0},
        /* 34 */ {0, 0, 0, 0},
        /* 35 */ {0, 0, 0, 0},
        /* 36 */ {0, 0, 0, 0},
        /* 37 */ {0, 0, 0, 0},
        /* 38 */ {0, 0, 0, 0},
        /* 39 */ {0, 0, 0, 0},
    };
#endif /* MOVE_PRIORITY_TABLE_SUPPORT */

    if (currentRule.hasObliqueLines) {
        memcpy(moveTable, moveTable_obliqueLine, sizeof(moveTable));
    } else {
        memcpy(moveTable, moveTable_noObliqueLine, sizeof(moveTable));
    }

#else /* CONST_MOVE_TABLE */

    for (int r = 1; r <= N_RINGS; r++) {
        for (int s = 0; s < N_SEATS; s++) {
            int p = r * N_SEATS + s;

            // 顺时针走一步的位置
            moveTable[p][MOVE_DIRECTION_CLOCKWISE] = r * N_SEATS + (s + 1) % N_SEATS;

            // 逆时针走一步的位置
            moveTable[p][MOVE_DIRECTION_ANTICLOCKWISE] = r * N_SEATS + (s + N_SEATS - 1) % N_SEATS;

            // 如果是 0、2、4、6位（偶数位）或是有斜线
            if (!(s & 1) || this->currentRule.hasObliqueLines) {
                if (r > 1) {
                    // 向内走一步的位置
                    moveTable[p][MOVE_DIRECTION_INWARD] = (r - 1) * N_SEATS + s;
                }

                if (r < N_RINGS) {
                    // 向外走一步的位置
                    moveTable[p][MOVE_DIRECTION_OUTWARD] = (r + 1) * N_SEATS + s;
                }
            }
#if 0
            // 对于无斜线情况下的1、3、5、7位（奇数位），则都设为棋盘外点（默认'\x00'）
            else {
                // 向内走一步的位置设为随便棋盘外一点
                moveTable[i * SEAT + j][2] = '\x00';
                // 向外走一步的位置设为随便棋盘外一点
                moveTable[i * SEAT + j][3] = '\x00';
            }
#endif
        }
    }
#endif /* CONST_MOVE_TABLE */

#if 0
    int sum =  0;
    for (int i = 0; i < N_POINTS; i++) {
        printf("/* %d */ {", i);
        for (int j = 0; j < N_MOVE_DIRECTIONS; j++) {
            if (j == N_MOVE_DIRECTIONS - 1)
                printf("%d", moveTable[i][j]);
            else
                printf("%d, ", moveTable[i][j]);
            sum += moveTable[i][j];
        }
        printf("},\n");
    }
    qDebug() << "sum = " << sum;
#endif
}

void MillGame::createMillTable()
{
#ifdef CONST_MILL_TABLE
    const int millTable_noObliqueLine[MillGame::N_POINTS][MillGame::N_DIRECTIONS][2] = {
        /* 0 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 1 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 2 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 3 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 4 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 5 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 6 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 7 */ {{0, 0}, {0, 0}, {0, 0}},

        /* 8 */ {{16, 24}, {9, 15}, {0, 0}},
        /* 9 */ {{0, 0}, {15, 8}, {10, 11}},
        /* 10 */ {{18, 26}, {11, 9}, {0, 0}},
        /* 11 */ {{0, 0}, {9, 10}, {12, 13}},
        /* 12 */ {{20, 28}, {13, 11}, {0, 0}},
        /* 13 */ {{0, 0}, {11, 12}, {14, 15}},
        /* 14 */ {{22, 30}, {15, 13}, {0, 0}},
        /* 15 */ {{0, 0}, {13, 14}, {8, 9}},

        /* 16 */ {{8, 24}, {17, 23}, {0, 0}},
        /* 17 */ {{0, 0}, {23, 16}, {18, 19}},
        /* 18 */ {{10, 26}, {19, 17}, {0, 0}},
        /* 19 */ {{0, 0}, {17, 18}, {20, 21}},
        /* 20 */ {{12, 28}, {21, 19}, {0, 0}},
        /* 21 */ {{0, 0}, {19, 20}, {22, 23}},
        /* 22 */ {{14, 30}, {23, 21}, {0, 0}},
        /* 23 */ {{0, 0}, {21, 22}, {16, 17}},

        /* 24 */ {{8, 16}, {25, 31}, {0, 0}},
        /* 25 */ {{0, 0}, {31, 24}, {26, 27}},
        /* 26 */ {{10, 18}, {27, 25}, {0, 0}},
        /* 27 */ {{0, 0}, {25, 26}, {28, 29}},
        /* 28 */ {{12, 20}, {29, 27}, {0, 0}},
        /* 29 */ {{0, 0}, {27, 28}, {30, 31}},
        /* 30 */ {{14, 22}, {31, 29}, {0, 0}},
        /* 31 */ {{0, 0}, {29, 30}, {24, 25}},

        /* 32 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 33 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 34 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 35 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 36 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 37 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 38 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 39 */ {{0, 0}, {0, 0}, {0, 0}}
    };

    const int millTable_hasObliqueLines[MillGame::N_POINTS][MillGame::N_DIRECTIONS][2] = {
        /*  0 */ {{0, 0}, {0, 0}, {0, 0}},
        /*  1 */ {{0, 0}, {0, 0}, {0, 0}},
        /*  2 */ {{0, 0}, {0, 0}, {0, 0}},
        /*  3 */ {{0, 0}, {0, 0}, {0, 0}},
        /*  4 */ {{0, 0}, {0, 0}, {0, 0}},
        /*  5 */ {{0, 0}, {0, 0}, {0, 0}},
        /*  6 */ {{0, 0}, {0, 0}, {0, 0}},
        /*  7 */ {{0, 0}, {0, 0}, {0, 0}},

        /*  8 */ {{16, 24}, {9, 15}, {0, 0}},
        /*  9 */ {{17, 25}, {15, 8}, {10, 11}},
        /* 10 */ {{18, 26}, {11, 9}, {0, 0}},
        /* 11 */ {{19, 27}, {9, 10}, {12, 13}},
        /* 12 */ {{20, 28}, {13, 11}, {0, 0}},
        /* 13 */ {{21, 29}, {11, 12}, {14, 15}},
        /* 14 */ {{22, 30}, {15, 13}, {0, 0}},
        /* 15 */ {{23, 31}, {13, 14}, {8, 9}},

        /* 16 */ {{8, 24}, {17, 23}, {0, 0}},
        /* 17 */ {{9, 25}, {23, 16}, {18, 19}},
        /* 18 */ {{10, 26}, {19, 17}, {0, 0}},
        /* 19 */ {{11, 27}, {17, 18}, {20, 21}},
        /* 20 */ {{12, 28}, {21, 19}, {0, 0}},
        /* 21 */ {{13, 29}, {19, 20}, {22, 23}},
        /* 22 */ {{14, 30}, {23, 21}, {0, 0}},
        /* 23 */ {{15, 31}, {21, 22}, {16, 17}},

        /* 24 */ {{8, 16}, {25, 31}, {0, 0}},
        /* 25 */ {{9, 17}, {31, 24}, {26, 27}},
        /* 26 */ {{10, 18}, {27, 25}, {0, 0}},
        /* 27 */ {{11, 19}, {25, 26}, {28, 29}},
        /* 28 */ {{12, 20}, {29, 27}, {0, 0}},
        /* 29 */ {{13, 21}, {27, 28}, {30, 31}},
        /* 30 */ {{14, 22}, {31, 29}, {0, 0}},
        /* 31 */ {{15, 23}, {29, 30}, {24, 25}},

        /* 32 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 33 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 34 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 35 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 36 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 37 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 38 */ {{0, 0}, {0, 0}, {0, 0}},
        /* 39 */ {{0, 0}, {0, 0}, {0, 0}}
    };

    if (currentRule.hasObliqueLines) {
        memcpy(millTable, millTable_hasObliqueLines, sizeof(millTable));
    } else {
        memcpy(millTable, millTable_noObliqueLine, sizeof(millTable));
    }
#else /* CONST_MILL_TABLE */
    for (int i = 0; i < N_SEATS; i++) {
        // 内外方向的“成三”
        // 如果是0、2、4、6位（偶数位）或是有斜线
        if (!(i & 1) || this->currentRule.hasObliqueLines) {
            millTable[1 * N_SEATS + i][0][0] = 2 * N_SEATS + i;
            millTable[1 * N_SEATS + i][0][1] = 3 * N_SEATS + i;

            millTable[2 * N_SEATS + i][0][0] = 1 * N_SEATS + i;
            millTable[2 * N_SEATS + i][0][1] = 3 * N_SEATS + i;

            millTable[3 * N_SEATS + i][0][0] = 1 * N_SEATS + i;
            millTable[3 * N_SEATS + i][0][1] = 2 * N_SEATS + i;
        }
        // 对于无斜线情况下的1、3、5、7位（奇数位）
        else {
            // 置空该组“成三”
            millTable[1 * N_SEATS + i][0][0] = 0;
            millTable[1 * N_SEATS + i][0][1] = 0;

            millTable[2 * N_SEATS + i][0][0] = 0;
            millTable[2 * N_SEATS + i][0][1] = 0;

            millTable[3 * N_SEATS + i][0][0] = 0;
            millTable[3 * N_SEATS + i][0][1] = 0;
        }

        // 当前圈上的“成三”
        // 如果是0、2、4、6位
        if (!(i & 1)) {
            millTable[1 * N_SEATS + i][1][0] = 1 * N_SEATS + (i + 1) % N_SEATS;
            millTable[1 * N_SEATS + i][1][1] = 1 * N_SEATS + (i + N_SEATS - 1) % N_SEATS;

            millTable[2 * N_SEATS + i][1][0] = 2 * N_SEATS + (i + 1) % N_SEATS;
            millTable[2 * N_SEATS + i][1][1] = 2 * N_SEATS + (i + N_SEATS - 1) % N_SEATS;

            millTable[3 * N_SEATS + i][1][0] = 3 * N_SEATS + (i + 1) % N_SEATS;
            millTable[3 * N_SEATS + i][1][1] = 3 * N_SEATS + (i + N_SEATS - 1) % N_SEATS;
            // 置空另一组“成三”
            millTable[1 * N_SEATS + i][2][0] = 0;
            millTable[1 * N_SEATS + i][2][1] = 0;

            millTable[2 * N_SEATS + i][2][0] = 0;
            millTable[2 * N_SEATS + i][2][1] = 0;

            millTable[3 * N_SEATS + i][2][0] = 0;
            millTable[3 * N_SEATS + i][2][1] = 0;
        }
        // 对于1、3、5、7位（奇数位）
        else {
            // 当前圈上逆时针的“成三”
            millTable[1 * N_SEATS + i][1][0] = 1 * N_SEATS + (i + N_SEATS - 2) % N_SEATS;
            millTable[1 * N_SEATS + i][1][1] = 1 * N_SEATS + (i + N_SEATS - 1) % N_SEATS;

            millTable[2 * N_SEATS + i][1][0] = 2 * N_SEATS + (i + N_SEATS - 2) % N_SEATS;
            millTable[2 * N_SEATS + i][1][1] = 2 * N_SEATS + (i + N_SEATS - 1) % N_SEATS;

            millTable[3 * N_SEATS + i][1][0] = 3 * N_SEATS + (i + N_SEATS - 2) % N_SEATS;
            millTable[3 * N_SEATS + i][1][1] = 3 * N_SEATS + (i + N_SEATS - 1) % N_SEATS;

            // 当前圈上顺时针的“成三”
            millTable[1 * N_SEATS + i][2][0] = 1 * N_SEATS + (i + 1) % N_SEATS;
            millTable[1 * N_SEATS + i][2][1] = 1 * N_SEATS + (i + 2) % N_SEATS;

            millTable[2 * N_SEATS + i][2][0] = 2 * N_SEATS + (i + 1) % N_SEATS;
            millTable[2 * N_SEATS + i][2][1] = 2 * N_SEATS + (i + 2) % N_SEATS;

            millTable[3 * N_SEATS + i][2][0] = 3 * N_SEATS + (i + 1) % N_SEATS;
            millTable[3 * N_SEATS + i][2][1] = 3 * N_SEATS + (i + 2) % N_SEATS;
        }
    }
#endif /* CONST_MILL_TABLE */

#if 0
    for (int i = 0; i < N_POINTS; i++) {
        printf("/* %d */ {", i);
        for (int j = 0; j < N_DIRECTIONS; j++) {
            printf("{");
            for (int k = 0; k < 2; k++) {
                if (k == 0) {
                    printf("%d, ", millTable[i][j][k]);
                } else {
                    printf("%d", millTable[i][j][k]);
                }

            }
            if (j == 2)
                printf("}");
            else
                printf("}, ");
        }
        printf("},\n");
    }

    printf("======== millTable End =========\n");

#endif
}

// 设置棋局状态和棋盘数据，用于初始化
bool MillGame::setContext(const struct Rule *rule, step_t maxStepsLedToDraw, int maxTimeLedToLose,
                        step_t initialStep, int flags, const char *board,
                        int nPiecesInHand_1, int nPiecesInHand_2, int nPiecesNeedRemove)
{
    // 有效性判断
    if (maxTimeLedToLose < 0) {
        return false;
    }

    // 根据规则
    this->currentRule = *rule;
    this->currentRule.maxStepsLedToDraw = maxStepsLedToDraw;
    this->currentRule.maxTimeLedToLose = maxTimeLedToLose;

    // 设置棋局数据

    // 设置步数
    this->currentStep = initialStep;
    this->moveStep = initialStep;

    // 局面阶段标识
    if (flags & GAME_NOTSTARTED) {
        context.stage = GAME_NOTSTARTED;
    } else if (flags & GAME_PLACING) {
        context.stage = GAME_PLACING;
    } else if (flags & GAME_MOVING) {
        context.stage = GAME_MOVING;
    } else if (flags & GAME_OVER) {
        context.stage = GAME_OVER;
    } else {
        return false;
    }

    // 轮流状态标识
    if (flags & PLAYER1) {
        context.turn = PLAYER1;
    } else if (flags & PLAYER2) {
        context.turn = PLAYER2;
    } else {
        return false;
    }

    // 动作状态标识
    if (flags & ACTION_CHOOSE) {
        context.action = ACTION_CHOOSE;
    } else if (flags & ACTION_PLACE) {
        context.action = ACTION_PLACE;
    } else if (flags & ACTION_CAPTURE) {
        context.action = ACTION_CAPTURE;
    } else {
        return false;
    }

    // 当前棋局（3×8）
    if (board == nullptr) {
        memset(context.board, 0, sizeof(context.board));
#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
        context.hash = 0;
#endif
    } else {
        memcpy(context.board, board, sizeof(context.board));
    }

    // 计算盘面子数
    // 棋局，抽象为一个（5×8）的数组，上下两行留空
    /*
        0x00 代表无棋子
        0x0F 代表禁点
        0x11～0x1C 代表先手第 1～12 子
        0x21～0x2C 代表后手第 1～12 子
        判断棋子是先手的用 (board[i] & 0x10)
        判断棋子是后手的用 (board[i] & 0x20)
     */
    context.nPiecesOnBoard_1 = context.nPiecesOnBoard_2 = 0;

    for (int r = 1; r < N_RINGS + 2; r++) {
        for (int s = 0; s < N_SEATS; s++) {
            int pos = r * N_SEATS + s;
            if (context.board[pos] & 0x10) {
                context.nPiecesOnBoard_1++;
            } else if (context.board[pos] & 0x20) {
                context.nPiecesOnBoard_2++;
            } else if (context.board[pos] & 0x0F) {
                // 不计算盘面子数
            }
        }
    }

    // 设置玩家盘面剩余子数和未放置子数
    if (context.nPiecesOnBoard_1 > rule->nTotalPiecesEachSide ||
        context.nPiecesOnBoard_2 > rule->nTotalPiecesEachSide) {
        return false;
    }

    if (nPiecesInHand_1 < 0 || nPiecesInHand_2 < 0) {
        return false;
    }

    context.nPiecesInHand_1 = rule->nTotalPiecesEachSide - context.nPiecesOnBoard_1;
    context.nPiecesInHand_2 = rule->nTotalPiecesEachSide - context.nPiecesOnBoard_2;
    context.nPiecesInHand_1 = std::min(nPiecesInHand_1, context.nPiecesInHand_1);
    context.nPiecesInHand_2 = std::min(nPiecesInHand_2, context.nPiecesInHand_2);

    // 设置去子状态时的剩余尚待去除子数
    if (flags & ACTION_CAPTURE) {
        if (0 <= nPiecesNeedRemove && nPiecesNeedRemove < 3) {
            context.nPiecesNeedRemove = nPiecesNeedRemove;
        }
    } else {
        context.nPiecesNeedRemove = 0;
    }

    // 清空成三记录
    context.millList.clear();

    // 胜负标识
    winner = NOBODY;

    // 生成着法表
    createMoveTable();

    // 生成成三表
    createMillTable();

    // 不选中棋子
    currentPos = 0;

    // 用时置零
    elapsedSeconds_1 = elapsedSeconds_2 = 0;

    // 提示
    setTips();

    // 计棋谱
    cmdlist.clear();

    int r;
    for (r = 0; r < N_RULES; r++) {
        if (strcmp(this->currentRule.name, RULES[r].name) == 0)
            break;
    }

    if (sprintf(cmdline, "r%1u s%03u t%02u", r + 1, maxStepsLedToDraw, maxTimeLedToLose) > 0) {
        cmdlist.emplace_back(string(cmdline));
        return true;
    }

    cmdline[0] = '\0';
    return false;
}

void MillGame::getContext(struct Rule &rule, step_t &step, int &flags,
                           int *&board, int &nPiecesInHand_1, int &nPiecesInHand_2, int &nPiecesNeedRemove)
{
    rule = this->currentRule;
    step = this->currentStep;
    flags = context.stage | context.turn | context.action;
    board_ = board;
    nPiecesInHand_1 = context.nPiecesInHand_1;
    nPiecesInHand_2 = context.nPiecesInHand_2;
    nPiecesNeedRemove = context.nPiecesNeedRemove;
}

bool MillGame::reset()
{
    if (context.stage == GAME_NOTSTARTED &&
        elapsedSeconds_1 == elapsedSeconds_2 == 0) {
        return true;
    }

    // 步数归零
    currentStep = 0;
    moveStep = 0;

    // 局面阶段标识
    context.stage = GAME_NOTSTARTED;

    // 轮流状态标识
    context.turn = PLAYER1;

    // 动作状态标识
    context.action = ACTION_PLACE;

    // 胜负标识
    winner = NOBODY;

    // 当前棋局（3×8）
    memset(board_, 0, sizeof(context.board));

    // 盘面子数归零
    context.nPiecesOnBoard_1 = context.nPiecesOnBoard_2 = 0;

    // 设置玩家盘面剩余子数和未放置子数
    context.nPiecesInHand_1 = context.nPiecesInHand_2 = currentRule.nTotalPiecesEachSide;

    // 设置去子状态时的剩余尚待去除子数
    context.nPiecesNeedRemove = 0;

    // 清空成三记录
    context.millList.clear();

    // 不选中棋子
    currentPos = 0;

    // 用时置零
    elapsedSeconds_1 = elapsedSeconds_2 = 0;

#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
    // 哈希归零
    context.hash = 0;
#endif

    // 提示
    setTips();

    // 计棋谱
    cmdlist.clear();

    int i;

    for (i = 0; i < N_RULES; i++) {
        if (strcmp(this->currentRule.name, RULES[i].name) == 0)
            break;
    }

    if (sprintf(cmdline, "r%1u s%03u t%02u",
                i + 1, currentRule.maxStepsLedToDraw, currentRule.maxTimeLedToLose) > 0) {
        cmdlist.emplace_back(string(cmdline));
        return true;
    }

    cmdline[0] = '\0';

    return false;
}

bool MillGame::start()
{
    switch (context.stage) {
    // 如果游戏已经开始，则返回false
    case GAME_PLACING:
    case GAME_MOVING:
        return false;
    // 如果游戏结束，则重置游戏，进入未开始状态
    case GAME_OVER:
        reset();
        [[fallthrough]];
    // 如果游戏处于未开始状态
    case GAME_NOTSTARTED:
        // 启动计时器
        startTime = time(NULL);
        // 进入开局状态
        context.stage = GAME_PLACING;
        return true;
    default:
        return false;
    }
}

// Unused
bool MillGame::getPieceRS(const Player &player, const int &number, int &r, int &s)
{
    int piece;

    if (player == PLAYER1) {
        piece = 0x10;
    } else if (player == PLAYER2) {
        piece = 0x20;
    } else {
        return false;
    }

    if (number > 0 && number <= currentRule.nTotalPiecesEachSide)
        piece &= number;
    else
        return false;

    for (int i = POS_BEGIN; i < POS_END; i++) {
        if (board_[i] == piece) {
            pos2rs(i, r, s);
            return true;
        }
    }

    return false;
}

// 获取当前棋子
bool MillGame::getCurrentPiece(Player &player, int &number)
{
    if (!onBoard[currentPos])
        return false;

    int p = board_[currentPos];

    if (p & 0x10) {
        player = PLAYER1;
        number = p - 0x10;
    } else if (p & 0x20) {
        player = PLAYER2;
        number = p - 0x20;
    } else {
        return false;
    }

    return true;
}

void MillGame::pos2rs(const int pos, int &r, int &s)
{
    //r = pos / N_SEATS;
    //s = pos % N_SEATS + 1;
    r = pos >> 3;
    s = (pos & 0x07) + 1;
}

int MillGame::rs2Pos(int r, int s)
{
    if (r < 1 || r > N_RINGS || s < 1 || s > N_SEATS)
        return 0;

    return r * N_SEATS + s - 1;
}

bool MillGame::place(int pos, int time_p, int8_t rs)
{
    // 如果局面为“结局”，返回false
    if (context.stage == GAME_OVER)
        return false;

    // 如果局面为“未开局”，则开局
    if (context.stage == GAME_NOTSTARTED)
        start();

    // 如非“落子”状态，返回false
    if (context.action != ACTION_PLACE)
        return false;

    // 如果落子位置在棋盘外、已有子点或禁点，返回false
    if (!onBoard[pos] || board_[pos])
        return false;

    // 格式转换
    int r = 0;
    int s = 0;
    pos2rs(pos, r, s);

    // 时间的临时变量
    int player_ms = -1;

    // 对于开局落子
    int piece = '\x00';
    int n = 0;

    if (context.stage == GAME_PLACING) {
        // 先手下
        if (context.turn == PLAYER1) {
            piece = '\x11' + currentRule.nTotalPiecesEachSide - context.nPiecesInHand_1;
            context.nPiecesInHand_1--;
            context.nPiecesOnBoard_1++;
        }
        // 后手下
        else {
            piece = '\x21' + currentRule.nTotalPiecesEachSide - context.nPiecesInHand_2;
            context.nPiecesInHand_2--;
            context.nPiecesOnBoard_2++;
        }

        board_[pos] = piece;

#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
        updateHash(pos);
#endif
        move_ = pos;

        if (rs) {
            player_ms = update(time_p);
            sprintf(cmdline, "(%1u,%1u) %02u:%02u",
                    r, s, player_ms / 60, player_ms % 60);
            cmdlist.emplace_back(string(cmdline));
            currentStep++;
        }

        currentPos = pos;

        n = addMills(currentPos);

        // 开局阶段未成三
        if (n == 0) {
            // 如果双方都无未放置的棋子
            if (context.nPiecesInHand_1 == 0 && context.nPiecesInHand_2 == 0) {
                // 进入中局阶段
                context.stage = GAME_MOVING;

                // 进入选子状态
                context.action = ACTION_CHOOSE;

                // 清除禁点
                cleanForbiddenPoints();

                // 设置轮到谁走
                if (currentRule.isDefenderMoveFirst) {
                    context.turn = PLAYER2;
                } else {
                    context.turn = PLAYER1;
                }

                // 再决胜负
                if (win()) {
                    goto out;
                }
            }
            // 如果双方还有子
            else {
                // 设置轮到谁走
                changeTurn();
            }
        }
        // 如果成三
        else {
            // 设置去子数目
            context.nPiecesNeedRemove = currentRule.allowRemoveMultiPieces ? n : 1;

            // 进入去子状态
            context.action = ACTION_CAPTURE;
        }

        goto out;
    }

    // 如果决出胜负
    if (win()) {
        goto out;
    }

    // 对于中局落子 (ontext.stage == GAME_MOVING)

    // 如果落子不合法
    if ((context.turn == PLAYER1 &&
         (context.nPiecesOnBoard_1 > currentRule.nPiecesAtLeast || !currentRule.allowFlyWhenRemainThreePieces)) ||
        (context.turn == PLAYER2 &&
         (context.nPiecesOnBoard_2 > currentRule.nPiecesAtLeast || !currentRule.allowFlyWhenRemainThreePieces))) {

        int i;
        for (i = 0; i < 4; i++) {
            if (pos == moveTable[currentPos][i])
                break;
        }

        // 不在着法表中
        if (i == 4) {
            return false;
        }
    }

    // 移子
    move_ = (currentPos << 8) + pos;
    if (rs) {
        player_ms = update(time_p);
        sprintf(cmdline, "(%1u,%1u)->(%1u,%1u) %02u:%02u", currentPos / N_SEATS, currentPos % N_SEATS + 1,
                r, s, player_ms / 60, player_ms % 60);
        cmdlist.emplace_back(string(cmdline));
        currentStep++;
        moveStep++;
    }

    board_[pos] = board_[currentPos];

#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
    updateHash(pos);
#endif

    board_[currentPos] = '\x00';

#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
    revertHash(currentPos);
#endif

    currentPos = pos;
    n = addMills(currentPos);

    // 中局阶段未成三
    if (n == 0) {
        // 进入选子状态
        context.action = ACTION_CHOOSE;

        // 设置轮到谁走
        changeTurn();

        // 如果决出胜负
        if (win()) {
            goto out;
        }
    }
    // 中局阶段成三
    else {
        // 设置去子数目
        context.nPiecesNeedRemove = currentRule.allowRemoveMultiPieces ? n : 1;

        // 进入去子状态
        context.action = ACTION_CAPTURE;
    }

out:
    if (rs) {
        setTips();
    }

    return true;
}

bool MillGame::_place(int r, int s, int time_p)
{
    // 转换为 pos
    int pos = rs2Pos(r, s);

    return place(pos, time_p, true);
}

bool MillGame::_capture(int r, int s, int time_p)
{
    // 转换为 pos
    int pos = rs2Pos(r, s);

    return capture(pos, time_p, 1);
}

bool MillGame::capture(int pos, int time_p, int8_t cp)
{
    // 如果局面为"未开局"或“结局”，返回false
    if (context.stage == GAME_NOTSTARTED || context.stage == GAME_OVER)
        return false;

    // 如非“去子”状态，返回false
    if (context.action != ACTION_CAPTURE)
        return false;

    // 如果去子完成，返回false
    if (context.nPiecesNeedRemove <= 0)
        return false;

    // 格式转换
    int r = 0;
    int s = 0;
    pos2rs(pos, r, s);

    // 时间的临时变量
    int player_ms = -1;

    // 对手
    char opponent = context.turn == PLAYER1 ? 0x20 : 0x10;

    // 判断去子是不是对手棋
    if (!(opponent & board_[pos]))
        return false;

    // 如果当前子是否处于“三连”之中，且对方还未全部处于“三连”之中
    if (!currentRule.allowRemoveMill &&
        isInMills(pos) && !isAllInMills(opponent)) {
        return false;
    }

    // 去子（设置禁点）
    if (currentRule.hasForbiddenPoint && context.stage == GAME_PLACING) {
#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
        revertHash(pos);
#endif
        board_[pos] = '\x0f';
#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
        updateHash(pos);
#endif
    } else { // 去子
#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
        revertHash(pos);
#endif
        board_[pos] = '\x00';
    }

    if (context.turn == PLAYER1)
        context.nPiecesOnBoard_2--;
    else if (context.turn == PLAYER2)
        context.nPiecesOnBoard_1--;

    move_ = -pos;

    if (cp) {
        player_ms = update(time_p);
        sprintf(cmdline, "-(%1u,%1u)  %02u:%02u", r, s, player_ms / 60, player_ms % 60);
        cmdlist.emplace_back(string(cmdline));
        currentStep++;
        moveStep = 0;
    }

    currentPos = 0;
    context.nPiecesNeedRemove--;
#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
    updateHash(pos);
#endif

    // 去子完成

    // 如果决出胜负
    if (win()) {
        goto out;
    }

    // 还有其余的子要去吗
    if (context.nPiecesNeedRemove > 0) {
        // 继续去子
        return true;
    }

    // 所有去子都完成了

    // 开局阶段
    if (context.stage == GAME_PLACING) {
        // 如果双方都无未放置的棋子
        if (context.nPiecesInHand_1 == 0 && context.nPiecesInHand_2 == 0) {

            // 进入中局阶段
            context.stage = GAME_MOVING;

            // 进入选子状态
            context.action = ACTION_CHOOSE;

            // 清除禁点
            cleanForbiddenPoints();

            // 设置轮到谁走
            if (currentRule.isDefenderMoveFirst) {
                context.turn = PLAYER2;
            } else {
                context.turn = PLAYER1;
            }

            // 再决胜负
            if (win()) {
                goto out;
            }
        }
        // 如果双方还有子
        else {
            // 进入落子状态
            context.action = ACTION_PLACE;

            // 设置轮到谁走
            changeTurn();

            // 如果决出胜负
            if (win()) {
                goto out;
            }
        }
    }
    // 中局阶段
    else {
        // 进入选子状态
        context.action = ACTION_CHOOSE;

        // 设置轮到谁走
        changeTurn();

        // 如果决出胜负
        if (win()) {
            goto out;
        }
    }

out:
    if (cp) {
        setTips();
    }

    return true;
}

bool MillGame::choose(int pos)
{
    // 如果局面不是"中局”，返回false
    if (context.stage != GAME_MOVING)
        return false;

    // 如非“选子”或“落子”状态，返回false
    if (context.action != ACTION_CHOOSE && context.action != ACTION_PLACE)
        return false;

    char t = context.turn == PLAYER1 ? 0x10 : 0x20;

    // 判断选子是否可选
    if (board_[pos] & t) {
        // 判断pos处的棋子是否被“闷”
        if (isSurrounded(pos)) {
            return false;
        }

        // 选子
        currentPos = pos;

        // 选子完成，进入落子状态
        context.action = ACTION_PLACE;

        return true;
    }

    return false;
}

bool MillGame::choose(int r, int s)
{
    return choose(rs2Pos(r, s));
}

bool MillGame::giveup(Player loser)
{
    if (context.stage == GAME_NOTSTARTED ||
        context.stage == GAME_OVER ||
        context.stage == GAME_NONE) {
        return false;
    }

    context.stage = GAME_OVER;

    if (loser == PLAYER1) {
        winner = PLAYER2;
        tips = "玩家1投子认负。";
        sprintf(cmdline, "Player1 give up!");
    } else if (loser == PLAYER2) { 
        winner = PLAYER1;
        tips = "玩家2投子认负。";
        sprintf(cmdline, "Player2 give up!");
    }

    cmdlist.emplace_back(string(cmdline));

    return true;
}

// 打算用个C++的命令行解析库的，简单的没必要，但中文编码有极小概率出问题
bool MillGame::command(const char *cmd)
{
    int r, t;
    step_t s;
    int r1, s1, r2, s2;
    int args = 0;
    int mm = 0, ss = 0;
    int tm = -1;

    // 设置规则
    if (sscanf(cmd, "r%1u s%3hd t%2u", &r, &s, &t) == 3) {
        if (r <= 0 || r > N_RULES) {
            return false;
        }

        return setContext(&MillGame::RULES[r - 1], s, t);
    }

    // 选子移动
    args = sscanf(cmd, "(%1u,%1u)->(%1u,%1u) %2u:%2u", &r1, &s1, &r2, &s2, &mm, &ss);

    if (args >= 4) {
        if (args == 7) {
            if (mm >= 0 && ss >= 0)
                tm = mm * 60 + ss;
        }

        if (choose(r1, s1)) {
            return _place(r2, s2, tm);
        }

        return false;
    }

    // 去子
    args = sscanf(cmd, "-(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
    if (args >= 2) {
        if (args == 5) {
            if (mm >= 0 && ss >= 0)
                tm = mm * 60 + ss;
        }
        return _capture(r1, s1, tm);
    }

    // 落子
    args = sscanf(cmd, "(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
    if (args >= 2) {
        if (args == 5) {
            if (mm >= 0 && ss >= 0)
                tm = mm * 60 + ss;
        }
        return _place(r1, s1, tm);
    }

    // 认输
    args = sscanf(cmd, "Players%1u give up!", &t);

    if (args == 1) {
        if (t == 1) {
            return giveup(PLAYER1);
        }
        if (t == 2) {
            return giveup(PLAYER2);
        }
    }

#ifdef THREEFOLD_REPETITION
    if (!strcmp(cmd, "Threefold Repetition. Draw!")) {
        return true;
    }

    if (!strcmp(cmd, "draw")) {
        context.stage = GAME_OVER;
        winner = DRAW;
        score_draw++;
        tips = "三次重复局面判和。";
        sprintf(cmdline, "Threefold Repetition. Draw!");
        cmdlist.emplace_back(string(cmdline));
#ifdef AOTO_RESTART_GAME
        start();    // TODO: 待转移到更合适的位置
#endif
        return true;
    }
#endif

    return false;
}

bool MillGame::command(int move)
{
    if (move < 0) {
        return capture(-move);
    }

    if (move & 0x1f00) {
        if (choose(move >> 8)) {
            return place(move & 0x00ff);
        }
    } else {
        return place(move & 0x00ff);
    }

    return false;
}

inline int MillGame::update(int time_p /*= -1*/)
{
    int ret = -1;
    time_t *player_ms = (context.turn == PLAYER1 ? &elapsedSeconds_1 : &elapsedSeconds_2);
    time_t playerNext_ms = (context.turn == PLAYER1 ? elapsedSeconds_2 : elapsedSeconds_1);

    // 根据局面调整计时器

    if (!(context.stage == GAME_PLACING || context.stage == GAME_MOVING)) {
        return -1;
    }

    currentTime = time(NULL);

    // 更新时间
    if (time_p >= *player_ms) {
        *player_ms = ret = time_p;
        startTime = currentTime - (elapsedSeconds_1 + elapsedSeconds_2);
    } else {
        *player_ms = ret = currentTime - startTime - playerNext_ms;
    }

    // 有限时要求则判断胜负
    if (currentRule.maxTimeLedToLose > 0) {
        win();
    }

    return ret;
}

// 是否分出胜负
bool MillGame::win()
{
    return win(false);
}

// 是否分出胜负
bool MillGame::win(bool forceDraw)
{
    if (context.stage == GAME_OVER) {
        return true;
    }

    if (context.stage == GAME_NOTSTARTED) {
        return false;
    }

    // 如果有时间限定
    if (currentRule.maxTimeLedToLose > 0) {
        context.stage = GAME_OVER;

        // 这里不能update更新时间，否则会形成循环嵌套
        // 如果玩家1超时
        if (elapsedSeconds_1 > currentRule.maxTimeLedToLose * 60) {
            elapsedSeconds_1 = currentRule.maxTimeLedToLose * 60;
            winner = PLAYER2;
            tips = "玩家1超时判负。";
            sprintf(cmdline, "Time over. Player2 win!");
        }
        // 如果玩家2超时
        else if (elapsedSeconds_2 > currentRule.maxTimeLedToLose * 60) {
            elapsedSeconds_2 = currentRule.maxTimeLedToLose * 60;
            winner = PLAYER1;
            tips = "玩家2超时判负。";
            sprintf(cmdline, "Time over. Player1 win!");
        }

        cmdlist.emplace_back(string(cmdline));
        return true;
    }

    // 如果有步数限定
    if (currentRule.maxStepsLedToDraw > 0 &&
        moveStep > currentRule.maxStepsLedToDraw) {
        winner = DRAW;
        context.stage = GAME_OVER;
        sprintf(cmdline, "Steps over. In draw!");
        cmdlist.emplace_back(string(cmdline));
        return true;
    }

    // 如果玩家1子数小于赛点，则玩家2获胜
    if (context.nPiecesOnBoard_1 + context.nPiecesInHand_1 < currentRule.nPiecesAtLeast) {
        winner = PLAYER2;
        context.stage = GAME_OVER;
        sprintf(cmdline, "Player2 win!");
        cmdlist.emplace_back(string(cmdline));
        return true;
    }

    // 如果玩家2子数小于赛点，则玩家1获胜
    if (context.nPiecesOnBoard_2 + context.nPiecesInHand_2 < currentRule.nPiecesAtLeast) {
        winner = PLAYER1;
        context.stage = GAME_OVER;
        sprintf(cmdline, "Player1 win!");
        cmdlist.emplace_back(string(cmdline));
#ifdef BOOK_LEARNING
        MillGameAi_ab::recordOpeningBookToHashMap();  // 暂时只对后手的失败记录到开局库
#endif /* BOOK_LEARNING */
        return true;
    }

    // 如果摆满了，根据规则判断胜负
    if (context.nPiecesOnBoard_1 + context.nPiecesOnBoard_2 >= N_SEATS * N_RINGS) {
        context.stage = GAME_OVER;

        if (currentRule.isStartingPlayerLoseWhenBoardFull) {
            winner = PLAYER2;
            sprintf(cmdline, "Player2 win!");
        } else {
            winner = DRAW;  
            sprintf(cmdline, "Full. In draw!");
        }

        cmdlist.emplace_back(string(cmdline));
        return true;
    }

    // 如果中局被“闷”
    if (context.stage == GAME_MOVING && context.action == ACTION_CHOOSE && isAllSurrounded(context.turn)) {
        // 规则要求被“闷”判负，则对手获胜
        context.stage = GAME_OVER;

        if (currentRule.isLoseWhenNoWay) {
            if (context.turn == PLAYER1) {
                tips = "玩家1无子可走被闷。";
                winner = PLAYER2;
                sprintf(cmdline, "Player1 no way to go. Player2 win!");
                cmdlist.emplace_back(string(cmdline));
            } else {
                tips = "玩家2无子可走被闷。";
                winner = PLAYER1;
                sprintf(cmdline, "Player2 no way to go. Player1 win!");
                cmdlist.emplace_back(string(cmdline));
#ifdef BOOK_LEARNING
                MillGameAi_ab::recordOpeningBookToHashMap();  // 暂时只对后手的失败记录到开局库
#endif /* BOOK_LEARNING */
            }

            return true;
        }

        // 否则让棋，由对手走
        changeTurn();
        return false;
    }

#ifdef THREEFOLD_REPETITION
    if (forceDraw)
    {
        tips = "重复三次局面和棋！";
        winner = DRAW;
        context.stage = GAME_OVER;
        sprintf(cmdline, "Threefold Repetition. Draw!");
        cmdlist.emplace_back(string(cmdline));
        return true;
    }
#endif

    return false;
}

int MillGame::isInMills(int pos, bool test)
{
    int n = 0;
    int pos1, pos2;
    int m = test? INT32_MAX : board_[pos] & '\x30';

    for (int i = 0; i < 3; i++) {
        pos1 = millTable[pos][i][0];
        pos2 = millTable[pos][i][1];
        if (m & board_[pos1] & board_[pos2])
            n++;
    }

    return n;
}

int MillGame::addMills(int pos)
{
    // 成三用一个64位整数了，规则如下
    // 0x   00     00     00    00    00    00    00    00
    //    unused unused piece1 pos1 piece2 pos2 piece3 pos3
    // piece1、piece2、piece3按照序号从小到大顺序排放
    uint64_t mill = 0;
    int n = 0;
    int p[3], min, temp;
    char m = board_[pos] & '\x30';

    for (int i = 0; i < 3; i++) {
        p[0] = pos;
        p[1] = millTable[pos][i][0];
        p[2] = millTable[pos][i][1];

        // 如果没有成三
        if (!(m & board_[p[1]] & board_[p[2]])) {
            continue;
        }

        // 如果成三

        // 排序
        for (int j = 0; j < 2; j++) {
            min = j;

            for (int k = j + 1; k < 3; k++) {
                if (p[min] > p[k])
                    min = k;
            }

            if (min == j) {
                continue;
            }

            temp = p[min];
            p[min] = p[j];
            p[j] = temp;
        }

        // 成三
        mill = (static_cast<uint64_t>(board_[p[0]]) << 40)
                + (static_cast<uint64_t>(p[0]) << 32)
                + (static_cast<uint64_t>(board_[p[1]]) << 24)
                + (static_cast<uint64_t>(p[1]) << 16)
                + (static_cast<uint64_t>(board_[p[2]]) << 8)
                +  static_cast<uint64_t>(p[2]);

        // 如果允许相同三连反复去子
        if (currentRule.allowRemovePiecesRepeatedly) {
            n++;
            continue;
        }

        // 如果不允许相同三连反复去子

        // 迭代器
        list<uint64_t>::iterator iter;

        // 遍历
        for (iter = context.millList.begin(); iter != context.millList.end(); iter++) {
            if (mill == *iter)
                break;
        }

        // 如果没找到历史项
        if (iter == context.millList.end()) {
            n++;
            context.millList.push_back(mill);
        }
    }

    return n;
}

bool MillGame::isAllInMills(char ch)
{
    for (int i = POS_BEGIN; i < POS_END; i++) {
        if (board_[i] & ch) {
            if (!isInMills(i)) {
                return false;
            }
        }
    }

    return true;
}

bool MillGame::isAllInMills(enum Player player)
{
    char ch = 0x00;

    if (player == PLAYER1)
        ch = 0x10;
    else if (player == PLAYER2)
        ch = 0x20;
    else
        return true;

    return isAllInMills(ch);
}

// 判断玩家的棋子周围有几个空位
int MillGame::getSurroundedEmptyPosCount(int pos, bool includeFobidden)
{
    int count = 0;

    if ((context.turn == PLAYER1 &&
        (context.nPiecesOnBoard_1 > currentRule.nPiecesAtLeast || !currentRule.allowFlyWhenRemainThreePieces)) ||
         (context.turn == PLAYER2 &&
        (context.nPiecesOnBoard_2 > currentRule.nPiecesAtLeast || !currentRule.allowFlyWhenRemainThreePieces))) {
        int d, movePos;
        for (d = 0; d < N_MOVE_DIRECTIONS; d++) {
            movePos = moveTable[pos][d];
            if (movePos) {
                if (board_[movePos] == 0x00 ||
                    (includeFobidden && board_[movePos] == 0x0F)) {
                    count++;
                }
            }
        }
    }

    return count;
}

// 计算玩家1和玩家2的棋子活动能力之差
int MillGame::getMobilityDiff(bool includeFobidden)
{
    int *board = context.board;
    int mobility1 = 0;
    int mobility2 = 0;
    int diff = 0;
    int n = 0;

    for (int i = POS_BEGIN; i < POS_END; i++) {
        n = getSurroundedEmptyPosCount(i, includeFobidden);

        if (board[i] & 0x10) {
            mobility1 += n;
        } else if (board[i] & 0x20) {
            mobility2 += n;
        }
    }

    diff = mobility1 - mobility2;

    return diff;
}

// 判断玩家的棋子是否被围
bool MillGame::isSurrounded(int pos)
{
    // 判断pos处的棋子是否被“闷”
    if ((context.turn == PLAYER1 &&
        (context.nPiecesOnBoard_1 > currentRule.nPiecesAtLeast || !currentRule.allowFlyWhenRemainThreePieces)) ||
        (context.turn == PLAYER2 &&
        (context.nPiecesOnBoard_2 > currentRule.nPiecesAtLeast || !currentRule.allowFlyWhenRemainThreePieces))) {
        int i, movePos;
        for (i = 0; i < 4; i++) {
            movePos = moveTable[pos][i];
            if (movePos && !board_[movePos])
                break;
        }
        // 被围住
        if (i == 4)
            return true;
    }
    // 没被围住
    return false;
}

bool MillGame::isAllSurrounded(char ch)
{
    // 如果摆满
    if (context.nPiecesOnBoard_1 + context.nPiecesOnBoard_2 >= N_SEATS * N_RINGS)
        return true;

    // 判断是否可以飞子
    if ((context.turn == PLAYER1 &&
        (context.nPiecesOnBoard_1 <= currentRule.nPiecesAtLeast && currentRule.allowFlyWhenRemainThreePieces)) ||
        (context.turn == PLAYER2 &&
        (context.nPiecesOnBoard_2 <= currentRule.nPiecesAtLeast && currentRule.allowFlyWhenRemainThreePieces))) {
        return false;
    }

    // 查询整个棋盘
    int movePos;
    for (int i = 1; i < N_SEATS * (N_RINGS + 1); i++) {
        if (!(ch & board_[i])) {
            continue;
        }

        for (int d = 0; d < N_MOVE_DIRECTIONS; d++) {
            movePos = moveTable[i][d];
            if (movePos && !board_[movePos])
                return false;
        }
    }

    return true;
}

// 判断玩家的棋子是否全部被围
bool MillGame::isAllSurrounded(enum Player ply)
{
    char t = '\x30';

    if (ply == PLAYER1)
        t &= '\x10';
    else if (ply == PLAYER2)
        t &= '\x20';

    return isAllSurrounded(t);
}

void MillGame::cleanForbiddenPoints()
{
    int pos = 0;

    for (int r = 1; r <= N_RINGS; r++) {
        for (int s = 0; s < N_SEATS; s++) {
            pos = r * N_SEATS + s;

            if (board_[pos] == '\x0f') {
#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))
                revertHash(pos);
#endif
                board_[pos] = '\x00';
            }
        }
    }
}

enum MillGame::Player MillGame::changeTurn()
{
    // 设置轮到谁走
    context.turn = (context.turn == PLAYER1) ? PLAYER2 : PLAYER1;

    return context.turn;
}

void MillGame::setTips()
{
    switch (context.stage) {
    case MillGame::GAME_NOTSTARTED:
        tips = "轮到玩家1落子，剩余" + std::to_string(context.nPiecesInHand_1) + "子" +
            "  比分 " + to_string(score_1) + ":" + to_string(score_2) + ", 和棋 " + to_string(score_draw);
        break;

    case MillGame::GAME_PLACING:
        if (context.action == ACTION_PLACE) {
            if (context.turn == PLAYER1) {
                tips = "轮到玩家1落子，剩余" + std::to_string(context.nPiecesInHand_1) + "子";
            } else if (context.turn == PLAYER2) {
                tips = "轮到玩家2落子，剩余" + std::to_string(context.nPiecesInHand_2) + "子";
            }
        } else if (context.action == ACTION_CAPTURE) {
            if (context.turn == PLAYER1) {
                tips = "成三！轮到玩家1去子，需去" + std::to_string(context.nPiecesNeedRemove) + "子";
            } else if (context.turn == PLAYER2) {
                tips = "成三！轮到玩家2去子，需去" + std::to_string(context.nPiecesNeedRemove) + "子";
            }
        }
        break;

    case MillGame::GAME_MOVING:
        if (context.action == ACTION_PLACE || context.action == ACTION_CHOOSE) {
            if (context.turn == PLAYER1) {
                tips = "轮到玩家1选子移动";
            } else if (context.turn == PLAYER2) {
                tips = "轮到玩家2选子移动";
            }
        } else if (context.action == ACTION_CAPTURE) {
            if (context.turn == PLAYER1) {
                tips = "成三！轮到玩家1去子，需去" + std::to_string(context.nPiecesNeedRemove) + "子";
            } else if (context.turn == PLAYER2) {
                tips = "成三！轮到玩家2去子，需去" + std::to_string(context.nPiecesNeedRemove) + "子";
            }
        }
        break;

    case MillGame::GAME_OVER:
        if (winner == DRAW) {
            score_draw++;
            tips = "双方平局！比分 " + to_string(score_1) + ":" + to_string(score_2) + ", 和棋 " + to_string(score_draw);            
        }            
        else if (winner == PLAYER1) {
            score_1++;
            if (tips.find("无子可走") != string::npos)
                tips += "玩家1获胜！比分 " + to_string(score_1) + ":" + to_string(score_2) + ", 和棋 " + to_string(score_draw);
            else
                tips = "玩家1获胜！比分 " + to_string(score_1) + ":" + to_string(score_2) + ", 和棋 " + to_string(score_draw);
        } else if (winner == PLAYER2) {
            score_2++;
            if (tips.find("无子可走") != string::npos)
                tips += "玩家2获胜！比分 " + to_string(score_1) + ":" + to_string(score_2) + ", 和棋 " + to_string(score_draw);
            else
                tips = "玩家2获胜！比分 " + to_string(score_1) + ":" + to_string(score_2) + ", 和棋 " + to_string(score_draw);
        }

#ifdef AOTO_RESTART_GAME
        start();    // TODO: 待转移到更合适的位置
#endif

        break;

    default:
        break;
    }
}

enum MillGame::Player MillGame::getWhosPiece(int r, int s)
{
    int pos = rs2Pos(r, s);

    if (board_[pos] & '\x10')
        return PLAYER1;

    if (board_[pos] & '\x20')
        return PLAYER2;

    return NOBODY;
}

void MillGame::getElapsedTime(time_t &p1_ms, time_t &p2_ms)
{
    update();

    p1_ms = elapsedSeconds_1;
    p2_ms = elapsedSeconds_2;
}

void MillGame::mirror(bool cmdChange /*= true*/)
{
    int ch;
    int r, s;
    int i;

    for (r = 1; r <= N_RINGS; r++) {
        for (s = 1; s < N_SEATS / 2; s++) {
            ch = board_[r * N_SEATS + s];
            board_[r * N_SEATS + s] = board_[(r + 1) * N_SEATS - s];
            //updateHash(i * N_SEATS + j);
            board_[(r + 1) * N_SEATS - s] = ch;
            //updateHash((i + 1) * N_SEATS - j);
        }
    }

    uint64_t llp[3] = {0};

    if (move_ < 0) {
        r = (-move_) / N_SEATS;
        s = (-move_) % N_SEATS;
        s = (N_SEATS - s) % N_SEATS;
        move_ = -(r * N_SEATS + s);
    } else {
        llp[0] = static_cast<uint64_t>(move_ >> 8);
        llp[1] = move_ & 0x00ff;

        for (i = 0; i < 2; i++) {
            r = static_cast<int>(llp[i]) / N_SEATS;
            s = static_cast<int>(llp[i]) % N_SEATS;
            s = (N_SEATS - s) % N_SEATS;
            llp[i] = (static_cast<uint64_t>(r) * N_SEATS + s);
        }

        move_ = static_cast<int16_t>(((llp[0] << 8) | llp[1]));
    }

    if (currentPos != 0) {
        r = currentPos / N_SEATS;
        s = currentPos % N_SEATS;
        s = (N_SEATS - s) % N_SEATS;
        currentPos = r * N_SEATS + s;
    }

    if (currentRule.allowRemovePiecesRepeatedly) {
        for (auto &mill : context.millList) {
            llp[0] = (mill & 0x000000ff00000000) >> 32;
            llp[1] = (mill & 0x0000000000ff0000) >> 16;
            llp[2] = (mill & 0x00000000000000ff);

            for (i = 0; i < 3; i++) {
                r = static_cast<int>(llp[i]) / N_SEATS;
                s = static_cast<int>(llp[i]) % N_SEATS;
                s = (N_SEATS - s) % N_SEATS;
                llp[i] = static_cast<uint64_t>(r * N_SEATS + s);
            }

            mill &= 0xffffff00ff00ff00;
            mill |= (llp[0] << 32) | (llp[1] << 16) | llp[2];
        }
    }

    // 命令行解析
    if (cmdChange) {
        int r1, s1, r2, s2;
        int args = 0;
        int mm = 0, ss = 0;

        args = sscanf(cmdline, "(%1u,%1u)->(%1u,%1u) %2u:%2u", &r1, &s1, &r2, &s2, &mm, &ss);
        if (args >= 4) {
            s1 = (N_SEATS - s1 + 1) % N_SEATS;
            s2 = (N_SEATS - s2 + 1) % N_SEATS;
            cmdline[3] = '1' + static_cast<char>(s1);
            cmdline[10] = '1' + static_cast<char>(s2);
        } else {
            args = sscanf(cmdline, "-(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
            if (args >= 2) {
                s1 = (N_SEATS - s1 + 1) % N_SEATS;
                cmdline[4] = '1' + static_cast<char>(s1);
            } else {
                args = sscanf(cmdline, "(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
                if (args >= 2) {
                    s1 = (N_SEATS - s1 + 1) % N_SEATS;
                    cmdline[3] = '1' + static_cast<char>(s1);
                }
            }
        }

        for (auto & iter : cmdlist) {
            args = sscanf(iter.c_str(), "(%1u,%1u)->(%1u,%1u) %2u:%2u", &r1, &s1, &r2, &s2, &mm, &ss);
            if (args >= 4) {
                s1 = (N_SEATS - s1 + 1) % N_SEATS;
                s2 = (N_SEATS - s2 + 1) % N_SEATS;
                iter[3] = '1' + static_cast<char>(s1);
                iter[10] = '1' + static_cast<char>(s2);
            } else {
                args = sscanf(iter.c_str(), "-(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
                if (args >= 2) {
                    s1 = (N_SEATS - s1 + 1) % N_SEATS;
                    iter[4] = '1' + static_cast<char>(s1);
                } else {
                    args = sscanf(iter.c_str(), "(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
                    if (args >= 2) {
                        s1 = (N_SEATS - s1 + 1) % N_SEATS;
                        iter[3] = '1' + static_cast<char>(s1);
                    }
                }
            }
        }
    }
}

void MillGame::turn(bool cmdChange /*= true*/)
{
    int ch;
    int r, s;
    int i;

    for (s = 0; s < N_SEATS; s++) {
        ch = board_[N_SEATS + s];
        board_[N_SEATS + s] = board_[N_SEATS * N_RINGS + s];
        //updateHash(N_SEATS + s);
        board_[N_SEATS * N_RINGS + s] = ch;
        //updateHash(N_SEATS * N_RINGS + s);
    }

    uint64_t llp[3] = {0};

    if (move_ < 0) {
        r = (-move_) / N_SEATS;
        s = (-move_) % N_SEATS;

        if (r == 1)
            r = N_RINGS;
        else if (r == N_RINGS)
            r = 1;

        move_ = -(r * N_SEATS + s);
    } else {
        llp[0] = static_cast<uint64_t>(move_ >> 8);
        llp[1] = move_ & 0x00ff;

        for (i = 0; i < 2; i++) {
            r = static_cast<int>(llp[i]) / N_SEATS;
            s = static_cast<int>(llp[i]) % N_SEATS;

            if (r == 1)
                r = N_RINGS;
            else if (r == N_RINGS)
                r = 1;

            llp[i] = static_cast<uint64_t>(r * N_SEATS + s);
        }

        move_ = static_cast<int16_t>(((llp[0] << 8) | llp[1]));
    }

    if (currentPos != 0) {
        r = currentPos / N_SEATS;
        s = currentPos % N_SEATS;

        if (r == 1)
            r = N_RINGS;
        else if (r == N_RINGS)
            r = 1;

        currentPos = r * N_SEATS + s;
    }

    if (currentRule.allowRemovePiecesRepeatedly) {        
        for (auto &mill : context.millList) {
            llp[0] = (mill & 0x000000ff00000000) >> 32;
            llp[1] = (mill & 0x0000000000ff0000) >> 16;
            llp[2] = (mill & 0x00000000000000ff);

            for (i = 0; i < 3; i++) {
                r = static_cast<int>(llp[i]) / N_SEATS;
                s = static_cast<int>(llp[i]) % N_SEATS;

                if (r == 1)
                    r = N_RINGS;
                else if (r == N_RINGS)
                    r = 1;

                llp[i] = static_cast<uint64_t>(r * N_SEATS + s);
            }

            mill &= 0xffffff00ff00ff00;
            mill |= (llp[0] << 32) | (llp[1] << 16) | llp[2];
        }
    }

    // 命令行解析
    if (cmdChange) {
        int r1, s1, r2, s2;
        int args = 0;
        int mm = 0, ss = 0;

        args = sscanf(cmdline, "(%1u,%1u)->(%1u,%1u) %2u:%2u",
                        &r1, &s1, &r2, &s2, &mm, &ss);

        if (args >= 4) {
            if (r1 == 1)
                r1 = N_RINGS;
            else if (r1 == N_RINGS)
                r1 = 1;

            if (r2 == 1)
                r2 = N_RINGS;
            else if (r2 == N_RINGS)
                r2 = 1;

            cmdline[1] = '0' + static_cast<char>(r1);
            cmdline[8] = '0' + static_cast<char>(r2);
        } else {
            args = sscanf(cmdline, "-(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
            if (args >= 2) {
                if (r1 == 1)
                    r1 = N_RINGS;
                else if (r1 == N_RINGS)
                    r1 = 1;
                cmdline[2] = '0' + static_cast<char>(r1);
            } else {
                args = sscanf(cmdline, "(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
                if (args >= 2) {
                    if (r1 == 1)
                        r1 = N_RINGS;
                    else if (r1 == N_RINGS)
                        r1 = 1;
                    cmdline[1] = '0' + static_cast<char>(r1);
                }
            }
        }

        for (auto & iter : cmdlist) {
            args = sscanf(iter.c_str(),
                            "(%1u,%1u)->(%1u,%1u) %2u:%2u",
                            &r1, &s1, &r2, &s2, &mm, &ss);

            if (args >= 4) {
                if (r1 == 1)
                    r1 = N_RINGS;
                else if (r1 == N_RINGS)
                    r1 = 1;

                if (r2 == 1)
                    r2 = N_RINGS;
                else if (r2 == N_RINGS)
                    r2 = 1;

                iter[1] = '0' + static_cast<char>(r1);
                iter[8] = '0' + static_cast<char>(r2);
            } else {
                args = sscanf(iter.c_str(), "-(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
                if (args >= 2) {
                    if (r1 == 1)
                        r1 = N_RINGS;
                    else if (r1 == N_RINGS)
                        r1 = 1;

                    iter[2] = '0' + static_cast<char>(r1);
                } else {
                    args = sscanf(iter.c_str(), "(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
                    if (args >= 2) {
                        if (r1 == 1)
                            r1 = N_RINGS;
                        else if (r1 == N_RINGS)
                            r1 = 1;

                        iter[1] = '0' + static_cast<char>(r1);
                    }
                }
            }
        }
    }
}

void MillGame::rotate(int degrees, bool cmdChange /*= true*/)
{
    // 将degrees转化为0~359之间的数
    degrees = degrees % 360;

    if (degrees < 0)
        degrees += 360;

    if (degrees == 0 || degrees % 90)
        return;

    degrees /= 45;

    int ch1, ch2;
    int r, s;
    int i;

    if (degrees == 2) {
        for (r = 1; r <= N_RINGS; r++) {
            ch1 = board_[r * N_SEATS];
            ch2 = board_[r * N_SEATS + 1];

            for (s = 0; s < N_SEATS - 2; s++) {
                board_[r * N_SEATS + s] = board_[r * N_SEATS + s + 2];
            }

            board_[r * N_SEATS + 6] = ch1;
            //updateHash(i * N_SEATS + 6);
            board_[r * N_SEATS + 7] = ch2;
            //updateHash(i * N_SEATS + 7);
        }
    } else if (degrees == 6) {
        for (r = 1; r <= N_RINGS; r++) {
            ch1 = board_[r * N_SEATS + 7];
            ch2 = board_[r * N_SEATS + 6];

            for (s = N_SEATS - 1; s >= 2; s--) {
                board_[r * N_SEATS + s] = board_[r * N_SEATS + s - 2];
                //updateHash(i * N_SEATS + j);
            }

            board_[r * N_SEATS + 1] = ch1;
            //updateHash(i * N_SEATS + 1);
            board_[r * N_SEATS] = ch2;
            //updateHash(i * N_SEATS);
        }
    } else if (degrees == 4) {
        for (r = 1; r <= N_RINGS; r++) {
            for (s = 0; s < N_SEATS / 2; s++) {
                ch1 = board_[r * N_SEATS + s];
                board_[r * N_SEATS + s] = board_[r * N_SEATS + s + 4];
                //updateHash(i * N_SEATS + j);
                board_[r * N_SEATS + s + 4] = ch1;
                //updateHash(i * N_SEATS + j + 4);
            }
        }
    } else {
        return;
    }

    uint64_t llp[3] = { 0 };

    if (move_ < 0) {
        r = (-move_) / N_SEATS;
        s = (-move_) % N_SEATS;
        s = (s + N_SEATS - degrees) % N_SEATS;
        move_ = -(r * N_SEATS + s);
    } else {
        llp[0] = static_cast<uint64_t>(move_ >> 8);
        llp[1] = move_ & 0x00ff;
        r = static_cast<int>(llp[0]) / N_SEATS;
        s = static_cast<int>(llp[0]) % N_SEATS;
        s = (s + N_SEATS - degrees) % N_SEATS;
        llp[0] = static_cast<uint64_t>(r * N_SEATS + s);
        r = static_cast<int>(llp[1]) / N_SEATS;
        s = static_cast<int>(llp[1]) % N_SEATS;
        s = (s + N_SEATS - degrees) % N_SEATS;
        llp[1] = static_cast<uint64_t>(r * N_SEATS + s);
        move_ = static_cast<int16_t>(((llp[0] << 8) | llp[1]));
    }

    if (currentPos != 0) {
        r = currentPos / N_SEATS;
        s = currentPos % N_SEATS;
        s = (s + N_SEATS - degrees) % N_SEATS;
        currentPos = r * N_SEATS + s;
    }

    if (currentRule.allowRemovePiecesRepeatedly) {
        for (auto &mill : context.millList){
            llp[0] = (mill & 0x000000ff00000000) >> 32;
            llp[1] = (mill & 0x0000000000ff0000) >> 16;
            llp[2] = (mill & 0x00000000000000ff);

            for (i = 0; i < 3; i++) {
                r = static_cast<int>(llp[i]) / N_SEATS;
                s = static_cast<int>(llp[i]) % N_SEATS;
                s = (s + N_SEATS - degrees) % N_SEATS;
                llp[i] = static_cast<uint64_t>(r * N_SEATS + s);
            }

            mill &= 0xffffff00ff00ff00;
            mill |= (llp[0] << 32) | (llp[1] << 16) | llp[2];
        }
    }

    // 命令行解析
    if (cmdChange) {
        int r1, s1, r2, s2;
        int args = 0;
        int mm = 0, ss = 0;

        args = sscanf(cmdline, "(%1u,%1u)->(%1u,%1u) %2u:%2u", &r1, &s1, &r2, &s2, &mm, &ss);
        if (args >= 4) {
            s1 = (s1 - 1 + N_SEATS - degrees) % N_SEATS;
            s2 = (s2 - 1 + N_SEATS - degrees) % N_SEATS;
            cmdline[3] = '1' + static_cast<char>(s1);
            cmdline[10] = '1' + static_cast<char>(s2);
        } else {
            args = sscanf(cmdline, "-(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);

            if (args >= 2) {
                s1 = (s1 - 1 + N_SEATS - degrees) % N_SEATS;
                cmdline[4] = '1' + static_cast<char>(s1);
            } else {
                args = sscanf(cmdline, "(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);

                if (args >= 2) {
                    s1 = (s1 - 1 + N_SEATS - degrees) % N_SEATS;
                    cmdline[3] = '1' + static_cast<char>(s1);
                }
            }
        }

        for (auto &iter : cmdlist) {
            args = sscanf(iter.c_str(), "(%1u,%1u)->(%1u,%1u) %2u:%2u", &r1, &s1, &r2, &s2, &mm, &ss);

            if (args >= 4) {
                s1 = (s1 - 1 + N_SEATS - degrees) % N_SEATS;
                s2 = (s2 - 1 + N_SEATS - degrees) % N_SEATS;
                iter[3] = '1' + static_cast<char>(s1);
                iter[10] = '1' + static_cast<char>(s2);
            } else {
                args = sscanf(iter.c_str(), "-(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);

                if (args >= 2) {
                    s1 = (s1 - 1 + N_SEATS - degrees) % N_SEATS;
                    iter[4] = '1' + static_cast<char>(s1);
                } else {
                    args = sscanf(iter.c_str(), "(%1u,%1u) %2u:%2u", &r1, &s1, &mm, &ss);
                    if (args >= 2) {
                        s1 = (s1 - 1 + N_SEATS - degrees) % N_SEATS;
                        iter[3] = '1' + static_cast<char>(s1);
                    }
                }
            }
        }
    }
}

#if ((defined HASH_MAP_ENABLE) || (defined BOOK_LEARNING) || (defined THREEFOLD_REPETITION))

#if 0
/*
 * 原始版本 hash 各数据位详解（名为 hash 但实际并无冲突，是算法用到的棋局数据的完全表示）[因效率问题废弃]
 * 56-63位：空白不用，全为0
 * 55位：轮流标识，0为先手，1为后手
 * 54位：动作标识，落子（选子移动）为0，1为去子
 * 6-53位（共48位）：从棋盘第一个位置点到最后一个位置点的棋子，每个点用2个二进制位表示，共24个位置点，即48位。
 *        0b00表示空白，0b01表示先手棋子，0b10表示后手棋子，0b11表示禁点
 * 4-5位（共2位）：待去子数，最大为3，用2个二进制位表示即可
 * 0-3位：player1的手棋数，不需要player2的（可计算出）
 */
#endif

/*
 * 新版本 hash 各数据位详解
 * 8-63位 (共56位): zobrist 值
 * TODO: 低8位浪费了哈希空间，待后续优化
 * 4-7位 (共4位)：player1的手棋数，不需要player2的（可计算出）, 走子阶段置为全1即为全15
 * 2-3位（共2位）：待去子数，最大为3，用2个二进制位表示即可
 * 1位: 动作标识，落子（选子移动）为0，1为去子
 * 0位：轮流标识，0为先手，1为后手
 */

void MillGame::constructHash()
{
    context.hash = 0;

#include "zobrist.h"
    memcpy(context.zobrist, zobrist0, sizeof(hash_t) * MillGame::N_POINTS * MillGame::POINT_TYPE_COUNT);

#if 0
    // 预留末8位后续填充局面特征标志
    for (int p = 0; p < N_POINTS; p++) {
        //qDebug("{\n");
        for (int t = MillGame::POINT_TYPE_EMPTY; t <= MillGame::POINT_TYPE_FORBIDDEN; t++) {
            context.zobrist[p][t] = rand56();
            //qDebug("%llX, ", context.zobrist[p][t]);
        }
        //qDebug("},\n");
    }      
#endif
}

MillGame::hash_t MillGame::getHash()
{
    // TODO: 每次获取哈希值时更新 hash 值低8位，放在此处调用不优雅
    updateHashMisc();

    return context.hash;
}

MillGame::hash_t MillGame::updateHash(int pos)
{
    // PieceType is board_[pos]

    // 0b00 表示空白，0b01 = 1 表示先手棋子，0b10 = 2 表示后手棋子，0b11 = 3 表示禁点
    int pointType = (board_[pos] & 0x30) >> 4;

    // 清除或者放置棋子
    context.hash ^= context.zobrist[pos][pointType];

    return context.hash;
}

MillGame::hash_t MillGame::revertHash(int pos)
{
    return updateHash(pos);
}

MillGame::hash_t MillGame::updateHashMisc()
{
    // 清除标记位
    context.hash &= static_cast<hash_t>(~0xFF);

    // 置位

    if (context.turn == PLAYER2) {
        context.hash |= 1U;
    }

    if (context.action == ACTION_CAPTURE) {
        context.hash |= 1U << 1;
    }

    context.hash |= static_cast<MillGame::hash_t>(context.nPiecesNeedRemove) << 2;
    context.hash |= static_cast<MillGame::hash_t>(context.nPiecesInHand_1) << 4;     // TODO: 或许换 game.stage 也可以？

    return context.hash;
}
#endif /* HASH_MAP_ENABLE etc. */
