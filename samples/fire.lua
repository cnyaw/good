local SW, SH = Good.GetWindowSize()
local PW, PH = 4, 4
local FW = math.floor(SW / PW)
local FH = math.floor(SH / PH)

Fire = {}

function makeColor(Colors, l, r, c1, c2)
  for i = l, r do
    local v = (i - l) / (r - l)
    local r = math.floor(c1[1] + v * (c2[1] - c1[1]))
    local g = math.floor(c1[2] + v * (c2[2] - c1[2]))
    local b = math.floor(c1[3] + v * (c2[3] - c1[3]))
    Colors[i] = 4278190080 + r * 65536 + g * 256 + b
  end
end

Fire.OnCreate = function(param)
  math.randomseed(os.clock())

  -- bitmap
  param.bmp = {}
  for i = 0, (2 + FW) * (2 + FH) - 1 do
    param.bmp[i] = 0
  end

  -- init color table
  local c1 = {0, 0, 0}
  local c2 = {255, 0, 0}
  local c3 = {255, 180, 0}
  local c4 = {255, 255, 255}

  param.Colors = {}

  makeColor(param.Colors, 0, 32, c1, c2)
  makeColor(param.Colors, 33, 65, c2, c3)
  makeColor(param.Colors, 66, 99, c3, c4)
end

local timer = 0

Fire.OnStep = function(param)
  timer = timer + 1
  if (9 > timer) then
    return
  end
  timer = 0

  for i = 1, FW do
    local idx = i + (2 + FW) * (1 + FH)
    if (math.random(2) == 1) then
      param.bmp[idx] = 0
    else
      param.bmp[idx] = 99
    end
  end

  for j = FH, 0, -1 do
    for i = 1, FW do
      local idx = i + (2 + FW) * j
      local idx1 = idx + (2 + FW)
      local idx2 = idx1 + 1
      local idx3 = idx1 - 1

      local c1 = param.bmp[idx1]
      local c2 = param.bmp[idx2]
      local c3 = param.bmp[idx3]
      local c = ((c1 + c2 + c3) / 3) - 1
      if (0 > c) then
        c = 0
      end

      param.bmp[idx] = c
    end
  end
end

Fire.OnDraw = function(param)
  for i = 1, FW do
    for j = 0, FH - 1 do
      local idx = i + j * (2 + FW)
      local color = param.Colors[math.floor(param.bmp[idx])]
      Graphics.FillRect(-1, (i - 1) * PW, j * PH, PW, PH, color)
    end
  end
end
