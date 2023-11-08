
local Color = {4,6,8,10,12,14}
local next1,next2,next3,cover = 21,22,23,20

BaseBlock = {}

BaseBlock.OnStep = function(param)
  if (nil == param.init) then
    param.init = true
    param.nextPlay = 300 + math.random(300)
    Good.StopAnim(param._id)
    return
  end

  param.nextPlay = param.nextPlay - 1
  if (0 < param.nextPlay) then
    return
  end
  param.nextPlay = 300 + math.random(300)

  Good.PlayAnim(param._id)
end

function randColor()
  return Color[math.random(#Color)]
end

function newNext()
  local a,b,c = Good.GetSpriteId(next1), Good.GetSpriteId(next2), Good.GetSpriteId(next3)
  Good.SetSpriteId(next1, randColor())
  Good.SetSpriteId(next2, randColor())
  Good.SetSpriteId(next3, randColor())
  return a,b,c
end

function moveBar(param, ox, oy)
  for i = 1,3 do
    local x,y = Good.GetPos(param.bar[i])
    Good.SetPos(param.bar[i], x + ox, y + oy)
  end
end

function checkFreeze(param, x, y)
  if (160 <= y or -1 ~= Good.PickObj(x, y + 16, Good.SPRITE)) then
    param.timer = 15
    param.stage = stageFreeze
    return true
  end

  return false
end

Level = {}

Level.OnCreate = function(param)
  param.gameover = false
  param.timer = 0
  param.stage = stageOver
  math.randomseed(os.clock())
end

Level.OnStep = function(param)
  param.stage(param)
end

function stageFall(param)
  local a,b,c = newNext()
  param.bar = {Good.GenObj(-1,a),Good.GenObj(-1,b),Good.GenObj(-1,c)}

  for i = 1,3 do
    Good.SetPos(param.bar[i], 64, 32 + 16 * (i - 4))
    Good.SetScript(param.bar[i], 'BaseBlock')
  end

  Good.AddChild(param._id, cover) -- make cover top most

  param.stage = stageFalling
end

local CROSS_DIR = {-1, 0, -2, 0, 1, 0, 2, 0, 0, 1, 0, 2, 0, -1, 0, -2, -1, 1, -2, 2, 1, -1, 2, -2, -1, -1, -2, -2, 1, 1, 2, 2}

function checkClearCross(x,y, spr)
  local r,c, match, hit
  for i = 0,3 do
    match = 1
    r,c = x + 16 * CROSS_DIR[8 * i + 1], y + 16 * CROSS_DIR[8 * i + 2]
    hit = Good.PickObj(r, c, Good.SPRITE)
    if (-1 ~= hit and (Good.GetSpriteId(hit) == spr or Good.GetSpriteId(hit) == spr + 1)) then
      match = match + 1
      r,c = x + 16 * CROSS_DIR[8 * i + 3], y + 16 * CROSS_DIR[8 * i + 4]
      hit = Good.PickObj(r, c, Good.SPRITE)
      if (-1 ~= hit and (Good.GetSpriteId(hit) == spr or Good.GetSpriteId(hit) == spr + 1)) then
        return true
      end
    end

    r,c = x + 16 * CROSS_DIR[8 * i + 5], y + 16 * CROSS_DIR[8 * i + 6]
    hit = Good.PickObj(r, c, Good.SPRITE)
    if (-1 ~= hit and (Good.GetSpriteId(hit) == spr or Good.GetSpriteId(hit) == spr + 1)) then
      match = match + 1
      if (2 < match) then
        return true
      end
      r,c = x + 16 * CROSS_DIR[8 * i + 7], y + 16 * CROSS_DIR[8 * i + 8]
      hit = Good.PickObj(r, c, Good.SPRITE)
      if (-1 ~= hit and (Good.GetSpriteId(hit) == spr or Good.GetSpriteId(hit) == spr + 1)) then
        return true
      end
    end
  end

  return false
end

function stageClear(param)
  param.timer = param.timer - 1
  if (0 < param.timer) then
    return
  end

  for i = 0,5 do
    local x = 32 + 16 * i
    for j = 0,8 do
      local y = 32 + 16 * j
      local hit = Good.PickObj(x, y, Good.SPRITE)
      if (-1 ~= hit and nil ~= Good.GetParam(hit).markClear) then
        Good.KillObj(hit)
      end
    end
  end

  param.stage = stageArrange
end

function stageArrange(param)
  local nArrange = false
  for i = 0,5 do
    local x = 32 + 16 * i
    for j = 8,0,-1 do
      local y = 32 + 16 * j
      local hit = Good.PickObj(x, y - 1, Good.SPRITE)
      if (-1 ~= hit) then
        x,y = Good.GetPos(hit)
        if (-1 == Good.PickObj(x, y + 16, Good.SPRITE)) then
          local x2,y2 = Good.GetPos(hit)
          Good.SetPos(hit, x2, y2 + 4)
          nArrange = true
        end
      end
    end
  end

  if (nArrange) then
    return
  end

  param.stage = stageCheck
end

function stageCheck(param)
  local nClear = 0
  for i = 0,5 do
    local x = 32 + 16 * i
    for j = 0,8 do
      local y = 32 + 16 * j
      local hit = Good.PickObj(x, y, Good.SPRITE)
      if (-1 ~= hit) then
        local spr = Good.GetSpriteId(hit)
        if (checkClearCross(x,y, spr)) then
          Good.SetSpriteId(hit, spr + 1)
          Good.GetParam(hit).markClear = true
          Good.PlayAnim(hit)
          nClear = nClear + 1
        end
      end
    end
  end

  if (0 ~= nClear) then
    param.timer = 35
    param.stage = stageClear
    return
  end

  param.stage = stageFall
end

function stageFreeze(param)
  local x,y = Good.GetPos(param.bar[3])
  param.timer = param.timer - 1
  if (0 >= param.timer) then
    if (160 <= y or -1 ~= Good.PickObj(x, y + 16, Good.SPRITE)) then
      if (-1 ~= Good.PickObj(x, 16, Good.SPRITE)) then
        local nc = Good.GetChildCount(param._id)
        for i = 4,nc - 2 do
          local id = Good.GetChild(param._id, i)
          Good.SetSpriteId(id, Good.GetSpriteId(id) + 1)
          Good.PlayAnim(id)
        end
        param.timer = 35
        param.stage = stageOver
      else
        param.stage = stageCheck
      end
    else
      param.stage = stageFalling
    end
  elseif (Input.IsKeyPushed(Input.LEFT)) then
    x = x - 16
    if (32 <= x and -1 == Good.PickObj(x, y, Good.SPRITE)) then
      moveBar(param, -16, 0)
      param.timer = 15
    end
  elseif (Input.IsKeyPushed(Input.RIGHT)) then
    x = x + 16
    if (128 > x and -1 == Good.PickObj(x, y, Good.SPRITE)) then
      moveBar(param, 16, 0)
      param.timer = 15
    end
  end
end

function stageFallDirect(param)
  local x,y = Good.GetPos(param.bar[3])
  if (checkFreeze(param, x,y)) then
    return
  end

  moveBar(param, 0, 8)
end

function stageFalling(param)
  local x,y = Good.GetPos(param.bar[3])
  if (checkFreeze(param, x,y)) then
    return
  end

  if (Input.IsKeyPushed(Input.UP + Input.BTN_A)) then -- rotate
    local spr1 = Good.GetSpriteId(param.bar[1])
    Good.SetSpriteId(param.bar[1], Good.GetSpriteId(param.bar[2]))
    Good.SetSpriteId(param.bar[2], Good.GetSpriteId(param.bar[3]))
    Good.SetSpriteId(param.bar[3], spr1)
  elseif (Input.IsKeyPushed(Input.LEFT)) then
    x = x - 16
    if (32 <= x and -1 == Good.PickObj(x, y + 15, Good.SPRITE)) then
      moveBar(param, -16, 0)
    end
  elseif (Input.IsKeyPushed(Input.RIGHT)) then
    x = x + 16
    if (128 > x and -1 == Good.PickObj(x, y + 15, Good.SPRITE)) then
      moveBar(param, 16, 0)
    end
  elseif (Input.IsKeyPushed(Input.DOWN)) then
    moveBar(param, 0, 16 - (math.floor(y) % 16))
    param.stage = stageFallDirect
    return
  end

  moveBar(param, 0, 0.5)
end

function stageOver(param)
  param.timer = param.timer - 1
  if (0 < param.timer) then
    return
  end

  local nc = Good.GetChildCount(param._id)
  for i = nc - 2,4,-1 do -- skip bk/cover/next(3)
    Good.KillObj(Good.GetChild(param._id, i))
  end

  newNext()
  param.stage = stageFall
end
