
//
// stge.h
// Runtime, STGE support.
//
// Copyright (c) 2015 Waync Cheng.
// All Rights Reserved.
//
// 2015/11/25 Waync created.
//

#pragma once

//
// STGE support.
//

typedef Particle<T> ParticleT;

stge::ScriptManager mScriptMgr;

typedef ParticleManager<T, ParticleT> ParticleMgrT;

ParticleMgrT mObjMgr[GOOD_MAX_STGE_OBJ_MGR];

stge::Object mStgePlayer;               // Player obj.
int mGoodPlayer;                        // idActor.

sw2::IntRect mStgeBounding;

void initStge()
{
  mGoodPlayer = -1;

  for (int i = 0; i < GOOD_MAX_STGE_OBJ_MGR; ++i) {
    ParticleMgrT &pm = mObjMgr[i];
    pm.init(&mScriptMgr);
    pm.iMgr = i;
    pm.active = true;
  }

  for (size_t i = 0; i < mRes.mStgeScriptIdx.size(); ++i) {
    std::string name = mRes.mStgeScript[mRes.mStgeScriptIdx[i]];
    std::stringstream ss;
    if (!loadFile(name, ss)) {
      continue;
    }
    if (!stge::Parser::parse(ss, mScriptMgr)) {
      trace("[%s] %s\n", name.c_str(), stge::Parser::getLastError().c_str());
    }
  }
}

void uninitStge()
{
  resetStge();

  mScriptMgr.clear();
}

void resetStge()
{
  for (int i = 0; i < GOOD_MAX_STGE_OBJ_MGR; ++i) {
    mObjMgr[i].reset();
  }
}

bool triggerStge()
{
  float et = 1 / (float)mRes.mFps;

  ActorT const& actor = mActors[mRoot];
  if (ActorT::TYPES_LEVEL != actor.mResType) {
    return false;
  }

  LevelT const& lvl = mRes.getLevel(actor.mResId);

  float offsetx = lvl.mWidth / 2.0f, offsety = lvl.mHeight / 2.0f;

  if (mActors.isUsed(mGoodPlayer)) {
    ActorT& player = mActors[mGoodPlayer];
    float x, y;
    player.getPos(x, y);
    mStgePlayer.x = x - offsetx;
    mStgePlayer.y = y - offsety;
  } else {
    mStgePlayer.x = 0;
    mStgePlayer.y = 0;
  }

  mStgeBounding = sw2::IntRect(0, 0, lvl.mWidth, lvl.mHeight);
  mStgeBounding.inflate(20, 20);

  for (int i = 0; i < GOOD_MAX_STGE_OBJ_MGR; ++i) {
    ParticleMgrT &pm = mObjMgr[i];
    if (pm.active) {
      pm.offsetX = offsetx;
      pm.offsetY = offsety;
      pm.update(et, mStgePlayer);
    }
  }

  return true;
}

//
// API.
//

bool iObjMgrValid(int iObjMgr) const
{
  if (0 > iObjMgr || GOOD_MAX_STGE_OBJ_MGR <= iObjMgr) {
    return false;
  } else {
    return true;
  }
}

void bindParticle(int idParticle, int idObj, int iObjMgr)
{
  if (!iObjMgrValid(iObjMgr)) {
    return;
  }

  if (!mActors.isUsed(idObj)) {
    idObj = -1;
  }

  ParticleMgrT& om = mObjMgr[iObjMgr];
  if (om.objects.isUsed(idParticle)) {
    om.objects[idParticle].idBind = idObj;
  } else {
    om.objects[idParticle].idBind = -1;
  }
}

void bindTask(int idTask, int idObj, int iObjMgr)
{
  if (!iObjMgrValid(iObjMgr)) {
    return;
  }

  ParticleMgrT& om = mObjMgr[iObjMgr];
  if (om.actions.isUsed(idTask)) {
    om.idBind[idTask] = idObj;
  } else {
    om.idBind[idTask] = -1;
  }
}

int getFirstParticle(int iObjMgr) const
{
  if (!iObjMgrValid(iObjMgr)) {
    return -1;
  } else {
    return mObjMgr[iObjMgr].objects.first();
  }
}

int getFirstTask(int iObjMgr) const
{
  if (!iObjMgrValid(iObjMgr)) {
    return -1;
  } else {
    return mObjMgr[iObjMgr].actions.first();
  }
}

int getNextParticle(int idCur, int iObjMgr) const
{
  if (!iObjMgrValid(iObjMgr)) {
    return -1;
  } else {
    return mObjMgr[iObjMgr].objects.next(idCur);
  }
}

int getNextTask(int idCur, int iObjMgr) const
{
  if (!iObjMgrValid(iObjMgr)) {
    return -1;
  } else {
    return mObjMgr[iObjMgr].actions.next(idCur);
  }
}

int getParticleBind(int idParticle, int iObjMgr) const
{
  if (iObjMgrValid(iObjMgr)) {
    const ParticleMgrT& om = mObjMgr[iObjMgr];
    if (om.objects.isUsed(idParticle)) {
      return om.objects[idParticle].idBind;
    }
  }
  return -1;
}

int getParticleCount(int iObjMgr) const
{
  if (!iObjMgrValid(iObjMgr)) {
    return 0;
  } else {
    return mObjMgr[iObjMgr].objects.size();
  }
}

float getParticleDirection(int idParticle, int iObjMgr) const
{
  if (iObjMgrValid(iObjMgr)) {
    const ParticleMgrT& om = mObjMgr[iObjMgr];
    if (om.objects.isUsed(idParticle)) {
      return om.objects[idParticle].direction;
    }
  }
  return .0f;
}

void getParticlePos(int idParticle, float &x, float &y, int iObjMgr) const
{
  if (iObjMgrValid(iObjMgr)) {
    const ParticleMgrT& om = mObjMgr[iObjMgr];
    if (om.objects.isUsed(idParticle)) {
      x = om.objects[idParticle].x + om.offsetX;
      y = om.objects[idParticle].y + om.offsetY;
    }
  }
}

int getTaskBind(int idTask, int iObjMgr) const
{
  if (iObjMgrValid(iObjMgr)) {
    const ParticleMgrT& om = mObjMgr[iObjMgr];
    if (om.actions.isUsed(idTask)) {
      std::map<int, int>::const_iterator it = om.idBind.find(idTask);
      if (om.idBind.end() != it) {
        return it->second;
      }
    }
  }
  return -1;
}

int getTaskCount(int iObjMgr) const
{
  if (!iObjMgrValid(iObjMgr)) {
    return 0;
  } else {
    return mObjMgr[iObjMgr].actions.size();
  }
}

float getUserData(int idParticle, int index, int iObjMgr) const
{
  if (iObjMgrValid(iObjMgr) && 0 <= index && stge::Object::NUM_USERDATA > index) {
    const ParticleMgrT& om = mObjMgr[iObjMgr];
    if (om.objects.isUsed(idParticle)) {
      return om.objects[idParticle].user[index];
    }
  }
  return .0f;
}

bool isStgeActive(int iObjMgr) const
{
  if (iObjMgrValid(iObjMgr)) {
    return mObjMgr[iObjMgr].active;
  } else {
    return false;
  }
}

void killAllParticle(int iObjMgr)
{
  if (!iObjMgrValid(iObjMgr)) {
    return;
  }

  ParticleMgrT& om = mObjMgr[iObjMgr];
  for (int i = om.objects.first(); -1 != i; ) {
    int next = om.objects.next(i);
    om.freeObject(i);
    i = next;
  }
}

void killAllTask(int iObjMgr)
{
  if (!iObjMgrValid(iObjMgr)) {
    return;
  }

  ParticleMgrT& om = mObjMgr[iObjMgr];
  for (int i = om.actions.first(); -1 != i; ) {
    int next = om.actions.next(i);
    om.freeAction(i);
    i = next;
  }
}

void killParticle(int idParticle, int iObjMgr)
{
  if (iObjMgrValid(iObjMgr)) {
    mObjMgr[iObjMgr].freeObject(idParticle);
  }
}

void killTask(int idTask, int iObjMgr)
{
  if (iObjMgrValid(iObjMgr)) {
    mObjMgr[iObjMgr].freeAction(idTask);
  }
}

int runStgeScript(const char* script, float *x, float *y, int iObjMgr)
{
  if (script && iObjMgrValid(iObjMgr)) {
    ParticleMgrT& om = mObjMgr[iObjMgr];
    return om.run(script, x ? *x - om.offsetX : 0, y ? *y - om.offsetY : 0);
  } else {
    return -1;
  }
}

void setPlayer(int idPlayer)
{
  if (mActors.isUsed(idPlayer)) {
    mGoodPlayer = idPlayer;
  }
}

void setParticleDirection(int idParticle, float newdir, int iObjMgr)
{
  if (!iObjMgrValid(iObjMgr)) {
    return;
  }

  ParticleMgrT& om = mObjMgr[iObjMgr];
  if (om.objects.isUsed(idParticle)) {
    om.objects[idParticle].direction = newdir;
  }
}

void setParticlePos(int idParticle, float newx, float newy, int iObjMgr)
{
  if (!iObjMgrValid(iObjMgr)) {
    return;
  }

  ParticleMgrT& om = mObjMgr[iObjMgr];
  if (om.objects.isUsed(idParticle)) {
    om.objects[idParticle].x = newx - om.offsetX;
    om.objects[idParticle].y = newy - om.offsetY;
  }
}

void setStgeActive(bool bActive, int iObjMgr)
{
  if (iObjMgrValid(iObjMgr)) {
    mObjMgr[iObjMgr].active = bActive;
  }
}

void setTaskDirection(int idTask, float newdir, int iObjMgr)
{
  if (!iObjMgrValid(iObjMgr)) {
    return;
  }

  ParticleMgrT& om = mObjMgr[iObjMgr];
  if (om.actions.isUsed(idTask)) {
    om.actions[idTask].direction = newdir;
  }
}

void setUserData(int idParticle, int index, float data, int iObjMgr)
{
  if (!iObjMgrValid(iObjMgr)) {
    return;
  }

  if (0 > index || stge::Object::NUM_USERDATA <= index) {
    return;
  }

  ParticleMgrT& om = mObjMgr[iObjMgr];
  if (om.objects.isUsed(idParticle)) {
    om.objects[idParticle].user[index] = data;
  }
}

// end of stge.h
