// This file is part of Sanmill.
// Copyright (C) 2019-2021 The Sanmill developers (see AUTHORS file)
//
// Sanmill is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Sanmill is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

part of '../mill.dart';

class _Mills {
  const _Mills._();

  static List<List<int>> get adjacentSquaresInit {
    return DB().rules.hasDiagonalLines
        ? _adjacentSquaresDiagonal
        : _adjacentSquares;
  }

  static List<List<List<int>>> get millTableInit {
    return DB().rules.hasDiagonalLines ? _millTableDiagonal : _millTable;
  }

  // Note: Not follow order of MoveDirection array
  static const _adjacentSquares = [
    /*  0 */ [0, 0, 0, 0],
    /*  1 */ [0, 0, 0, 0],
    /*  2 */ [0, 0, 0, 0],
    /*  3 */ [0, 0, 0, 0],
    /*  4 */ [0, 0, 0, 0],
    /*  5 */ [0, 0, 0, 0],
    /*  6 */ [0, 0, 0, 0],
    /*  7 */ [0, 0, 0, 0],
    /*  8 */ [16, 9, 15, 0],
    /*  9 */ [10, 8, 0, 0],
    /* 10 */ [18, 11, 9, 0],
    /* 11 */ [12, 10, 0, 0],
    /* 12 */ [20, 13, 11, 0],
    /* 13 */ [14, 12, 0, 0],
    /* 14 */ [22, 15, 13, 0],
    /* 15 */ [8, 14, 0, 0],
    /* 16 */ [8, 24, 17, 23],
    /* 17 */ [18, 16, 0, 0],
    /* 18 */ [10, 26, 19, 17],
    /* 19 */ [20, 18, 0, 0],
    /* 20 */ [12, 28, 21, 19],
    /* 21 */ [22, 20, 0, 0],
    /* 22 */ [14, 30, 23, 21],
    /* 23 */ [16, 22, 0, 0],
    /* 24 */ [16, 25, 31, 0],
    /* 25 */ [26, 24, 0, 0],
    /* 26 */ [18, 27, 25, 0],
    /* 27 */ [28, 26, 0, 0],
    /* 28 */ [20, 29, 27, 0],
    /* 29 */ [30, 28, 0, 0],
    /* 30 */ [22, 31, 29, 0],
    /* 31 */ [24, 30, 0, 0],
    /* 32 */ [0, 0, 0, 0],
    /* 33 */ [0, 0, 0, 0],
    /* 34 */ [0, 0, 0, 0],
    /* 35 */ [0, 0, 0, 0],
    /* 36 */ [0, 0, 0, 0],
    /* 37 */ [0, 0, 0, 0],
    /* 38 */ [0, 0, 0, 0],
    /* 39 */ [0, 0, 0, 0],
  ];

  static const _adjacentSquaresDiagonal = [
    /*  0 */ [0, 0, 0, 0],
    /*  1 */ [0, 0, 0, 0],
    /*  2 */ [0, 0, 0, 0],
    /*  3 */ [0, 0, 0, 0],
    /*  4 */ [0, 0, 0, 0],
    /*  5 */ [0, 0, 0, 0],
    /*  6 */ [0, 0, 0, 0],
    /*  7 */ [0, 0, 0, 0],
    /*  8 */ [9, 15, 16, 0],
    /*  9 */ [17, 8, 10, 0],
    /* 10 */ [9, 11, 18, 0],
    /* 11 */ [19, 10, 12, 0],
    /* 12 */ [11, 13, 20, 0],
    /* 13 */ [21, 12, 14, 0],
    /* 14 */ [13, 15, 22, 0],
    /* 15 */ [23, 8, 14, 0],
    /* 16 */ [17, 23, 8, 24],
    /* 17 */ [9, 25, 16, 18],
    /* 18 */ [17, 19, 10, 26],
    /* 19 */ [11, 27, 18, 20],
    /* 20 */ [19, 21, 12, 28],
    /* 21 */ [13, 29, 20, 22],
    /* 22 */ [21, 23, 14, 30],
    /* 23 */ [15, 31, 16, 22],
    /* 24 */ [25, 31, 16, 0],
    /* 25 */ [17, 24, 26, 0],
    /* 26 */ [25, 27, 18, 0],
    /* 27 */ [19, 26, 28, 0],
    /* 28 */ [27, 29, 20, 0],
    /* 29 */ [21, 28, 30, 0],
    /* 30 */ [29, 31, 22, 0],
    /* 31 */ [23, 24, 30, 0],
    /* 32 */ [0, 0, 0, 0],
    /* 33 */ [0, 0, 0, 0],
    /* 34 */ [0, 0, 0, 0],
    /* 35 */ [0, 0, 0, 0],
    /* 36 */ [0, 0, 0, 0],
    /* 37 */ [0, 0, 0, 0],
    /* 38 */ [0, 0, 0, 0],
    /* 39 */ [0, 0, 0, 0],
  ];

  static const _millTable = [
    /* 0 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 1 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 2 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 3 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 4 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 5 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 6 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 7 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 8 */ [
      [16, 24],
      [9, 15],
      [0, 0]
    ],
    /* 9 */ [
      [0, 0],
      [15, 8],
      [10, 11]
    ],
    /* 10 */ [
      [18, 26],
      [11, 9],
      [0, 0]
    ],
    /* 11 */ [
      [0, 0],
      [9, 10],
      [12, 13]
    ],
    /* 12 */ [
      [20, 28],
      [13, 11],
      [0, 0]
    ],
    /* 13 */ [
      [0, 0],
      [11, 12],
      [14, 15]
    ],
    /* 14 */ [
      [22, 30],
      [15, 13],
      [0, 0]
    ],
    /* 15 */ [
      [0, 0],
      [13, 14],
      [8, 9]
    ],
    /* 16 */ [
      [8, 24],
      [17, 23],
      [0, 0]
    ],
    /* 17 */ [
      [0, 0],
      [23, 16],
      [18, 19]
    ],
    /* 18 */ [
      [10, 26],
      [19, 17],
      [0, 0]
    ],
    /* 19 */ [
      [0, 0],
      [17, 18],
      [20, 21]
    ],
    /* 20 */ [
      [12, 28],
      [21, 19],
      [0, 0]
    ],
    /* 21 */ [
      [0, 0],
      [19, 20],
      [22, 23]
    ],
    /* 22 */ [
      [14, 30],
      [23, 21],
      [0, 0]
    ],
    /* 23 */ [
      [0, 0],
      [21, 22],
      [16, 17]
    ],
    /* 24 */ [
      [8, 16],
      [25, 31],
      [0, 0]
    ],
    /* 25 */ [
      [0, 0],
      [31, 24],
      [26, 27]
    ],
    /* 26 */ [
      [10, 18],
      [27, 25],
      [0, 0]
    ],
    /* 27 */ [
      [0, 0],
      [25, 26],
      [28, 29]
    ],
    /* 28 */ [
      [12, 20],
      [29, 27],
      [0, 0]
    ],
    /* 29 */ [
      [0, 0],
      [27, 28],
      [30, 31]
    ],
    /* 30 */ [
      [14, 22],
      [31, 29],
      [0, 0]
    ],
    /* 31 */ [
      [0, 0],
      [29, 30],
      [24, 25]
    ],
    /* 32 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 33 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 34 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 35 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 36 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 37 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 38 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 39 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ]
  ];

  static const _millTableDiagonal = [
    /*  0 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /*  1 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /*  2 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /*  3 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /*  4 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /*  5 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /*  6 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /*  7 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /*  8 */ [
      [16, 24],
      [9, 15],
      [0, 0]
    ],
    /*  9 */ [
      [17, 25],
      [15, 8],
      [10, 11]
    ],
    /* 10 */ [
      [18, 26],
      [11, 9],
      [0, 0]
    ],
    /* 11 */ [
      [19, 27],
      [9, 10],
      [12, 13]
    ],
    /* 12 */ [
      [20, 28],
      [13, 11],
      [0, 0]
    ],
    /* 13 */ [
      [21, 29],
      [11, 12],
      [14, 15]
    ],
    /* 14 */ [
      [22, 30],
      [15, 13],
      [0, 0]
    ],
    /* 15 */ [
      [23, 31],
      [13, 14],
      [8, 9]
    ],
    /* 16 */ [
      [8, 24],
      [17, 23],
      [0, 0]
    ],
    /* 17 */ [
      [9, 25],
      [23, 16],
      [18, 19]
    ],
    /* 18 */ [
      [10, 26],
      [19, 17],
      [0, 0]
    ],
    /* 19 */ [
      [11, 27],
      [17, 18],
      [20, 21]
    ],
    /* 20 */ [
      [12, 28],
      [21, 19],
      [0, 0]
    ],
    /* 21 */ [
      [13, 29],
      [19, 20],
      [22, 23]
    ],
    /* 22 */ [
      [14, 30],
      [23, 21],
      [0, 0]
    ],
    /* 23 */ [
      [15, 31],
      [21, 22],
      [16, 17]
    ],
    /* 24 */ [
      [8, 16],
      [25, 31],
      [0, 0]
    ],
    /* 25 */ [
      [9, 17],
      [31, 24],
      [26, 27]
    ],
    /* 26 */ [
      [10, 18],
      [27, 25],
      [0, 0]
    ],
    /* 27 */ [
      [11, 19],
      [25, 26],
      [28, 29]
    ],
    /* 28 */ [
      [12, 20],
      [29, 27],
      [0, 0]
    ],
    /* 29 */ [
      [13, 21],
      [27, 28],
      [30, 31]
    ],
    /* 30 */ [
      [14, 22],
      [31, 29],
      [0, 0]
    ],
    /* 31 */ [
      [15, 23],
      [29, 30],
      [24, 25]
    ],
    /* 32 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 33 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 34 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 35 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 36 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 37 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 38 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ],
    /* 39 */ [
      [0, 0],
      [0, 0],
      [0, 0]
    ]
  ];
}
