
local SCR_W, SCR_H = Good.GetWindowSize()
local hit_snd = Resource.GetSoundId('hit')
local bgm = Resource.GetSoundId('bgm')
local Colors = {0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xffff00ff, 0xff00ffff, 0xffffffff, 0xff800000, 0xff008000, 0xff000080, 0xff808000, 0xff008080, 0xff800080, 0xff808080}

local spdxBall = 4
local spdyBall = 4
local spdPaddle = 5.5
local deltax = 0
local TOUCH_AREA_W = 60

Game = {}

releaseAll = function(param)
  local id = param._id
  local nc = Good.GetChildCount(id)
  for i = nc-1,0,-1 do
    local idChild = Good.GetChild(id, i)
    Good.KillObj(idChild)
  end
end

newGame = function(param)
  releaseAll(param)
  param.i = 0
  param.r = 0
  param.c = 0
  param.timer = 0
  param.stage = createWall
end

createWall = function(param)
  -- check time
  param.timer = param.timer + 1
  if (1 ~= param.timer) then
    return
  end
  param.timer = 0

  -- create brick
  local b = GenColorObj(param._id, param.bw, param.bh, Colors[math.random(#Colors)])
  Good.SetPos(b, param.c * param.bw, param.r * param.bh)

  -- next brick
  param.r = param.r - 1
  param.c = param.c + 1
  if (0 > param.r or param.w == param.c) then
    param.i = param.i + 1
    if (param.w + param.h - 1 == param.i) then
      param.stage = startGame
      return
    end

    local n
    if (param.i >= param.h) then
      n = (param.h - 1) * param.w + (param.i - param.h + 1)
    else
      n = param.i * param.w
    end

    param.c = n % param.w
    param.r = math.floor(n / param.w)
  end
end

newBall = function(param)
  Good.KillObj(param.ball)

  local b = GenColorObj(param._id, 20, 20, 0xffffff00)
  Good.SetPos(b, math.floor((SCR_W - 20) / 2), SCR_H - 6 * param.bh)
  param.ball = b

  if (math.random(2) == 1) then
    param.dirx = -1
  else
    param.dirx = 1
  end
  param.diry = 1

  spdxBall = spdyBall
end

startGame = function(param)
  -- create paddle
  local p = GenColorObj(param._id, math.floor(2.5 * param.bw), math.floor(param.bh / 2), 0xffffffff)
  Good.SetPos(p, math.floor((SCR_W - 2.5 * param.bw) / 2), SCR_H - 4 * param.bh)
  param.paddle = p

  -- create ball
  param.ball = -1
  newBall(param)

  -- next stage
  param.stage = mainLoop
end

movePaddle = function(param)
  local IsLeftTouch = false
  local IsRightTouch = false
  if (Input.IsKeyDown(Input.LBUTTON)) then
    local x,y = Input.GetMousePos()
    if (PtInRect(x, y, 0, 0, TOUCH_AREA_W, SCR_H)) then
      IsLeftTouch = true
    elseif (PtInRect(x, y, SCR_W - TOUCH_AREA_W, 0, SCR_W, SCR_H)) then
      IsRightTouch = true
    end
  end
  if (IsLeftTouch or Input.IsKeyDown(Input.LEFT)) then
    local x,y = Good.GetPos(param.paddle)
    x = x - spdPaddle
    if (0 > x) then
      x = 0
    end
    Good.SetPos(param.paddle, x, y)
    deltax = deltax - 1
    if (-3 > deltax) then
      deltax = -3
    end
  elseif (IsRightTouch or Input.IsKeyDown(Input.RIGHT)) then
    local x,y = Good.GetPos(param.paddle)
    local l,t,w,h = Good.GetDim(param.paddle)
    x = x + spdPaddle
    if (x + w > SCR_W) then
      x = SCR_W - w
    end
    Good.SetPos(param.paddle, x, y)
    deltax = deltax + 1
    if (3 < deltax) then
      deltax = 3
    end
  else
    deltax = 0
  end
end

moveBall = function(param)
  local x,y = Good.GetPos(param.ball)
  local l,t,w,h = Good.GetDim(param.ball)

  local ret = false
  x = x + param.dirx * spdxBall
  y = y + param.diry * spdyBall

  if (0 > x) then
    x = 0
    param.dirx = param.dirx * -1
    ret = true
  elseif (x + w > SCR_W) then
    x = SCR_W - w
    param.dirx = param.dirx * -1
    ret = true
  end

  if (0 > y) then
    y = 0
    param.diry = param.diry * -1
    ret = true
  end

  Good.SetPos(param.ball, x, y)

  return ret -- is hit
end

ballOutOfLowerBound = function(param)
  local x,y = Good.GetPos(param.ball)
  if (SCR_H <= y) then
    return true
  else
    return false
  end
end

ballCollidePaddle = function(param)
  local bx,by = Good.GetPos(param.ball)
  local bl,bt,bw,bh = Good.GetDim(param.ball)
  local px,py = Good.GetPos(param.paddle)
  local pl,pt,pw,ph = Good.GetDim(param.paddle)

  if (bx + bw > px and by + bh > py and bx < px + pw and py < py + ph) then
    Good.SetPos(param.ball, bx, py - bh)
    param.diry = param.diry * -1
    spdxBall = deltax + spdxBall
    if (10 < spdxBall) then
      spdxBall = 10
    elseif (-10 > spdxBall) then
      spdxBall = -10
    end
    return true
  else
    return false
  end
end

ballCollideBrick = function(param)
  local x,y = Good.GetPos(param.ball)
  local l,t,w,h = Good.GetDim(param.ball)

  local nextx = x + param.dirx * spdxBall
  local nexty = y + param.diry * spdyBall

  local id

  id = Good.PickObj(x, nexty, Good.COLBG)
  if (-1 ~= id and param.ball ~= id and param.paddle ~= id) then
    param.diry = param.diry * -1
    Good.KillObj(id)
    return true
  end

  id = Good.PickObj(x + w, nexty + h, Good.COLBG)
  if (-1 ~= id and param.ball ~= id and param.paddle ~= id) then
    param.diry = param.diry * -1
    Good.KillObj(id)
    return true
  end

  id = Good.PickObj(nextx, y, Good.COLBG)
  if (-1 ~= id and param.ball ~= id and param.paddle ~= id) then
    param.dirx = param.dirx * -1
    Good.KillObj(id)
    return true
  end

  id = Good.PickObj(nextx + w, y + h, Good.COLBG)
  if (-1 ~= id and param.ball ~= id and param.paddle ~= id) then
    param.dirx = param.dirx * -1
    Good.KillObj(id)
    return true
  end

  id = Good.PickObj(nextx, nexty, Good.COLBG)
  if (-1 ~= id and param.ball ~= id and param.paddle ~= id) then
    param.dirx = param.dirx * -1
    param.diry = param.diry * -1
    Good.KillObj(id)
    return true
  end

  id = Good.PickObj(nextx + w, nexty + h, Good.COLBG)
  if (-1 ~= id and param.ball ~= id and param.paddle ~= id) then
    param.dirx = param.dirx * -1
    param.diry = param.diry * -1
    Good.KillObj(id)
    return true
  end

  return false
end

checkCollision = function(param)
  if (ballCollidePaddle(param)) then
    Sound.PlaySound(hit_snd)
    return
  end

  if (ballCollideBrick(param)) then
    Sound.PlaySound(hit_snd)
    return
  end

  if (ballOutOfLowerBound(param)) then
    newBall(param)
    return
  end
end

mainLoop = function(param)
  -- move paddle
  movePaddle(param)

  -- move ball
  if (moveBall(param)) then
    Sound.PlaySound(hit_snd)
  end

  -- check collision
  checkCollision(param)

  -- new game?
  if (Input.IsKeyPressed(Input.BTN_A)) then
    newGame(param)
  end
end

Game.OnCreate = function(param)
  math.randomseed(os.clock())
  param.w = 10
  param.h = 6
  param.bw = SCR_W/10
  param.bh = math.floor(param.bw / 2.5)

  local idBgm = Sound.PlaySound(bgm)

  newGame(param)
end

Game.OnStep = function(param)
  param.stage(param)
end
