local SCR_W, SCR_H = Good.GetWindowSize()
local w, h = 200, 200

TestBlock = {}

TestBlock.OnCreate = function(param)
  if (math.random(2) == 1) then
    param.dirx = -1 * math.random(1,3)
    param.diry = 1 * math.random(1,3)
    param.texx = -1
    param.texy = 1
  else
    param.dirx = 1 * math.random(1,3)
    param.diry = -1 * math.random(1,3)
    param.texx = -1
    param.texy = 1
  end
  Good.SetPos(param._id, math.random(0, SCR_W - w), math.random(0, SCR_H - h))
end

TestBlock.OnStep = function(param)
  local id = param._id
  local x,y = Good.GetPos(id)
  x = x + param.dirx
  y = y + param.diry
  Good.SetPos(id, x, y)
  local l,t,w,h = Good.GetDim(id)
  if (SCR_W <= x + w or 0 >= x) then
    param.dirx = -1 * param.dirx
  end
  if (SCR_H <= y + h or 0 >= y) then
    param.diry = -1 * param.diry
  end
  l = l + param.texx
  t = t + param.texy
  local TW,TH = Resource.GetTexSize(1)
  if (TW <= l + w or 0 >= l) then
    param.texx = -1 * param.texx
  end
  if (TH <= t + h or 0 >= t) then
    param.texy = -1 * param.texy
  end
  Good.SetDim(id, l,t,w,h)
end

Level = {}

Level.OnCreate = function(param)
  math.randomseed(os.clock())
  for i = 1,8 do
    local o = GenTexObj(-1, 1, w, w, math.random(1, w), math.random(1, w), 'TestBlock')
  end
end
