local SW,SH = Good.GetWindowSize()

Logo = {}

Logo.OnCreate = function(param)
  param.t = 0
  local x,y,w,h = Good.GetDim(4)
  Good.SetPos(4, (SW - w)/2 - 30, (SH - h)/2)
  x,y = Good.GetPos(4)
  local x2,y2,w2,h2 = Good.GetDim(2)
  Good.SetPos(2, x + w, y - 24)
end

Logo.OnStep = function(param)
  param.t = param.t + 1
  if (40 <= param.t and 80 > param.t) then
    SetAlpha(4, param.t, 40)
  end
  if (100 <= param.t and 140 > param.t) then
    SetAlpha(2, param.t, 100)
  end
  if (240 < param.t) then
    Good.Exit()
  end
end

function SetAlpha(id, t, st)
    Good.SetVisible(id, Good.VISIBLE)
    local dt = (t - st) / 40
    alpha = math.floor(256 * dt)
    Good.SetBgColor(id, 0xffffff + alpha * 0x1000000)
end
