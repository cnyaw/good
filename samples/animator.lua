
Anim1 = {}

Anim1.OnStep = function(param)
  if (nil == param.k) then
    local loop1 = ArAddLoop()
    ArAddMoveBy(loop1, 'Rot', 0, 1)
    local loop2 = ArAddLoop()
    ArAddMoveTo(loop2, 'Scale', 0.75, 0, 0).ease = ArEaseIn
    ArAddMoveTo(loop2, 'Scale', 0.75, 1, 1).ease = ArEaseOutElastic
    param.k = ArAddAnimator({loop1, loop2})
  else
    ArStepAnimator(param, param.k)
  end
end

Anim2 = {}

Anim2.OnCreate = function(param)
  local o = param._id
  local x,y = Good.GetPos(o)
  local ididx = o - 20 + 1
  local lp = ArAddLoop()
  local Ease1 = {ArEaseIn, ArEaseOut, ArEaseInOut, ArEaseOutIn}
  ArAddMoveTo(lp, 'Pos', 2, 550, y).ease = Ease1[ididx]
  ArAddDelay(lp, 1)
  ArAddMoveTo(lp, 'Pos', 0, 96, y)
  local Ease2 = {ArEaseInBack, ArEaseOutBack, ArEaseInOutBack, ArEaseOutInBack}
  ArAddMoveTo(lp, 'Pos', 2, 550, y).ease = Ease2[ididx]
  ArAddDelay(lp, 1)
  ArAddMoveTo(lp, 'Pos', 0, 96, y)
  local Ease3 = {ArEaseInBounce, ArEaseOutBounce, ArEaseInOutBounce, ArEaseOutInBounce}
  ArAddMoveTo(lp, 'Pos', 2, 550, y).ease = Ease3[ididx]
  ArAddDelay(lp, 1)
  ArAddMoveTo(lp, 'Pos', 0, 96, y)
  local Ease4 = {ArEaseInElastic, ArEaseOutElastic, ArEaseInOutElastic, ArEaseOutInElastic}
  ArAddMoveTo(lp, 'Pos', 2, 550, y).ease = Ease4[ididx]
  ArAddDelay(lp, 1)
  ArAddMoveTo(lp, 'Pos', 0, 96, y)
  param.k = ArAddAnimator({lp})
end

Anim2.OnStep = function(param)
  ArStepAnimator(param, param.k)
end

ToggleColorObj = function(param)
  local o = 26
  local c = Good.GetBgColor(o)
  local alpha = Good.GetAlpha(o)
  local c2 = c - Lshift(Rshift(c, 24), 24)
  if (0xff0000 == c2) then
    Good.SetBgColor(o, 0x00ffff)
  else
    Good.SetBgColor(o, 0xff0000)
  end
  Good.SetAlpha(o, alpha)
end

Anim3 = {}

Anim3.OnCreate = function(param)
  Good.SetAnchor(param._id, 0.5, 0.5)
  local loopn = ArAddLoop()
  local loop2 = ArAddLoop(loopn, 2)
  ArAddMoveTo(loop2, 'Pos', 1, 450, 350)
  ArAddMoveTo(loop2, 'Pos', 2, 50, 200)
  ArAddMoveTo(loop2, 'Pos', 0.5, 450, 40)
  ArAddMoveTo(loopn, 'Pos', 1, 10, 310)
  ArAddMoveBy(loopn, 'Pos', 1, 40, 20)
  ArAddCall(loopn, 'ToggleColorObj', 0)

  local loopn2 = ArAddLoop()
  ArAddMoveBy(loopn2, 'Rot', 1, 150)

  local loopn3 = ArAddLoop()
  ArAddMoveBy(loopn3, 'Scale', 2, 1.5, 0)
  ArAddMoveBy(loopn3, 'Scale', 2, 0, 1.5)
  ArAddDelay(loopn3, 5)
  ArAddMoveBy(loopn3, 'Scale', 2, 0, -1.5)
  ArAddMoveBy(loopn3, 'Scale', 2, -1.5, 0)
  ArAddDelay(loopn3, 5)

  local loopn4 = ArAddLoop(nil, 5)
  ArAddMoveTo(loopn4, 'BgColor', 2, 0xffff0000)
  ArAddMoveTo(loopn4, 'BgColor', 2, 0xffffffff)
  
  param.k = ArAddAnimator({loopn, loopn2, loopn3, loopn4})
end

Anim3.OnStep = function(param)
  ArStepAnimator(param, param.k)
end

Anim4 = {}

Anim4.OnStep = function(param)
  if (nil == param.k) then
    local loop1 = ArAddLoop()
    ArAddMoveTo(loop1, 'Alpha', 1.5, 0)
    ArAddMoveTo(loop1, 'Alpha', 1.5, 0xff)
    param.k = ArAddAnimator({loop1})
  else
    ArStepAnimator(param, param.k)
  end
end
