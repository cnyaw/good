
Test = {}

Test.OnCreate = function(param)
  param.scale = 50
  Good.SetAnchor(param._id, 0.5, 0.5)
end

Test.OnStep = function(param)
  local id = param._id

  -- rotate
  local rot = Good.GetRot(id)
  rot = rot + 1
  if (360 < rot) then
    rot = 0
  end
  Good.SetRot(id, rot)

  -- scale
  param.scale = param.scale + 0.5
  if (100 <= param.scale) then
    param.scale = 0
  end
  local scale = 2 * math.sin(math.pi * param.scale/100)
  Good.SetScale(id, scale, scale)
end
