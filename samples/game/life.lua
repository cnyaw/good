
local TIMER = 10
local MAPW,MAPH = 16,16

local Bound = {-MAPW-1, -MAPW, -MAPW+1,-1,1,MAPW-1,MAPW,MAPW+1}
local OffsetX = {-1, 0, 1, -1, 1, -1, 0, 1}

Cells = {}

function numNeihbors(idxCell)
  local n = 0
  for i=1,8 do
    local x = (idxCell % MAPW) + OffsetX[i]
    if (0 <= x and MAPW > x) then
      local idx = idxCell + Bound[i]
      if (0 <= idx and MAPW*MAPH > idx and Cells[idx + 1]) then
        n = n + 1
      end
    end
  end
  return n
end

function goLife()
  for i=0,255 do
    local n = numNeihbors(i)
    if (not Cells[i + 1]) then -- dead
      if (3 == n) then
        local c,r = i % MAPW, i / MAPW
        local o = Good.PickObj(16 * c, 16 * r, Good.COLBG)
        fillObj(i + 1, o)
      end
    else -- alive
      if (2 > n or 3 < n) then
        local c,r = i % MAPW, i / MAPW
        local o = Good.PickObj(16 * c, 16 * r, Good.COLBG)
        clearObj(o)
      end
    end
  end

  for i=0,255 do
    local c,r = i % MAPW, i / MAPW
    local o = Good.PickObj(16 * c, 16 * r, Good.COLBG)
    Cells[i + 1] = isFillObj(o)
  end
end

function flip(i, o)
  if (isFill(i)) then
    clear(i, o)
  else
    fill(i, o)
  end
end

function isFillObj(o)
  local c = Good.GetBgColor(o)
  if (0xff000000 == c) then
    return false
  else
    return true
  end
end

function isFill(i)
  return Cells[i]
end

function fillObj(i, o)
  local v = i - 1
  local c,r = v % 16, v / 16
  local c = 4278190080 + (255 - c * r) + 16 * r * 256 + (15 - c) * 16 * 65536
  Good.SetBgColor(o, c)
end

function clearObj(o)
  Good.SetBgColor(o, 0xff000000)
end

function fill(i, o)
  fillObj(i, o)
  Cells[i] = true
end

function clear(i, o)
  clearObj(o)
  Cells[i] = false
end

Game = {}

Game.OnCreate = function(param)
  local id = param._id

  for i=0, 255 do
    local o = GenColorObj(id)
    Good.SetPos(o, 16 * (i % 16), 16 * math.floor(i / 16))
    table.insert(Cells, i + 1, false)
    clear(i + 1, o)
  end

  param.hit = -1
  param.pause = false
  param.timer = TIMER
end

Game.OnStep = function(param)
  local id = param._id

  if (Input.IsKeyPressed(Input.BTN_A)) then
    param.pause = not param.pause
  elseif (Input.IsKeyPressed(Input.BTN_B)) then
    local nc = Good.GetChildCount(id)
    for i=0,nc-1 do
      local o = Good.GetChild(id, i)
      clear(i + 1, o)
    end
  end

  if (Input.IsKeyDown(Input.LBUTTON)) then
    local x,y = Input.GetMousePos()
    local hit = Good.PickObj(x, y, Good.COLBG)
    if (hit ~= param.hit) then
      local hx,hy = Good.GetPos(hit)
      flip(1 + (hx / 16) + 16 * (hy / 16), hit)
      param.hit = hit
    end
  else
    param.hit = -1
  end

  if (not param.pause) then
    param.timer = param.timer - 1
    if (0 == param.timer) then
      goLife()
      param.timer = TIMER
    end
  end
end
