
local W,H = 384,384
local x,y = (640-W)/2, (480-H)/2
local w,h = W/3, H/3

local blank = 8 -- position of blank piece

Level = {}

genPuzzle = function()
  local a
  for i=1,9 do
    local ii = i - 1
    if (9 == i) then
      a = GenColorObj(-1, w, h, 0xff000000)
    else
      a = GenTexObj(-1, 3, w, h, w * (ii % 3), h * math.floor(ii / 3))
    end
    Good.SetPos(a, x + (1 + w) * (ii % 3), y + (1 + h) * math.floor(ii / 3))
  end
end

moveLeft = function()
  if (2 == blank or 5 == blank or 8 == blank) then
    return
  end

  local xx, yy = x + (1 + w) * (blank % 3), y + (1 + h) * math.floor(blank / 3)
  local b = Good.PickObj(xx, yy, Good.COLBG)
  local a = Good.PickObj(xx + w + 1, yy, Good.TEXBG)

  Good.SetPos(b, xx + w + 1, yy)
  Good.SetPos(a, xx, yy)

  blank = blank + 1
end

moveRight = function()
  if (0 == blank or 3 == blank or 6 == blank) then
    return
  end

  local xx, yy = x + (1 + w) * (blank % 3), y + (1 + h) * math.floor(blank / 3)
  local b = Good.PickObj(xx, yy, Good.COLBG)
  local a = Good.PickObj(xx - w - 1, yy, Good.TEXBG)

  Good.SetPos(b, xx - w - 1, yy)
  Good.SetPos(a, xx, yy)

  blank = blank - 1
end

moveUp = function()
  if (6 == blank or 7 == blank or 8 == blank) then
    return
  end

  local xx, yy = x + (1 + w) * (blank % 3), y + (1 + h) * math.floor(blank / 3)
  local b = Good.PickObj(xx, yy, Good.COLBG)
  local a = Good.PickObj(xx, yy + h + 1, Good.TEXBG)

  Good.SetPos(b, xx, yy + h + 1)
  Good.SetPos(a, xx, yy)

  blank = blank + 3
end

moveDown = function()
  if (0 == blank or 1 == blank or 2 == blank) then
    return
  end

  local xx, yy = x + (1 + w) * (blank % 3), y + (1 + h) * math.floor(blank / 3)
  local b = Good.PickObj(xx, yy, Good.COLBG)
  local a = Good.PickObj(xx, yy - h - 1, Good.TEXBG)

  Good.SetPos(b, xx, yy - h - 1)
  Good.SetPos(a, xx, yy)

  blank = blank - 3
end

shufflePuzzle = function()
  for i=1,150 do
    local m = math.random(1,4)
    if (1 == m) then
      moveLeft()
    elseif (2 == m) then
      moveRight()
    elseif (3 == m) then
      moveDown()
    else
      moveUp()
    end
  end
end

Level.OnCreate = function(param)
  math.randomseed(os.clock())
  genPuzzle()
end

Level.OnStep = function(param)
  if (Input.IsKeyPushed(Input.LEFT)) then
    moveLeft()
  elseif (Input.IsKeyPushed(Input.RIGHT)) then
    moveRight()
  elseif (Input.IsKeyPushed(Input.DOWN)) then
    moveDown()
  elseif (Input.IsKeyPushed(Input.UP)) then
    moveUp()
  end

  if (Input.IsKeyPushed(Input.LBUTTON)) then
    local mx,my = Input.GetMousePos()

    local xx, yy = x + (1 + w) * (blank % 3), y + (1 + h) * math.floor(blank / 3) -- blank pos
    if (mx >= xx - w and mx < xx and my >= yy and my < yy + h) then
      moveRight()
    elseif (mx >= xx and mx < xx + w and my >= yy - h and my < yy) then
      moveDown()
    elseif (mx >= xx + w and mx < xx + 2 * w and my >= yy and my < yy + h) then
      moveLeft()
    elseif (mx >= xx and mx < xx + w and my >= yy + h and my < yy + 2 * h) then
      moveUp()
    elseif (mx < x or my < y or mx > x + 3 * w or my > y + 3 * h) then
      shufflePuzzle()
    end
  end
end
