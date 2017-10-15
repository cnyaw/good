
SolarObj = {}

SolarObj.OnStep = function(param)
  local v = 2 * math.pi * param.t / param.c
  local x = param.d * math.cos(v) + param.ox
  local y = param.d * math.sin(v) + param.oy
  Good.SetPos(param._id, x, y)
  param.t = (param.t + 1) % param.c
end

function InitObj(obj, dist, cycle, offsetx, offsety)
  local p = Good.GetParam(obj)
  p.d = dist
  p.c = cycle
  p.ox = offsetx
  p.oy = offsety
  p.t = 0
  Good.SetScript(obj, 'SolarObj')
  SolarObj.OnStep(p)
end

Level = {}

Level.OnCreate = function(param)
  local id = param._id

  local sun = Good.FindChild(id, 'sun')
  local earth = Good.FindChild(sun, 'earth')
  local moon = Good.FindChild(earth, 'moon')

  InitObj(moon, 75, 60, 0, 0)
  InitObj(earth, 150, 12 * 60, 0, 0)
  InitObj(sun, 32, 16 * 60, 280, 170)
end
