
//
// override.h
// Default overrideables.
//
// Copyright (c) 2016 Waync Cheng.
// All Rights Reserved.
//
// 2016/6/29 Waync created.
//

#pragma once

void onAppCreate()
{
}

void onAppDestroy()
{
}

void onAppPause()
{
}

bool doInit(std::string const& prjname)
{
  return true;
}

void doUninit()
{
}

void onPackageChanged()
{
}

void doTrace(std::string const &s)
{
}

bool doOpenUrl(std::string const& url) const
{
  return false;
}

void doUserIntEvent(int i)
{
}

void onRender()
{
}

void onTrigger()
{
}

// end of override.h
