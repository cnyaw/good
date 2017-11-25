local SCR_W, SCR_H = Good.GetWindowSize()

TestBlock = {}

TestBlock.OnCreate = function(param)
  param.dirx = math.random(1,5)
  if (math.random(2) == 1) then
    param.dirx = -1 * param.dirx
  end
  param.diry = math.random(1,5)
  if (math.random(2) == 1) then
    param.diry = -1 * param.diry
  end
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
end

Level = {}

Level.OnCreate = function(param)
  math.randomseed(os.clock())
  for i = 1,2000 do
    local w, h = math.random(32, 128), math.random(32, 128)
    local color = 0xff000000 + math.random(65535) * 256
    local o = GenColorObj(-1, w, h, color, 'TestBlock')
    Good.SetPos(o, math.random(0, SCR_W - w), math.random(0, SCR_H - h))
  end
end
