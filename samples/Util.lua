
function PtInRect(x, y, left, top, right, bottom)
  if (left <= x and right > x and top <= y and bottom > y) then
    return true
  else
    return false
  end
end

function PtInObj(x, y, o)
  local ox, oy = Good.GetPos(o)
  local l, t, w, h = Good.GetDim(o)
  local sx, sy = Good.GetScale(o)
  return PtInRect(x, y, ox, oy, ox + math.abs(sx) * w, oy + math.abs(sy) * h)
end

function GenStrObj(parent, x, y, str, texture, charw, charh, color)
  if (nil == str) then
    return -1
  end
  local _parent = parent
  if (nil == parent) then
    _parent = -1
  end
  local dummy = Good.GenDummy(_parent)
  if (-1 == dummy) then
    return -1
  end
  local _x, _y = x, y
  if (nil == x) then
    _x = 0
  end
  if (nil == y) then
    _y = 0
  end
  Good.SetPos(dummy, _x, _y)
  local _tex = texture
  if (nil == _tex) then
    _tex = Resource.GetTexId('font')
  end
  local _cw, _ch = charw, charh
  if (nil == _cw) then
    _cw = 16
  end
  if (nil == _ch) then
    _ch = 32
  end
  local _color = color
  if (nil == _color) then
    _color = 0xffffffff
  end
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

function GenColorObj(parent, w, h, color, script)
  local _parent = parent
  if (nil == parent) then
    _parent = -1
  end
  local _script = script
  if (nil == script) then
    _script = ''
  end
  local o = Good.GenObj(_parent, -1, _script)
  if (-1 ~= o) then
    local _w, _h = w, h
    local _color = color
    if (nil == w) then
      _w = 16
    end
    if (nil == h) then
      _h = 16
    end
    if (nil == color) then
      _color = 0xffff0000
    end
    Good.SetDim(o, 0, 0, _w, _h)
    Good.SetBgColor(o, _color)
  end
  return o
end

function GenTexObj(parent, texture, w, h, srcx, srcy, script)
  local _parent = parent
  if (nil == parent) then
    _parent = -1
  end
  local _tex = texture
  if (nil == texture) then
    _tex = -1
  end
  local _script = script
  if (nil == script) then
    _script = ''
  end
  local o = Good.GenObj(_parent, _tex, _script)
  if (-1 ~= o) then
    local _w, _h = w, h
    if (nil == w) then
      _w = 16
    end
    if (nil == h) then
      _h = 16
    end
    local _srcx, _srcy = srcx, srcy
    if (nil == srcx) then
      _srcx = 0
    end
    if (nil == srcx) then
      _srcy = 0
    end
    Good.SetDim(o, _srcx, _srcy, _w, _h)
  end
  return o
end

function SetTextObjColor(msg, color)
  for i = 0, Good.GetChildCount(msg) - 1 do
    Good.SetBgColor(Good.GetChild(msg, i), color)
  end
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

function Lshift(x, by)                  -- Bitwise left shift.
  return x * 2 ^ by
end

function Rshift(x, by)                  -- Bitwise right shift.
  return math.floor(x / 2 ^ by)
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

Good.GetScreenPos = function(id)
  local x, y = Good.GetPos(id)
  local idLvl = Good.GetLevelId()
  while (id ~= idLvl) do
    id = Good.GetParent(id)
    if (-1 == id or idLvl == id) then
      break
    end
    local lx, ly = Good.GetPos(id)
    x = x + lx
    y = y + ly
  end
  return x, y
end