local w, h = Resource.GetMapSize(2)
local tw, th = Resource.GetTileSize(2)

function GenFlower(param)
  while true do
    local x = math.random(w - 2)
    local y = math.random(h - 2)
    local pos = x + w * y
    local found = false
    for i = 0, w * h - 1 do
      if (param.o[i][1] == pos) then
        found = true
        break
      end
    end
    if (not found) then
      Good.SetPos(param.flower[0], tw * x, th * y)
      param.flower[1] = pos
      return
    end
  end
end

Game = {}

Game.OnCreate = function(param)
  param.active = false
  param.gameover = false

  param.o = {}
  for i = 0, w * h do -- +1 for dummy
    param.o[i] = {}
    param.o[i][0] = 0 -- id
    param.o[i][1] = 0 -- pos
  end

  local head = Good.FindChild(param._id, 'head')
  local x, y = Good.GetPos(head)
  param.o[0][0] = head
  param.o[0][1] = math.floor(x / tw) + w * math.floor(y / th)

  local spr = Resource.GetSpriteId('flower')
  local flower = Good.GenObj(-1, spr)
  param.flower = {}
  param.flower[0] = flower
  GenFlower(param)
end

Game.OnStep = function(param)
  -- gameover?
  if (param.gameover) then
    if (0 ~= param.o[param.i][0]) then
      param.count = param.count + 1
      if (5 ~= param.count) then
        return
      end
      param.count = 0
      local spr = Resource.GetSpriteId('bomb')
      Good.SetSpriteId(param.o[param.i][0], spr)
      param.i = param.i + 1
    end
    return
  end

  -- handle arrow key
  local dir = -1
  if (Input.IsKeyPushed(Input.LEFT)) then
    dir = Input.LEFT
  elseif (Input.IsKeyPushed(Input.RIGHT)) then
    dir = Input.RIGHT
  elseif (Input.IsKeyPushed(Input.UP)) then
    dir = Input.UP
  elseif (Input.IsKeyPushed(Input.DOWN)) then
    dir = Input.DOWN
  end

  -- save move direction
  if (-1 ~= dir) then
    param.dir = dir
    if (not param.active) then
      param.active = true
      param.count = 0
    end
  end

  if (not param.active) then
    return
  end

  -- time to move?
  param.count = param.count + 1
  if (30 ~= param.count) then
    return
  end
  param.count = 0

  -- get next head pos
  local head = param.o[0][0]
  local headpos = param.o[0][1]
  local headx = math.floor(headpos % w)
  local heady = math.floor(headpos / w)

  local spr
  if (Input.LEFT == param.dir) then
    headx = headx - 1
    spr = Resource.GetSpriteId('left')
  elseif (Input.RIGHT == param.dir) then
    headx = headx + 1
    spr = Resource.GetSpriteId('right')
  elseif (Input.UP == param.dir) then
    heady = heady - 1
    spr = Resource.GetSpriteId('up')
  elseif (Input.DOWN == param.dir) then
    heady = heady + 1
    spr = Resource.GetSpriteId('down')
  end

  -- check gameover
  if (57 ~= Resource.GetTileByPos(2, tw * headx, th * heady)) then -- hit wall
    param.gameover = true
    param.i = 0
    return
  end

  headpos = headx + w * heady -- hit sel body
  for i = 1, w * h - 1 do
    if (param.o[i][1] == headpos) then
      param.gameover = true
      param.i = 0
      return
    end
  end

  -- move snake body
  for j = w * h - 1, 1, -1 do
    if (0 ~= param.o[j][0]) then
      for i = j, 1, -1 do
        local o = param.o[i][0]
        local op = param.o[i - 1][0]
        Good.SetPos(o, Good.GetPos(op))
        Good.SetSpriteId(o, Good.GetSpriteId(op))
        param.o[i][1] = param.o[i - 1][1]
      end
      break
    end
  end

  -- move head
  Good.SetPos(head, tw * headx, th * heady)
  Good.SetSpriteId(head, spr)
  param.o[0][1] = headpos

  -- grow body
  if (headx + w * heady == param.flower[1]) then
    GenFlower(param)
    for i = w * h - 1, 0, -1 do
      if (0 ~= param.o[i][0]) then
        local tailpos = param.o[i][1]
        local tailx = math.floor(tailpos % w)
        local taily = math.floor(tailpos / w)
        local spr = Good.GetSpriteId(param.o[i][0])
        local o = Good.GenObj(-1, spr)
        if (Resource.GetSpriteId('left') == spr) then
          tailx = tailx + 1
        elseif (Resource.GetSpriteId('right') == spr) then
          tailx = tailx - 1
        elseif (Resource.GetSpriteId('up') == spr) then
          taily = taily + 1
        elseif (Resource.GetSpriteId('down') == spr) then
          taily = taily - 1
        end
        Good.SetPos(o, tw * tailx, th * taily)
        param.o[i + 1][0] = o
        param.o[i + 1][1] = tailx + w * taily
        break
      end
    end
  end
end
