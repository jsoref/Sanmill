/*
  FlutterMill, a mill game playing frontend derived from ChessRoad
  Copyright (C) 2019 He Zhaoyun (ChessRoad author)
  Copyright (C) 2019-2020 Calcitem <calcitem@outlook.com>

  FlutterMill is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FlutterMill is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import './routes/main_menu.dart';
import 'services/audios.dart';
import 'services/player.dart';

void main() {
  //
  runApp(SanmillApp());

  SystemChrome.setPreferredOrientations(
    [DeviceOrientation.portraitUp, DeviceOrientation.portraitDown],
  );

  if (Platform.isAndroid) {
    SystemChrome.setSystemUIOverlayStyle(
      SystemUiOverlayStyle(statusBarColor: Colors.transparent),
    );
  }

  SystemChrome.setEnabledSystemUIOverlays([]);
}

class SanmillApp extends StatefulWidget {
  //
  static const StatusBarHeight = 28.0;

  @override
  _SanmillAppState createState() => _SanmillAppState();
}

class _SanmillAppState extends State<SanmillApp> {
  //
  @override
  void initState() {
    super.initState();
    //Audios.loopBgm('bg_music.mp3');
    Player.loadProfile();
  }

  @override
  Widget build(BuildContext context) {
    //
    return MaterialApp(
      theme: ThemeData(primarySwatch: Colors.brown),
      debugShowCheckedModeBanner: false,
      home: WillPopScope(
        onWillPop: () async {
          Audios.release();
          return true;
        },
        child: MainMenu(),
      ),
    );
  }
}
