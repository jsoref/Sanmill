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

import 'package:flutter/animation.dart';
import 'package:sanmill/common/config.dart';
import 'package:sanmill/engine/engine.dart';
import 'package:sanmill/mill/types.dart';

import 'mill.dart';
import 'position.dart';

enum PlayerType { human, AI }
Map<String, bool> isAi = {Color.black: false, Color.white: true};

class Game {
  static Game _instance;

  Position _position;
  int _focusIndex, _blurIndex;

  String sideToMove = Color.black;

  bool isColorInverted = false;

  Map<String, bool> isSearching = {Color.black: false, Color.white: false};

  EngineType engineType;

  bool aiIsSearching() {
    return isSearching[Color.black] == true || isSearching[Color.white] == true;
  }

  void setWhoIsAi(EngineType type) {
    engineType = type;

    switch (type) {
      case EngineType.humanVsAi:
      case EngineType.testViaLAN:
        if (Config.aiMovesFirst) {
          isAi[Color.black] = true;
          isAi[Color.white] = false;
        } else if (!Config.aiMovesFirst) {
          isAi[Color.black] = false;
          isAi[Color.white] = true;
        }
        break;
      case EngineType.humanVsHuman:
      case EngineType.humanVsLAN:
        isAi[Color.black] = false;
        isAi[Color.white] = false;
        break;
      case EngineType.aiVsAi:
        isAi[Color.black] = true;
        isAi[Color.white] = true;
        break;
      case EngineType.humanVsCloud:
        break;
      default:
        break;
    }
  }

  void start() {
    position.reset();

    setWhoIsAi(engineType);
  }

  bool hasAnimation;

  int animationDurationTime;

  static bool hasSound = true;

  bool resignIfMostLose = false;

  bool isAutoChangeFirstMove = false;

  bool isAiFirstMove = false;

  int ruleIndex;

  String tips;

  List<String> moveHistory = [""];

  String getTips() => tips;

  bool isAiToMove() {
    return isAi[sideToMove];
  }

  static get shared {
    _instance ??= Game();
    return _instance;
  }

  init() {
    _position = Position();
    _focusIndex = _blurIndex = Move.invalidMove;
  }

  newGame() {
    Game.shared.position.phase = Phase.ready;
    Game.shared.start();
    Game.shared.position.init();
    _focusIndex = _blurIndex = Move.invalidMove;
    moveHistory = [""];
    // TODO
    sideToMove = Color.black;
  }

  select(int pos) {
    _focusIndex = pos;
    _blurIndex = Move.invalidMove;
    //Audios.playTone('click.mp3');
  }

  bool move(int from, int to) {
    //
    position.move(from, to);

    _blurIndex = from;
    _focusIndex = to;

    return true;
  }

  bool regret({moves = 2}) {
    //
    // Can regret only our turn
    // TODO
    if (_position.side != Color.white) {
      //Audios.playTone('invalid.mp3');
      return false;
    }

    var regretted = false;

    /// Regret 2 step

    for (var i = 0; i < moves; i++) {
      //
      if (!_position.regret()) break;

      final lastMove = _position.lastMove;

      if (lastMove != null) {
        //
        _blurIndex = lastMove.from;
        _focusIndex = lastMove.to;
        //
      } else {
        //
        _blurIndex = _focusIndex = Move.invalidMove;
      }

      regretted = true;
    }

    if (regretted) {
      //Audios.playTone('regret.mp3');
      return true;
    }

    //Audios.playTone('invalid.mp3');
    return false;
  }

  clear() {
    _blurIndex = _focusIndex = Move.invalidMove;
  }

  get position => _position;

  get focusIndex => _focusIndex;

  get blurIndex => _blurIndex;

  bool doMove(String move) {
    int total;
    double blackWinRate, whiteWinRate, drawRate;

    if (position.phase == Phase.ready) {
      start();
    }

    print("Computer: $move");

    moveHistory.add(move);

    if (!position.doMove(move)) {
      return false;
    }

    sideToMove = position.sideToMove();

    total = position.score[Color.black] +
        position.score[Color.white] +
        position.score[Color.draw];

    if (total == 0) {
      blackWinRate = 0;
      whiteWinRate = 0;
      drawRate = 0;
    } else {
      blackWinRate = position.score[Color.black] * 100 / total;
      whiteWinRate = position.score[Color.white] * 100 / total;
      drawRate = position.score[Color.draw] * 100 / total;
    }

    String stat = "Score: " +
        position.score[Color.black].toString() +
        " : " +
        position.score[Color.white].toString() +
        " : " +
        position.score[Color.draw].toString() +
        "\ttotal: " +
        total.toString() +
        "\n" +
        blackWinRate.toString() +
        "% : " +
        whiteWinRate.toString() +
        "% : " +
        drawRate.toString() +
        "%" +
        "\n";

    print(stat);
    return true;
  }
}