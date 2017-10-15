libgood
-----------------------------
libgood for Android is the Good Game Engine Runtime Library to build small game
APP for Android platform. Good Game Engine is a cross platform, small and simple
2D game engine. You can use it for any purpose.

How to use libgood?
-----------------------------
1. Copy all files under libs to your Android project libs folder.
2. Prepare your game asserts that created by Good Game Editor, and put them under
asserts folder of your Android project. It is recommand to generate a stand alone
Good Game Package(*.good).
3. Create a simple activity class and inherits goodJniActivity, following is a
simple and workable sample:

01|package weilican.m25940;
02|
03|import weilican.good.*;
04|
05|import android.os.Bundle;
06|
07|public class m25940Activity extends goodJniActivity
08|{
09|  @override protected void onCreate(Bundle b)
10|  {
11|    super.onCreate(b);
12|  }
13|
14|  @override protected void DoChooseFile() {
15|    goodJniLib.create("25940m.good", getAssets());
16|  }
17|}

4. Rebuild your project and try it!

Support
-----------------------------
For more information, please visits official blog: https://good-ed.blogspot.com
