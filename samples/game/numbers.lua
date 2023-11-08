
local SW, SH = Good.GetWindowSize()
local BORDER = 30
local OFFSET_X = BORDER
local OFFSET_Y = BORDER - 10
local MAP_W = SW - 2 * BORDER
local MAP_H = SH - 2 * BORDER

local level = 2
local N
local map, index
local dx, dy
local curr_index

local tick
local timer = 0
local otimer

math.randomseed(os.clock())

Game = {}

function GenTimerStrObj()
  return GenStrObj(-1, 5, SH - 40, string.format('%d:%02d', timer, tick))
end

function Game.OnCreate(param)
  N = level * level
  dx = math.floor(MAP_W / level)
  dy = math.floor(MAP_H / level)

  map = {}
  index = {}
  curr_index = 1

  for i = 0, N - 1 do
    map[i] = nil
    index[i] = i + 1
  end

  for i = 0, N - 1 do
    local r = math.random(i + 1) - 1
    local n = index[r]
    index[r] = index[i]
    index[i] = n
  end

  for i = 0, N - 1 do
    local row = math.floor(i / level)
    local col = i % level
    local o = GenColorObj(-1, dx - 1, dy - 1, 0xffffffff)
    local x = OFFSET_X + col * dx
    local y = OFFSET_Y + row * dy
    Good.SetPos(o, x, y)
    local s = GenStrObj(o, 0, 0, tostring(index[i]), 1, 16, 32, 0xffff0000)
    map[i] = o
  end

  tick = 60
  timer = timer + math.floor(N / 2) + level - 1

  otimer = GenTimerStrObj()
end

function Game.OnStep(param)
  if (curr_index == N + 1 or 0 == timer) then
    if (Input.IsKeyPushed(Input.LBUTTON)) then
      if (0 == timer) then
        level = 2
      else
        level = level + 1
      end
      Good.GenObj(-1, 2)
    end
    return
  end

  tick = tick - 1
  if (0 == tick) then
    tick = 60
    timer = timer - 1
    if (0 == timer) then
      GenColorObj(-1, SW, SH, 0x66000000)
      GenStrObj(-1, 150, 160, 'Game Over!')
      tick = 0
      Good.KillObj(otimer)
      otimer = GenTimerStrObj()
      return
    end
  end

  Good.KillObj(otimer)
  otimer = GenTimerStrObj()

  if (Input.IsKeyPushed(Input.LBUTTON)) then
    local x, y = Input.GetMousePos()
    local row = math.floor((y - OFFSET_Y) / dy)
    local col = math.floor((x - OFFSET_X) / dy)
    local i = col + row * level
    if (nil == map[i]) then
      return
    end
    if (curr_index ~= index[i]) then
      return
    end
    Good.SetScript(map[i], 'Clear')
    map[i] = nil
    curr_index = curr_index + 1
    if (curr_index == N + 1) then
      GenStrObj(-1, 100, 180, 'Level Complete!')
    end
  end
end

Clear = {}

function Clear.OnStep(param)
  local id = param._id

  if (nil == param.step) then
    Good.KillAllChild(id)
    param.step = 0;
    return
  end

  param.step = param.step + 1
  if (8 == param.step) then
    Good.KillObj(id)
    return
  end

  local d = math.floor(0xf0 * (1 - math.sin(math.pi/2 * param.step/10)))
  Good.SetBgColor(id, 0xffffff + d * 0x1000000)
end
