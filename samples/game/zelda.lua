local scrw, scrh = Good.GetWindowSize()
local scrolling, dir, offset
local MoveableTile = {121,122,123,145,146,147,169,170,171,221,230,281,305,329,330,349,350,377}

function KillAndDynaGenObj(idLvl)
  local nc = Good.GetChildCount(idLvl)
  for i = nc,0,-1 do
    local idChild = Good.GetChild(idLvl, i)
    if (Good.SPRITE == Good.GetType(idChild) and 14 ~= idChild) then
      Good.KillObj(idChild)
    end
  end

  local x,y = Good.GetPos(idLvl)
  for i = 0,9 do
    for j = 0,7 do
      local lx,ly = x + 16 * i, y + 16 * j
      local tile = Resource.GetTileByPos(1, lx, ly)
      local spr = -1
      if (377 == tile) then
        spr = 3
      elseif (401 == tile) then
        spr = 7
      elseif (425 == tile) then
        spr = 8
      elseif (449 == tile) then
        spr = 9
      end
      if (-1 ~= spr) then
        Good.SetPos(Good.GenObj(-1, spr), lx, ly)
      end
    end
  end

  Good.AddChild(idLvl, 14) -- change link's zorder to topmost
end

function IsMoveable(x, y)
  local tile = Resource.GetTileByPos(1, x, y)
  for i,v in ipairs(MoveableTile) do
    if (tile == v) then
      return true
    end
  end
  return false
end

Link = {}

Link.OnStep = function(param)
  local id = param._id
  local x,y = Good.GetPos(id)

  if (scrolling) then
    local spd1, spd2 = 0.5, 0.5
    if (Input.LEFT == dir) then
      x = x - spd1
    elseif (Input.RIGHT == dir) then
      x = x + spd1
    elseif (Input.DOWN == dir) then
      y = y + spd2
    elseif (Input.UP == dir) then
      y = y - spd2
    end
    Good.SetPos(id, x, y)
    return
  end

  local ox,oy = x,y
  local spr
  local spd = 0.8

  if (Input.IsKeyDown(Input.LEFT)) then
    x = x - spd
    spr = 10
  elseif (Input.IsKeyDown(Input.RIGHT)) then
    x = x + spd
    spr = 13
  end
  if (Input.IsKeyDown(Input.DOWN)) then
    y = y + spd
    spr = 11
  elseif (Input.IsKeyDown(Input.UP)) then
    y = y - spd
    spr = 12
  end

  if (ox == x and oy == y) then
    return
  end

  Good.SetSpriteId(id, spr)

  local cx,cy = x + 8, y + 10
  if (Input.IsKeyDown(Input.LEFT)) then
    cx = x
  elseif (Input.IsKeyDown(Input.RIGHT)) then
    cx = x + 16
  end
  if (Input.IsKeyDown(Input.DOWN)) then
    cy = y + 16
  end
  if (not IsMoveable(cx, cy)) then
    return
  end

  local idLvl = Good.GetParent(id)
  local px,py = Good.GetPos(idLvl)

  dir = -1
  if (px > x) then
    dir = Input.LEFT
    x = x - 0.5
  elseif (px + scrw < x + 16) then
    dir = Input.RIGHT
    x = x + 0.5
  end
  if (py > y) then
    dir = Input.UP
    y = y - 0.5
  elseif (py + scrh < y + 16) then
    dir = Input.DOWN
    y = y + 0.5
  end

  if (-1 ~= dir) then
    offset = 0
    scrolling = true
  end

  Good.SetPos(id, x, y)
end

Game = {}

Game.OnCreate = function(param)
  local idLvl = param._id

  scrolling = false
  Good.SetPos(idLvl, 320, 1280)
  KillAndDynaGenObj(idLvl)
end

Game.OnStep = function(param)
  local idLvl = param._id

  if (not scrolling) then
    return
  end

  local x,y = Good.GetPos(idLvl)
  local spd1, spd2 = scrw/32, scrh/32   -- In 0.5 second.
  if (Input.LEFT == dir) then
    x = x - spd1
    offset = offset + spd1
  elseif (Input.RIGHT == dir) then
    x = x + spd1
    offset = offset + spd1
  elseif (Input.DOWN == dir) then
    y = y + spd2
    offset = offset + spd2
  elseif (Input.UP == dir) then
    y = y - spd2
    offset = offset + spd2
  end
  Good.SetPos(idLvl, x, y)

  if (Input.LEFT == dir or Input.RIGHT == dir) then
    if (scrw <= offset) then
      scrolling = false
    end
  elseif (Input.DOWN == dir or Input.UP == dir) then
    if (scrh <= offset) then
      scrolling = false
    end
  end

  if (not scrolling) then
    KillAndDynaGenObj(idLvl)
  end
end
