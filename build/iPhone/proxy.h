/*
 *  proxy.h
 *  good proxy
 *
 *  Created by waync on 2009/7/16.
 *
 */

void goodInit(const char* pathApp, int w, int h);
void goodStepOneFrame();
void goodDraw(int x, int y);

void goodBeginInput();
void goodAddInput(int x, int y);
void goodEndInput(int state); // 0:begin, 1:move, 2:end

// end of proxy.h
