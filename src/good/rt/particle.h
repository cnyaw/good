
//
// particle.h
// Runtime, particle(stge object).
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/06/10 Waync created.
//

#pragma once

namespace good {

namespace rt {

template<class AppT, class ParticleT>
class ParticleManager : public stge::ObjectManager<ParticleManager<AppT, ParticleT>, ParticleT>
{
public:

  typedef stge::ObjectManager<ParticleManager, ParticleT> BaseT;

  int iMgr;
  bool active;

  float offsetX, offsetY;
  std::map<int, int> idBind;            // idAction, idBind.

  ParticleManager()
  {
    reset();
  }

  void freeAction(int id)
  {
    BaseT::freeAction(id);

    std::map<int, int>::iterator it = idBind.find(id);
    if (idBind.end() != it) {
      idBind.erase(it);
    }
  }

  void freeObject(int id)
  {
    AppT& app = AppT::getInst();

    typename AppT::ActorT& a = app.mActors[app.mRoot];
    a.OnKillParticle(id, iMgr);

    BaseT::freeObject(id);
  }

  void reset()
  {
    BaseT::reset();

    offsetX = offsetY = .0f;
    idBind.clear();
  }

  template<typename PlayerT>
  void update(float fElapsed, PlayerT& player)
  {
    AppT const& app = AppT::getInst();

    for (std::map<int,int>::const_iterator it = idBind.begin();
         idBind.end() != it;
         ++it) {

      if (!app.mActors.isUsed(it->second)) {
        continue;
      }

      typename AppT::ActorT const& a = app.mActors[it->second];

      float x,y;
      a.getPos(x,y);

      typename BaseT::ActionT &ac = BaseT::actions[it->first];
      ac.x = x - offsetX;
      ac.y = y - offsetY;
    }

    for (int i = BaseT::objects.first(); -1 != i; i = BaseT::objects.next(i)) {

      ParticleT& p = BaseT::objects[i];

      if (!app.mActors.isUsed(p.idBind)) {
        continue;
      }

      typename AppT::ActorT const& a = app.mActors[p.idBind];

      float x,y;
      a.getPos(x,y);

      p.x = x - offsetX;
      p.y = y - offsetY;
    }

    BaseT::update(fElapsed, player);
  }

  static int getWindowWidth()
  {
    return AppT::getInst().mRes.mWidth;
  }

  static int getWindowHeight()
  {
    return AppT::getInst().mRes.mHeight;
  }
};

template<class AppT>
class Particle : public stge::Object
{
public:

  typedef ParticleManager<AppT, Particle<AppT> > ObjMgrT;

  int idThis;
  int idBind;

  bool init(ObjMgrT& om, int idAction, int idNewObj) // Return false if init failed, and clear this object.
  {
    idThis = idNewObj;
    idBind = -1;

    AppT& app = AppT::getInst();

    typename AppT::ActorT& a = app.mActors[app.mRoot];
    a.OnNewParticle(idNewObj, om.iMgr);

    return true;
  }

  template<typename PlayerT>
  bool update(float fElapsed, ObjMgrT& om, PlayerT& player) // Return false, and clear this object.
  {
    stge::Object::update(fElapsed, om, player);

    float newx = x + om.offsetX;
    float newy = y + om.offsetY;

    AppT& app = AppT::getInst();

    if (!app.mStgeBounding.ptInRect(sw2::IntPoint((int)newx, (int)newy))) {
      return false;
    }

    if (app.mActors.isUsed(idBind)) {
      typename AppT::ActorT& a = app.mActors[idBind];
      a.mPosX = newx;
      a.mPosY = newy;
      app.mDirty = true;
    }

    return true;
  }
};

} // namespace rt

} // namespace good

// end of particle.h
