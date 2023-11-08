
math.randomseed(os.time())

local idTex = Resource.GetTexId('tex1')
local CELLW, CELLH = 16, 16

Level = {}

function Display(cell, over)
  if (cell.isOpen) then
    Good.SetDim(cell.obj, 0, (15 - cell.neighbor) * CELLH, CELLW, CELLH)
  end

  if (cell.isMark) then
    if (over and not cell.isMine) then
      Good.SetDim(cell.obj, 0, 4 * CELLH, CELLW, CELLH)
    else
      Good.SetDim(cell.obj, 0, CELLH, CELLW, CELLH)
    end
  elseif (cell.isMine) then
    if (cell.hitpoint) then
      Good.SetDim(cell.obj, 0, 3 * CELLH, CELLW, CELLH)
    else
      Good.SetDim(cell.obj, 0, 5 * CELLH, CELLW, CELLH)
    end
  end
end

function NewGame(param)
  param.over = false

  -- reset cells
  for i = 1, param.row * param.col do
    local cell = param.cells[i]
    cell.isMine = false
    cell.isOpen = false
    cell.isMark = false
    cell.hitpoint = false
    cell.neighbor = 0
    Good.SetDim(cell.obj, 0, 0, CELLW, CELLH)
  end

  -- put mines
  for i = 1, param.mines do
    local c = math.random(param.col) - 1
    local r = math.random(param.row) - 1
    local cell = param.cells[1 + c + r * param.col]
    if (cell.isMine) then
      i = i - 1
    else
      cell.isMine = true
      Good.SetDim(cell.obj, 0, 0, CELLW, CELLH)
    end
  end

  -- pre calc neighbor of mines
  for i = 0, param.row * param.col - 1 do
    local c = i % param.col
    local r = math.floor(i / param.col)
    local cell = param.cells[1 + i]
    for k = -1,1 do
      for l = -1,1 do
        if (r + k >= 0 and r + k < param.row and c + l >= 0 and c + l < param.col) then
          local cell2 = param.cells[1 + (r + k) * param.col + c + l]
          if (cell2.isMine) then
            cell.neighbor = cell.neighbor + 1
          end
        end
      end
    end
  end
end

Level.OnCreate = function(param)
  param.row = 9
  param.col = 9
  param.mines = 10
  param.xoffset = (320 - (CELLW * param.col)) / 2
  param.yoffset = (240 - (CELLH * param.row)) / 2
  param.cells = {}

  -- create cell objs
  local ncells = param.row * param.col
  for i = 0, ncells-1 do
    local cell = {}
    local obj = GenTexObj(-1, idTex, CELLW, CELLH)
    local x = CELLW * (i % param.col) + param.xoffset
    local y = CELLH * math.floor(i / param.col) + param.yoffset
    Good.SetPos(obj, x, y)
    cell.obj = obj
    param.cells[1 + i] = cell
  end

  NewGame(param)
end

function Expand(param, col, row)
  for i = -1,1 do
    for j = -1,1 do
      if (row + j >= 0 and row + j < param.row and col + i >= 0 and col + i < param.col) then
        local cell = param.cells[1 + (row + j) * param.col + col + i]
        if (not cell.isOpen) then
          cell.isOpen = true
          Display(cell, false)
          if (0 == cell.neighbor) then
            Expand(param, col + i, row + j)
          end
        end
      end
    end
  end
end

Level.OnStep = function(param)
  if (Input.IsKeyPushed(Input.RETURN) or Input.IsKeyPushed(Input.BTN_A)) then
    NewGame(param)
  elseif (not param.over and Input.IsKeyPushed(Input.LBUTTON) or Input.IsKeyPushed(Input.RBUTTON)) then
    local mx,my = Input.GetMousePos()
    if (param.xoffset >= mx or param.yoffset >= my or param.xoffset + param.col * CELLW <= mx or param.yoffset + param.row * CELLH <= my) then
      return
    end

    local col = math.floor((mx - param.xoffset) / CELLW)
    local row = math.floor((my - param.yoffset) / CELLH)
    local cell = param.cells[1 + row * param.col + col]

    if (Input.IsKeyPushed(Input.LBUTTON)) then -- try to open
      if (cell.isMark) then
        -- do nothing
      elseif (cell.isMine) then -- gameover
        param.over = true
        cell.hitpoint = true
        for i = 1, param.col * param.row do
          Display(param.cells[i], true)
        end
      elseif (0 < cell.neighbor) then
        cell.isOpen = true
        Display(cell, false)
      else
        cell.isOpen = true
        Display(cell, false)
        Expand(param, col, row)
      end
    else -- try to mark
      if (cell.isMark) then
        cell.isMark = false
        Good.SetDim(cell.obj, 0, 0, CELLW, CELLH)
      elseif (not cell.isOpen) then
        cell.isMark = true
        Display(cell, false)
      end
    end
  end
end
