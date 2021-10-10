/*
  This file is part of Sanmill.
  Copyright (C) 2019-2021 The Sanmill developers (see AUTHORS file)

  Sanmill is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Sanmill is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

class Zobrist {
  const Zobrist._();

  static const int KEY_MISC_BIT = 2;
  static List<List<int>> psq = [
    [
      0x4E421A,
      0x3962FF,
      0x6DB6EE,
      0x219AE1,
      0x1F3DE2,
      0xD9AACB,
      0xD51733,
      0xD3F9EA,
      0xF5A7BB,
      0xDC4109,
      0xEE4319,
      0x7CDA7A,
      0xFD7B4D,
      0x4138BE,
      0xCCBB2D,
      0xDA6097,
      0x06D827,
      0xCBC16C,
      0x46F125,
      0xE29F22,
      0xCAAB94,
      0x5B02DB,
      0x877CD6,
      0x35E438,
      0x49FDAE,
      0xE68314,
      0xBE1664,
      0x1F49D3,
      0x50F5B1,
      0x149AAF,
      0xF509B9,
      0x47AEB5,
      0x18E993,
      0x76BB4F,
      0xFE1739,
      0xF87B87,
      0x0A8CD2,
      0x630C6B,
      0x88F5B4,
      0x0A583E,
    ],
    [
      0xA0128E,
      0x6F2251,
      0x51E99D,
      0x6D35BF,
      0x66D6D9,
      0x87D366,
      0x75A57A,
      0x534FC4,
      0x1FE34B,
      0xAD6FB0,
      0xE5679D,
      0xF88AFF,
      0x0462DA,
      0x4BDE96,
      0xF28912,
      0x10537E,
      0x26D8EA,
      0x37E6E7,
      0x0871D9,
      0xCD5F4F,
      0xF4AFA1,
      0x44A51B,
      0x772656,
      0x8B7965,
      0xD8F17D,
      0x80F3D7,
      0x6B6206,
      0x19B8BB,
      0xFBC229,
      0x0FCAB4,
      0xFD7374,
      0xA647B9,
      0x296A8D,
      0xA3D742,
      0x624D6D,
      0x459FD4,
      0xCE8C26,
      0x965448,
      0x410171,
      0x1EDD7A,
    ],
    [
      0x1FCF95,
      0xA5634E,
      0x21976A,
      0x32902D,
      0x55A27C,
      0x49EC5F,
      0x0176A1,
      0xCAAAEF,
      0x145886,
      0xB4C808,
      0x0153EE,
      0x7D78DF,
      0xE9C3C5,
      0x66B7A6,
      0x3CD930,
      0xDBBA23,
      0xF19841,
      0x6BEFDF,
      0xB979FE,
      0xBA4D06,
      0x96AECF,
      0x33B96E,
      0x76A99C,
      0x1B8762,
      0x747B20,
      0x0DEC24,
      0xA4E632,
      0xBA2442,
      0x59C91B,
      0x41482D,
      0xF2CD39,
      0x30E9C1,
      0x6B156D,
      0xC7F191,
      0x012D36,
      0xC66B36,
      0x631560,
      0xA891FC,
      0xF6C8AC,
      0xD80B94,
    ],
    [
      0xF641E9,
      0xF164BF,
      0x2DBE4C,
      0xE2A40C,
      0x53FA06,
      0x4F3117,
      0x0ACA70,
      0x2C72F5,
      0xC81047,
      0x4B76AE,
      0xEB55C8,
      0x0DB6EF,
      0x7F57AB,
      0x22D060,
      0x390554,
      0xDE9A43,
      0x6583AF,
      0x41D141,
      0x9CBF92,
      0x7E528F,
      0x2BEFA1,
      0x5C5FDC,
      0x4DDAFA,
      0x7C98A1,
      0x65A13B,
      0x2953BF,
      0x8769A8,
      0xE6DCA1,
      0xD01A6E,
      0xBCD935,
      0x175659,
      0xAD5A73,
      0xB04E7D,
      0x815F53,
      0x12469A,
      0xB2F25C,
      0x564E4B,
      0xD19437,
      0xA4F63C,
      0x7169E5,
    ]
  ];

  static int side = 0x201906;
}
