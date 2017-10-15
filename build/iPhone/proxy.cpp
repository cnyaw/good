/*
 *  proxy.cpp
 *  good proxy
 *
 *  Created by waync on 2009/7/16.
 *
 */

#include <math.h>
#include <string.h>

#include <bitset>
#include <deque>
#include <stdexcept>
#include <fstream>
#include <iterator>

extern "C" {
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "proxy.h"
}

#define GOOD_USE_STGE
#include "rt/rt.h"
#include "iPhone_app.h"

good::gx::GLGraphics gx;

void goodInit(const char* pathApp, int w, int h)
{
    gx.SCREEN_W = w;
    gx.SCREEN_H = h;
    gx.init();
    
    std::string package(pathApp);
    package += "/samples.txt";
    
    good::rt::IPhoneApplication::getInst().init(package);
}

void goodStepOneFrame()
{
    good::rt::IPhoneApplication::getInst().doTrigger();
}

void goodDraw(int x, int y)
{
    good::rt::IPhoneApplication::getInst().doRender(gx);
}

std::vector<sw2::POINT_t<int> > input_pair;

void goodBeginInput()
{
    input_pair.clear();
}

void goodAddInput(int x, int y)
{
    sw2::POINT_t<int> pt(x,y);
    input_pair.push_back(pt);
}

void goodEndInput(int state) // 0:begin, 1:move, 2:end
{
    good::rt::IPhoneApplication::getInst().handleTouchs(input_pair, state);
}

// end of proxy.cpp

