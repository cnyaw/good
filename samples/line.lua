local DOT_SZ = 5
local DOT_GAP = 8
local DOT_COLOR = 0xffff0000

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

function BeginDrag(param)
  param.mouse_down = true
  param.x, param.y = Input.GetMousePos()
  Good.KillAllChild(param.dummy)
end

function Dragging(param)
  local x, y = Input.GetMousePos()
  if (x ~= param.x or y ~= param.y) then
    Good.KillAllChild(param.dummy)
    GenDotLineObj(param.dummy, x, y, param.x, param.y, DOT_SZ, DOT_GAP, DOT_COLOR)
  end
end

function EndDrag(param)
  Dragging(param)
  param.mouse_down = false
end

Line = {}

Line.OnCreate = function(param)
  local dummy = Good.GenDummy(-1)
  GenDotLineObj(dummy, 44, 22, 333, 222, DOT_SZ, DOT_GAP, DOT_COLOR)
  param.dummy = dummy
  param.mouse_down = false
end

Line.OnStep = function(param)
  if (Input.IsKeyDown(Input.LBUTTON)) then
    if (not param.mouse_down) then
      BeginDrag(param)
    else
      Dragging(param)
    end
  else
    if (param.mouse_down) then
      EndDrag(param)
    end
  end
end
