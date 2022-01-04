// ignore_for_file: use_build_context_synchronously

part of '../mill.dart';

class TapHandler {
  static const _tag = "[Tap Handler]";

  final AnimationController animationController;
  final BuildContext context;

  final controller = MillController();
  final gameMode = MillController().gameInstance.gameMode;
  final showTip = MillController().tip.showTip;

  TapHandler({
    required this.animationController,
    required this.context,
  });

  Future<void> onBoardTap(int sq) async {
    if (gameMode == GameMode.aiVsAi || gameMode == GameMode.testViaLAN) {
      return logger.v("$_tag Engine type is no human, ignore tapping.");
    }

    if (controller.gameInstance._isAiToMove) {
      return logger.i("$_tag AI's turn, skip tapping.");
    }

    final position = controller.position;

    // Human to go
    bool ret = false;
    switch (position._action) {
      case Act.place:
        if (await position._putPiece(sq)) {
          animationController.reset();
          animationController.animateTo(1.0);
          if (position._action == Act.remove) {
            showTip(S.of(context).tipMill, snackBar: true);
          } else {
            if (gameMode == GameMode.humanVsAi) {
              if (DB().rules.mayOnlyRemoveUnplacedPieceInPlacingPhase) {
                showTip(S.of(context).continueToMakeMove);
              } else {
                showTip(S.of(context).tipPlaced);
              }
            } else {
              if (DB().rules.mayOnlyRemoveUnplacedPieceInPlacingPhase) {
                // TODO: HumanVsHuman - Change tip
                showTip(S.of(context).tipPlaced);
              } else {
                final side = controller.gameInstance.sideToMove.opponent
                    .playerName(context);
                showTip(
                  S.of(context).tipToMove(side),
                );
              }
            }
          }
          ret = true;
          logger.v("$_tag putPiece: [$sq]");
          break;
        } else {
          logger.v("$_tag putPiece: skip [$sq]");
          showTip(S.of(context).tipBanPlace);
        }

        // If cannot move, retry select, do not break
        //[[fallthrough]];
        continue select;
      select:
      case Act.select:
        if (position.phase == Phase.placing) {
          showTip(S.of(context).tipCannotPlace, snackBar: true);
          break;
        }
        switch (position._selectPiece(sq)) {
          case SelectionResponse.ok:
            await Audios().playTone(Sound.select);
            controller.gameInstance._select(squareToIndex[sq]!);
            ret = true;
            logger.v("$_tag selectPiece: [$sq]");

            final pieceOnBoardCount =
                position.pieceOnBoardCount[controller.gameInstance.sideToMove];
            if (position.phase == Phase.moving &&
                DB().rules.mayFly &&
                (pieceOnBoardCount == DB().rules.flyPieceCount ||
                    pieceOnBoardCount == 3)) {
              // TODO: [Calcitem, Leptopoda] why is the [DB().rules.flyPieceCount] not respected?
              logger.v("$_tag May fly.");
              showTip(S.of(context).tipCanMoveToAnyPoint, snackBar: true);
            } else {
              showTip(S.of(context).tipPlace, snackBar: true);
            }
            break;
          // TODO: [Leptopoda] deduplicate
          case SelectionResponse.illegalPhase:
            await Audios().playTone(Sound.illegal);
            logger.v("$_tag selectPiece: skip [$sq]");
            if (position.phase != Phase.gameOver) {
              showTip(S.of(context).tipCannotMove, snackBar: true);
            }
            break;
          case SelectionResponse.canOnlyMoveToAdjacentEmptyPoints:
            await Audios().playTone(Sound.illegal);
            logger.v("$_tag selectPiece: skip [$sq]");
            showTip(S.of(context).tipCanMoveOnePoint, snackBar: true);
            break;
          case SelectionResponse.pleaseSelectOurPieceToMove:
            await Audios().playTone(Sound.illegal);
            logger.v("$_tag selectPiece: skip [$sq]");
            showTip(S.of(context).tipSelectPieceToMove, snackBar: true);
            break;
          case SelectionResponse.illegalAction:
            await Audios().playTone(Sound.illegal);
            logger.v("$_tag selectPiece: skip [$sq]");
            showTip(S.of(context).tipSelectWrong, snackBar: true);
            break;
        }
        break;

      case Act.remove:
        try {
          await position._removePiece(sq);

          animationController.reset();
          animationController.animateTo(1.0);

          ret = true;
          logger.v("$_tag removePiece: [$sq]");
          if (position._pieceToRemoveCount >= 1) {
            showTip(S.of(context).tipContinueMill, snackBar: true);
          } else {
            if (gameMode == GameMode.humanVsAi) {
              showTip(S.of(context).tipRemoved);
            } else {
              final them = controller.gameInstance.sideToMove.opponent
                  .playerName(context);
              showTip(S.of(context).tipToMove(them));
            }
          }
        } on CanNotRemoveSelfException {
          await Audios().playTone(Sound.illegal);
          logger.i("$_tag removePiece: Cannot Remove our pieces, skip [$sq]");
          showTip(S.of(context).tipSelectOpponentsPiece, snackBar: true);
        } on CanNotRemoveMillException {
          await Audios().playTone(Sound.illegal);
          logger.i(
            "$_tag removePiece: Cannot remove piece from Mill, skip [$sq]",
          );
          showTip(S.of(context).tipCannotRemovePieceFromMill, snackBar: true);
        } on Exception catch (e) {
          if (e is IllegalActionException ||
              e is IllegalPhaseException ||
              e is NoPieceToRemoveException) {
            await Audios().playTone(Sound.illegal);
            logger.v("$_tag removePiece: skip [$sq]");
            if (position.phase != Phase.gameOver) {
              showTip(S.of(context).tipBanRemove, snackBar: true);
            }
          } else {
            rethrow;
          }
        }
    }

    if (ret) {
      controller.gameInstance.sideToMove = position.sideToMove;

      // TODO: Need Others?
      // Increment ply counters. In particular,
      // rule50 will be reset to zero later on
      // in case of a remove.
      ++position._gamePly;
      ++position.st.rule50;
      ++position.st.pliesFromNull;

      if (position._record != null &&
          position._record!.move.length > "-(1,2)".length) {
        if (position._posKeyHistory.isEmpty ||
            position._posKeyHistory.last != position.st.key) {
          position._posKeyHistory.add(position.st.key);
          if (DB().rules.threefoldRepetitionRule && position._hasGameCycle) {
            position._setGameOver(
              PieceColor.draw,
              GameOverReason.drawThreefoldRepetition,
            );
          }
        }
      } else {
        position._posKeyHistory.clear();
      }

      controller.recorder.add(position._record!);

      if (position.winner == PieceColor.nobody) {
        engineToGo(isMoveNow: false);
      } else {
        _showResult();
      }
    }

    controller.gameInstance.sideToMove = position.sideToMove;
  }

  // TODO: [Leptopoda] the reference of this method has been removed in a few instances.
  // We'll need to find a better way for this.
  Future<void> engineToGo({required bool isMoveNow}) async {
    const _tag = "[engineToGo]";

    final position = controller.position;

    // TODO
    logger.v("$_tag engine type is $gameMode");

    if (isMoveNow) {
      if (!controller.gameInstance._isAiToMove) {
        logger.i("$_tag Human to Move. Cannot get search result now.");
        return ScaffoldMessenger.of(context)
            .showSnackBarClear(S.of(context).notAIsTurn);
      }
      if (controller.recorder.isNotEmpty) {
        logger.i("$_tag History is not clean. Cannot get search result now.");
        return ScaffoldMessenger.of(context)
            .showSnackBarClear(S.of(context).aiIsNotThinking);
      }
    }

    while ((position.winner == PieceColor.nobody ||
            DB().preferences.isAutoRestart) &&
        controller.gameInstance._isAiToMove) {
      if (gameMode == GameMode.aiVsAi) {
        showTip(position.scoreString);
      } else {
        showTip(S.of(context).thinking);

        final String? n = controller.recorder.lastF?.notation;

        if (DB().preferences.screenReaderSupport &&
            position._action != Act.remove &&
            n != null) {
          ScaffoldMessenger.of(context)
              .showSnackBar(CustomSnackBar("${S.of(context).human}: $n"));
        }
      }

      try {
        logger.v("$_tag Searching..., isMoveNow: $isMoveNow");
        final extMove = await controller.engine.search(moveNow: isMoveNow);

        await controller.gameInstance._doMove(extMove);
        animationController.reset();
        animationController.animateTo(1.0);

        if (DB().preferences.screenReaderSupport) {
          ScaffoldMessenger.of(context).showSnackBar(
            CustomSnackBar("${S.of(context).ai}: ${extMove.notation}"),
          );
        }
      } on EngineTimeOutException {
        logger.i("$_tag Engine response type: timeout");
        showTip(S.of(context).timeout, snackBar: true);
      } on EngineNoBestMoveException {
        logger.i("$_tag Engine response type: nobestmove");
        showTip(S.of(context).error("No best move"));
      }

      _showResult();
    }
  }

  void _showResult() {
    final winner = controller.position.winner;
    final message = winner.getWinString(context);
    if (message != null) {
      showTip(message);
    }

    if (!DB().preferences.isAutoRestart && winner != PieceColor.nobody) {
      showDialog(
        context: context,
        builder: (_) => GameResultAlert(winner: winner),
      );
    }
  }
}
