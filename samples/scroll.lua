
Level = {}

scroll = function(id, offset)
  local x,y = Good.GetPos(id)
  x = x - offset
  Good.SetPos(id, x,y)
end

Level.OnCreate = function(param)
  local id = param._id
  param.bg = Good.FindChild(id, 'bg')
  param.bg2 = Good.FindChild(id, 'bg2')
  param.sun = Good.FindChild(id, 'sun')
end

Level.OnStep = function(param)
  scroll(param.bg, 0.5)
  scroll(param.bg2, 0.25)
  scroll(param.sun, 0.15)
end
