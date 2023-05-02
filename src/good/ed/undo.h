
//
// undo.h
// Editor, undo framework.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/11/08 Waync created.
//

#pragma once

namespace good {

namespace ed {

class UndoCommand
{
public:

  int mCmdId;
  int mTag;

  UndoCommand(int id) : mCmdId(id), mTag(0)
  {
  }

  virtual ~UndoCommand()
  {
  }

  virtual bool exec()=0;

  virtual bool undo()=0;

  virtual
  bool
  redo()=0;

  int getId() const
  {
    return mCmdId;
  }
};

class UndoImpl
{
public:

  typedef std::vector<UndoCommand*>::size_type size_type;

  size_type mUndoLevel;

  size_type mCurCmd;
  std::vector<UndoCommand*> mCmd;

  int mTag;

  explicit UndoImpl(size_type level) : mUndoLevel(level), mCurCmd(0), mTag(0)
  {
  }

  virtual ~UndoImpl()
  {
    clear();
  }

  UndoCommand* getCurCommand()
  {
    if (mCmd.empty() || !canUndo()) {
      return 0;
    } else {
      return mCmd[mCurCmd - 1];
    }
  }

  bool canUndo() const
  {
    return 0 < mCurCmd;
  }

  bool canRedo() const
  {
    return mCmd.size() > mCurCmd;
  }

  bool undo()
  {
    if (!canUndo()) {
      return false;
    }

    int tag = mCmd[mCurCmd - 1]->mTag;

    do {

      if (!mCmd[mCurCmd - 1]->undo()) {
        return false;
      }

      mCurCmd--;

      if (!canUndo()) {
        return true;
      }

    } while (tag == mCmd[mCurCmd - 1]->mTag);

    return true;
  }

  bool redo()
  {
    if (!canRedo()) {
      return false;
    }

    int tag = mCmd[mCurCmd]->mTag;

    do {

      if (!mCmd[mCurCmd]->redo()) {
        return false;
      }

      mCurCmd++;

      if (!canRedo()) {
        return true;
      }

    } while (tag == mCmd[mCurCmd]->mTag);

    return true;
  }

  bool addCommand(UndoCommand* pCmd)
  {
    assert(pCmd);

    if (canRedo()) {
      for (size_type i = mCurCmd; i < mCmd.size(); ++i) {
        delete mCmd[i];
      }
      mCmd.erase(mCmd.begin() + mCurCmd, mCmd.end());
    }

    if (mCmd.size() == mUndoLevel) {
      delete mCmd[0];
      mCmd.erase(mCmd.begin());
    }

    mCmd.push_back(pCmd);
    mCurCmd = mCmd.size();

    pCmd->mTag = mTag++;

    return true;
  }

  void clear()
  {
    for (size_type i = 0; i < mCmd.size(); ++i) {
      delete mCmd[i];
    }
    mCmd.clear();
  }

  bool execAndAdd(UndoCommand* pCmd)
  {
    assert(pCmd);

    if (!pCmd->exec()) {
      delete pCmd;
      return false;
    }

    addCommand(pCmd);

    return true;
  }
};

template<class PrjT>
class UndoSupport
{
public:

  UndoImpl mUndo;

  UndoSupport() : mUndo(PrjT::UNDO_LEVEL)
  {
  }

  bool canRedo()
  {
    return mUndo.canRedo();
  }

  bool canUndo()
  {
    return mUndo.canUndo();
  }

  bool redo()
  {
    if (mUndo.redo()) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool undo()
  {
    if (mUndo.undo()) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }
};

} // namespace ed

} // namespace good

// end of undo.h
