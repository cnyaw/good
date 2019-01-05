-- Simple animator support.

local TICK_PER_SECOND = 60

function ArSetValue(o, n, v1, v2)
  if (nil == v2) then
    assert(loadstring('Good.Set' .. n .. '(' .. o .. ',' .. v1 .. ')'))()
  else
    assert(loadstring('Good.Set' .. n .. '(' .. o .. ',' .. v1 .. ',' .. v2 .. ')'))()
  end
end

function ArGetValue(o, n)
  return assert(loadstring('return Good.Get' .. n .. '(' .. o .. ')'))()
end

function ArLerp(v0, v1, t)
  return (1 - t) * v0 + t * v1
end

ArAddAr = function(Parent, n, dt, v1, v2, OnStep, fn)
  local ar = {}
  ar.v1 = v1
  ar.v2 = v2
  ar.Duration = math.floor(dt * TICK_PER_SECOND) -- Convert seconds to frames.
  ar.dt = 0
  ar.n = n
  ar.OnStep = OnStep
  ar.fn = fn
  ar.lerp = ArLerp
  ArAppendAr(Parent, ar)
  return ar
end

ArOnStep = function(param, f1, f2)
  if (nil == param.ar) then
    return
  end
  local o = param._id
  local ar = param.ar
  if (nil == ar.SrcV1) then
    ar.SrcV1, ar.SrcV2 = ArGetValue(o, ar.n)
  end
  local dt = 1
  if (0 < ar.Duration) then
    dt = ar.dt / ar.Duration
    if (nil ~= ar.ease) then
      dt = ar.ease(dt)
    end
  end
  if (nil == ar.SrcV2) then
    ArSetValue(o, ar.n, ar.lerp(ar.SrcV1, f1(ar), dt))
  else
    ArSetValue(o, ar.n, ar.lerp(ar.SrcV1, f1(ar), dt), ar.lerp(ar.SrcV2, f2(ar), dt))
  end
  if (ar.dt < ar.Duration) then
    ar.dt = ar.dt + 1
  else
    ar.SrcV1 = nil
    ar.dt = 0
    param.ar = nil
  end
end

ArAddMoveTo = function(Parent, n, dt, DstV1, DstV2)
  local f = function(param)
    ArOnStep(param, function(ar) return ar.v1 end, function(ar) return ar.v2 end)
  end
  return ArAddAr(Parent, n, dt, DstV1, DstV2, f)
end

ArAddMoveBy = function(Parent, n, dt, v1, v2)
  local f = function(param)
    ArOnStep(param, function(ar) return (ar.SrcV1 + ar.v1) end, function(ar) return (ar.SrcV2 + ar.v2) end)
  end
  return ArAddAr(Parent, n, dt, v1, v2, f)
end

ArOnStepDelay = function(param)
  if (nil == param.ar) then
    return
  end
  local ar = param.ar
  if (ar.dt ~= ar.Duration) then
    ar.dt = ar.dt + 1
  else
    ar.dt = 0
    param.ar = nil
    if (nil ~= ar.fn) then
      ArAddCallTmp = param
      assert(loadstring(ar.fn .. '(ArAddCallTmp)'))()
    end
  end
end

ArAddDelay = function(Parent, dt)
  return ArAddAr(Parent, nil, dt, nil, nil, ArOnStepDelay)
end

ArAddCall = function(Parent, fn, dt)
  return ArAddAr(Parent, nil, dt, nil, nil, ArOnStepDelay, fn)
end

ArOnStepLoop = function(param)
  if (nil == param.ar) then
    return
  end
  local loop = param.ar
  local ar = loop.Alist[loop.Cur]
  if (nil ~= ar) then
    param.ar = ar
    ar.OnStep(param)
  end
  if (nil == param.ar) then             -- End of animation.
    loop.Cur = loop.Cur + 1
    if (#loop.Alist < loop.Cur) then    -- End of animation loop.
      if (nil ~= loop.Cnt) then         -- None infinite loop.
        loop.Cnt = loop.Cnt - 1
        loop.Cur = 1
        if (0 == loop.Cnt) then
          loop.Cnt = loop.InitCnt
          loop = nil
        end
      else                              -- Infinte loop.
        loop.Cur = 1
      end
    end
  end
  param.ar = loop
end

ArAddLoop = function(Parent, Cnt)
  local ar = {}
  ar.InitCnt = Cnt
  ar.Cnt = Cnt
  ar.Cur = 1
  ar.Alist = {}
  ar.OnStep = ArOnStepLoop
  ArAppendAr(Parent, ar)
  return ar
end

ArAppendAr = function(Parent, ar)
  if (nil ~= Parent) then
    table.insert(Parent.Alist, ar)
  end
end

ArAddAnimator = function(t)
  return t
end

ArStepAnimator = function(param, arl)
  for i = 1, #arl do
    if (nil ~= arl[i]) then
      local ar = arl[i]
      param.ar = ar
      ar.OnStep(param)
      if (nil == param.ar) then
        arl[i] = nil
      end
    end
  end
end

-- Ease function.
-- https://github.com/dmccuskey/DMC-Corona-UI/blob/master/lib/dmc_ui/dmc_widget/lib/easingx.lua

ArEaseIn = function(t)
  return t * t * t
end

ArEaseOut = function(t)
  local invt = t - 1.0
  return (invt * invt * invt) + 1.0
end

ArEaseInOut = function(t)
  if (t < 0.5) then
    return 0.5 * ArEaseIn(t * 2.0)
  else
    return 0.5 * ArEaseOut((t - 0.5) * 2.0) + 0.5
  end
end

ArEaseOutIn = function(t)
  if (t < 0.5) then
    return 0.5 * ArEaseOut(t * 2.0)
  else
    return 0.5 * ArEaseIn((t - 0.5) * 2.0) + 0.5
  end
end

ArEaseInBack = function(t)
  local s = 1.70158
  return math.pow(t, 2.0) * ((s + 1.0) * t - s)
end

ArEaseOutBack = function(t)
  local invt = t - 1.0
  local s = 1.70158
  return math.pow(invt, 2.0) * ((s + 1.0) * invt + s) + 1.0
end

ArEaseInOutBack = function(t)
  if (t < 0.5) then
    return 0.5 * ArEaseInBack(t * 2.0)
  else
    return 0.5 * ArEaseOutBack((t - 0.5) * 2.0) + 0.5
  end
end

ArEaseOutInBack = function(t)
  if (t < 0.5) then
    return 0.5 * ArEaseOutBack(t * 2.0)
  else
    return 0.5 * ArEaseInBack((t - 0.5) * 2.0) + 0.5
  end
end

ArEaseInBounce = function(t)
  return 1.0 - ArEaseOutBounce(1.0 - t)
end

ArEaseOutBounce = function(t)
  local s = 7.5625
  local p = 2.75
  local l
  if t < (1.0 / p) then
    l = s * math.pow(t, 2.0)
  else
    if t < (2.0 / p) then
      t = t - (1.5 / p)
      l = s * math.pow(t, 2.0) + 0.75
    else
      if t < (2.5 / p) then
        t = t - (2.25 / p)
        l = s * math.pow(t, 2.0) + 0.9375
      else
        t = t - (2.65 / p)
        l = s * math.pow(t, 2.0) + 0.984375
      end
    end
  end
  return l
end

ArEaseInOutBounce = function(t)
  if (t < 0.5) then
    return 0.5 * ArEaseInBounce(t * 2.0)
  else
    return 0.5 * ArEaseOutBounce((t - 0.5) * 2.0) + 0.5
  end
end

ArEaseOutInBounce = function(t)
  if (t < 0.5) then
    return 0.5 * ArEaseOutBounce(t * 2.0)
  else
    return 0.5 * ArEaseInBounce((t - 0.5) * 2.0) + 0.5
  end
end

ArEaseInElastic = function(t)
  if t == 0 or t == 1.0 then return t end
  local p = 0.3
  local s = p / 4.0
  local invt = t - 1.0
  return -1 * math.pow(2.0, 10.0 * invt) * math.sin((invt - s) * 2 * math.pi / p)
end

ArEaseOutElastic = function(t)
  if t == 0 or t == 1.0 then return t end
  local p = 0.3
  local s = p / 4.0
  return -1 * math.pow(2.0, -10.0 * t) * math.sin((t + s) * 2 * math.pi / p) + 1.0
end

ArEaseInOutElastic = function(t)
  if (t < 0.5) then
    return 0.5 * ArEaseInElastic(t * 2.0)
  else
    return 0.5 * ArEaseOutElastic((t - 0.5) * 2.0) + 0.5
  end
end

ArEaseOutInElastic = function(t)
  if (t < 0.5) then
    return 0.5 * ArEaseOutElastic(t * 2.0)
  else
    return 0.5 * ArEaseInElastic((t - 0.5) * 2.0) + 0.5
  end
end
