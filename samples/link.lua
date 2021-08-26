
math.randomseed(os.time())

local MAP_W = 20
local MAP_H = 12
local TILE_W = 24
local TILE_H = 34

local mx, my = 0, 0                     -- Top-Left cornor of map.

Puzzle = {}

Game = {}

Game.OnCreate = function(param)

  -- Get puzzle mask tile map.

  local m = Good.FindChild(param._id, 'map')
  Good.SetVisible(m, Good.INVISIBLE)

  mx, my = Good.GetPos(m)

  local resm = Good.GetMapId(m)

  -- Calc valid tiles of the map.

  param.p = {}                          -- The game state.
  param.o = {}                          -- The tile objects.

  param.timer = 0
  param.link = {}

  param.c = 0                           -- Total cards need to clear.
  param.p1 = -1
  param.p2 = -1

  param.level = Good.GetLevelId(param._id)

  local MaxCard = 0

  local i = 0
  for y = 0, MAP_H - 1 do
    for x = 0, MAP_W - 1 do
      local c = Resource.GetTileByPos(resm, TILE_W * x, TILE_H * y)
      if (0 ~= c) then
        MaxCard = c
      end
      if (0 ~= c) then
        param.c = param.c + 1
        param.p[i] = 1
      else
        param.p[i] = -1
      end
      param.o[i] = nil
      i = i + 1
    end
  end

  if (1 == (param.c % 2)) then
    Good.Trace('Tile mask should be times of 2!')
    return
  end

  -- Init and gen puzzle.

  Cards = {}

  for i = 0, math.floor(param.c/2) - 1 do
    local t = math.random(MaxCard) - 1
    Cards[2 * i + 0] = t
    Cards[2 * i + 1] = t
  end

  -- Random suffule.

  for i = 0, param.c - 1 do
    local r = math.random(i + 1) - 1
    local n = Cards[r]
    Cards[r] = Cards[i]
    Cards[i] = n
  end

  -- Gen puzzle map.

  i = 0
  local iCards = 0
  for y = 0, MAP_H - 1 do
    local ay = TILE_H * y
    for x = 0, MAP_W - 1 do
      local ax = TILE_W * x
      if (1 == param.p[i]) then
        local p = Cards[iCards]
        param.p[i] = p
        local o = GenTexObj(param._id, 1, TILE_W, TILE_H, TILE_W * (p % 9), TILE_H * math.floor(p / 9))
        Good.SetPos(o, mx + ax, my + ay)
        param.o[i] = o
        iCards = iCards + 1
      end
      i = i + 1
    end
  end

  -- Gen selection mask.

  local o = GenColorObj(param._id, TILE_W, TILE_H, 0xaaff0000)
  Good.SetPos(o, mx, my)
  Good.SetVisible(o, Good.INVISIBLE)
  param.s = o

end

function isFlowVert(param, c, r1, r2)

  if (r1 == r2) then
    return true
  end

  if (r1 > r2) then
    local tmp = r1
    r1 = r2
    r2 = tmp
  end

  local r
  for r = r1, r2 do
    local idx = c + r * MAP_W
    if (-1 ~= param.p[idx] and idx ~= param.p1 and idx ~= param.p2) then
      return false
    end
  end

  return true
end

function isFlowHorz(param, r, c1, c2)

  if (c1 == c2) then
    return true
  end

  if (c1 > c2) then
    local tmp = c1
    c1 = c2
    c2 = tmp
  end

  local c
  for c = c1, c2 do
    local idx = c + r * MAP_W
    if (-1 ~= param.p[idx] and idx ~= param.p1 and idx ~= param.p2) then
      return false
    end
  end

  return true
end

function CheckLink(param)

  if (-1 == param.p2) then
    return false
  end

  local c1 = param.p1 % MAP_W
  local r1 = math.floor(param.p1 / MAP_W)
  local c2 = param.p2 % MAP_W
  local r2 = math.floor(param.p2 / MAP_W)

  -- Parallel row condition.

  local path = nil                      -- Shortest path.
  local r
  for r = 0, MAP_H - 1 do
    if (isFlowVert(param, c1, r1, r) and
        isFlowVert(param, c2, r, r2) and
        isFlowHorz(param, r, c1, c2)) then
      local p = math.abs(c1 - c2) + math.abs(r1 - r) + math.abs(r - r2)
      if (nil == path or p < path) then
        param.link[0] = param.p1
        param.link[1] = c1 + MAP_W * r
        param.link[2] = c2 + MAP_W * r
        param.link[3] = param.p2
        path = p
      end
    end
  end

  -- Parallel column condition.

  local c
  for c = 0, MAP_W - 1 do
    if (isFlowHorz(param, r1, c1, c) and
        isFlowHorz(param, r2, c, c2) and
        isFlowVert(param, c, r1, r2)) then
      local p = math.abs(r1 - r2) + math.abs(c1 - c) + math.abs(c - c2)
      if (nil == path or p < path) then
        param.link[0] = param.p1
        param.link[1] = c + MAP_W * r1
        param.link[2] = c + MAP_W * r2
        param.link[3] = param.p2
        path = p
      end
    end
  end

  if (nil ~= path) then
    return true
  end

  return false
end

function GenLinkObj(p1, p2)
  if (p1 == p2) then
    return -1
  end
  local c1 = p1 % MAP_W
  local r1 = math.floor(p1 / MAP_W)
  local c2 = p2 % MAP_W
  local r2 = math.floor(p2 / MAP_W)
  local c, r = c1, r1
  local o
  if (c1 == c2) then
    o = GenColorObj(-1, 2, TILE_H * math.abs(r1 - r2), 0xffff0000)
    if (r1 > r2) then
      r = r2
    end
  else
    o = GenColorObj(-1, TILE_W * math.abs(c1 - c2), 2, 0xffff0000)
    if (c1 > c2) then
      c = c2
    end
  end
  Good.SetPos(o, mx + TILE_W * c + TILE_W/2, my + TILE_H * r + TILE_H/2)
  return o
end

function HandleKey(param, dx, dy)
  if (nil == param.cur) then
    local o = GenTexObj(param._id, 1, TILE_W, TILE_H, 192, 136)
    Good.SetPos(o, mx, my)
    param.cur = o
  end
  local x, y = Good.GetPos(param.cur)
  x = x + dx
  y = y + dy
  Good.SetPos(param.cur, x, y)
end

Game.OnStep = function(param)

  -- Is game complete?

  if (0 == param.c) then
    if (Input.IsKeyPushed(Input.BTN_A + Input.LBUTTON)) then
      local level = Resource.GetNextLevelId(param.level)
      if (0 < level) then
        Good.GenObj(-1, level)
      end
    end
    return
  end

  -- Do clear link.

  if (0 < param.timer) then
    param.timer = param.timer - 1
    if (0 == param.timer) then
      Good.KillObj(param.o[param.p1])
      Good.KillObj(param.o[param.p2])
      for i = 0,2 do
        if (nil ~= param.ol[i]) then
          Good.KillObj(param.ol[i])
        end
      end
      param.p[param.p1] = -1
      param.p[param.p2] = -1
      param.p1 = -1
      param.p2 = -1
      param.link = {}
      param.c = param.c - 2
      Good.SetVisible(param.s, Good.INVISIBLE)

      -- Level clear?

      if (0 == param.c) then
        local level = Resource.GetNextLevelId(param.level)
        if (0 < level) then
          GenStrObj(-1, 100, 100, 'Click to start next level')
        else
          GenStrObj(-1, 100, 100, 'All level clear')
        end
      end
    end
  end

  -- Handle key down.

  local kdown = false
  if (Input.IsKeyPushed(Input.UP)) then
    HandleKey(param, 0, -TILE_H)
  elseif (Input.IsKeyPushed(Input.DOWN)) then
    HandleKey(param, 0, TILE_H)
  elseif (Input.IsKeyPushed(Input.LEFT)) then
    HandleKey(param, -TILE_W, 0)
  elseif (Input.IsKeyPushed(Input.RIGHT)) then
    HandleKey(param, TILE_W, 0)
  elseif (Input.IsKeyPushed(Input.BTN_A + Input.RETURN)) then
    HandleKey(param, 0, 0)
    kdown = true
  end

  -- Handle mouse down.

  if (kdown or Input.IsKeyPushed(Input.LBUTTON)) then

    -- Is mouse click in the puzzle?

    local x, y
    if (kdown) then
      x, y = Good.GetPos(param.cur)
    else
      x, y = Input.GetMousePos()
      if (nil ~= param.cur) then
        Good.KillObj(param.cur)
        param.cur = nil
      end
    end

    if (not PtInRect(x, y, mx, my, mx + TILE_W * MAP_W, my + TILE_H * MAP_H)) then
      return
    end

    -- Check click tile.

    local c = math.floor((x - mx) / TILE_W)
    local r = math.floor((y - my) / TILE_H)
    local pi = c + r * MAP_W
    if (-1 == param.p[pi]) then
      return
    end

    -- Check and update first and second selection tile.

    if (param.p1 == pi) then
      return
    elseif (-1 == param.p1) then
      param.p1 = pi
      Good.SetPos(param.s, mx + TILE_W * c, my + TILE_H * r)
      Good.SetVisible(param.s, Good.VISIBLE)
    elseif (-1 == param.p2) then
      if (param.p[param.p1] == param.p[pi]) then
        param.p2 = pi
      else
        param.p1 = pi
        Good.SetPos(param.s, mx + TILE_W * c, my + TILE_H * r)
        Good.SetVisible(param.s, Good.VISIBLE)
      end
    end

    if (-1 == param.p1 or -1 == param.p2) then
      return
    end

    -- Check link.

    if (not CheckLink(param)) then
      param.p1 = pi
      param.p2 = -1
      Good.SetPos(param.s, mx + TILE_W * c, my + TILE_H * r)
      Good.SetVisible(param.s, Good.VISIBLE)
      return
    end

    -- Clear link.

    param.timer = 10

    param.ol = {}
    if (param.link[1] ~= param.link[2]) then
      param.ol[0] = GenLinkObj(param.link[0], param.link[1])
      param.ol[1] = GenLinkObj(param.link[1], param.link[2])
      param.ol[2] = GenLinkObj(param.link[2], param.link[3])
    else
      param.ol[0] = GenLinkObj(param.link[0], param.link[3])
    end
  end
end
