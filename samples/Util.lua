
function FillImage(canvas, x, y, tex, w, h)
  local cx, cy = Resource.GetTexSize(tex)
  for ay = 0, h, cy do
    for ax = 0, w, cx do
      Graphics.DrawImage(canvas, ax, ay, tex, 0, 0, cx, cy)
    end
  end
end

function GenColorObj(parent, w, h, color, script)
  local _parent = parent or -1
  local _script = script or ''
  local o = Good.GenObj(_parent, -1, _script)
  if (-1 ~= o) then
    local _w = w or 16
    local _h = h or 16
    Good.SetDim(o, 0, 0, _w, _h)
    local _color = color or 0xffff0000
    Good.SetBgColor(o, _color)
  end
  return o
end

function GenDotLineObj(parent, x1, y1, x2, y2, sz, gap, color)
  local len = math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))
  local delta = 1 / (len / gap)
  local t = 0
  while (true) do
    local o = GenColorObj(parent, sz, sz, color)
    Good.SetPos(o, Lerp(x1, x2, t), Lerp(y1, y2, t))
    t = t + delta
    if (1 <= t) then
      break
    end
  end
end

function GenStrObj(parent, x, y, str, texture, charw, charh, color)
  if (nil == str) then
    return -1
  end
  local _parent = parent or -1
  local dummy = Good.GenDummy(_parent)
  if (-1 == dummy) then
    return -1
  end
  local _x = x or 0
  local _y = y or 0
  Good.SetPos(dummy, _x, _y)
  local _tex = texture or Resource.GetTexId('font')
  local _cw = charw or 16
  local _ch = charh or 32
  local _color = color or 0xffffffff
  for i = 1, string.len(str) do
    local ch = string.byte(str, i) - 32
    local charx = _cw * math.floor(ch % 15)
    local chary = _ch * math.floor(ch / 15)
    local o = GenTexObj(dummy, _tex, _cw, _ch, charx, chary)
    Good.SetPos(o, (i - 1) * _cw, 0)
    Good.SetBgColor(o, _color)
  end
  return dummy
end

function GenTexObj(parent, texture, w, h, srcx, srcy, script)
  local _parent = parent or -1
  local _tex = texture or -1
  local _script = script or ''
  local o = Good.GenObj(_parent, _tex, _script)
  if (-1 ~= o) then
    local _w = w or 16
    local _h = h or 16
    local _srcx = srcx or 0
    local _srcy = srcy or 0
    Good.SetDim(o, _srcx, _srcy, _w, _h)
  end
  return o
end

function GetARgbFromColor(color)
  local hex = string.format('%08x', color)
  local a = string.sub(hex, 1, 2)
  local r = string.sub(hex, 3, 4)
  local g = string.sub(hex, 5, 6)
  local b = string.sub(hex, 7, 8)
  return tonumber(a, 16), tonumber(r, 16), tonumber(g, 16), tonumber(b, 16)
end

function GetColorFromARgb(A, R, G, B)
  local hex = string.format('%02X%02X%02X%02X', A, R, G, B)
  return tonumber(hex, 16)
end

function GetTextObjWidth(msg)
  local W = 0
  for i = 0, Good.GetChildCount(msg) - 1 do
    local o = Good.GetChild(msg, i)
    local l,t,w,h = Good.GetDim(o)
    W = W + w
  end
  return W
end

function Lerp(v0, v1, t)
  return (1 - t) * v0 + t * v1
end

function LerpARgb(c0, c1, t)
  local a,r,g,b = GetARgbFromColor(c0)
  local ba,br,bg,bb = GetARgbFromColor(c1)
  local na = Lerp(a, ba, t)
  local nr = Lerp(r, br, t)
  local ng = Lerp(g, bg, t)
  local nb = Lerp(b, bb, t)
  return GetColorFromARgb(na, nr, ng, nb)
end

function Lshift(x, by)                  -- Bitwise left shift.
  return x * 2 ^ by
end

function PtInObj(x, y, o, world)
  if (Good.INVISIBLE == Good.GetVisible(o)) then
    return false
  end
  local ox, oy
  if (nil ~= world) then
    ox, oy = Good.GetPos(o, world)
  else
    ox, oy = Good.GetPos(o)
  end
  local l, t, w, h = Good.GetDim(o)
  local sx, sy = Good.GetScale(o)
  local ax, ay = Good.GetAnchor(o)
  local sw, sh = math.abs(w * sx), math.abs(h * sy)
  ox = Lerp(ox, ox + (w - sw), ax)
  oy = Lerp(oy, oy + (h - sh), ay)
  return PtInRect(x, y, ox, oy, ox + math.abs(sx) * w, oy + math.abs(sy) * h)
end

function PtInRect(x, y, left, top, right, bottom)
  if (left <= x and right > x and top <= y and bottom > y) then
    return true
  else
    return false
  end
end

function RotSign(vx, vy, tx, ty)
  if (vy * tx > vx * ty) then
    return -1
  else
    return 1
  end
end

function Rshift(x, by)                  -- Bitwise right shift.
  return math.floor(x / 2 ^ by)
end

function ScaleToSize(o, width, height)
  local l, t, w, h = Good.GetDim(o)
  local sw, sh
  if (w == h) then
    sw = width / w
    sh = height / h
  elseif (w > h) then
    local s = width/w
    sw = s
    sh = s
  else
    local s = height/h
    sw = s
    sh = s
  end
  Good.SetScale(o, sw, sh)
  return w * sw, h * sh
end

function SetTextObjColor(msg, color)
  for i = 0, Good.GetChildCount(msg) - 1 do
    Good.SetBgColor(Good.GetChild(msg, i), color)
  end
end

function SetTopmost(o)
  Good.AddChild(-1, o)                  -- Change zorder to topmost.
end

Good.GetAlpha = function(id)
  return Rshift(Good.GetBgColor(id), 24)
end

Good.SetAlpha = function(id, a)
  a = math.floor(a)
  if (0 >= a) then
    a = 0
  end
  if (0xff <= a) then
    a = 0xff
  end
  local clr = Good.GetBgColor(id)
  local x = Lshift(Rshift(clr, 24), 24)
  local y = Lshift(a, 24)
  clr = clr - x + y
  Good.SetBgColor(id, clr)
end
