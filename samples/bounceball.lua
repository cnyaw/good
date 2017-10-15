local sndHit = Resource.GetSoundId('hit')

Level = {}

Level.OnCreate = function(param)
  param.ball = Good.FindChild(param._id, 'ball')  
  param.speedx = 4
  param.speedy = 4
end

Level.OnStep = function(param)
  local W,H = Good.GetWindowSize()
  local l,t,w,h = Good.GetDim(param.ball)

  local x,y = Good.GetPos(param.ball)
  x = x + param.speedx
  y = y + param.speedy
  
  if (0 > x) then
    x = 0
    param.speedx = -param.speedx
    Sound.PlaySound(sndHit)
  elseif (W - w <= x) then
    x = W - w
    param.speedx = -param.speedx
    Sound.PlaySound(sndHit)
  end
  
  if (0 > y) then
    y = 0
    param.speedy = -param.speedy
    Sound.PlaySound(sndHit)
  elseif (H - h <= y) then
    y = H - h
    param.speedy = -param.speedy
    Sound.PlaySound(sndHit)
  end
  
  Good.SetPos(param.ball, x,y)
end
