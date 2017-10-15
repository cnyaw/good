
math.randomseed(os.time())

local CX_CARD, CY_CARD = 71, 96
local TEX_CARDS, TEX_EMPTY = 5, 7

local CHW, CHH = 16, 32

local IDBKGND = 3

local Cards = {}
local CardsTop = 0

function PrepareCards()
  -- prepare cards
  CardsTop = 0
  Cards = {}

  for i = 0, 51 do
    Cards[i] = i
  end

  -- random suffule
  for i = 1, 51 do
    local r = math.random(i + 1) - 1
    local n = Cards[r]
    Cards[r] = Cards[i]
    Cards[i] = n
  end
end

function DealOneCard()
  if (52 == CardsTop) then
    return -1
  end

  local c = Cards[CardsTop]
  CardsTop = CardsTop + 1

  return c
end

function GetCardPos(index)
  local x = 64 + CX_CARD + (CX_CARD + 2) * (index % 5)
  local y = 4 + (CY_CARD + 2) * math.floor(index / 5)
  return x, y
end

function GetNumber(c)
  return c % 13
end

function GetFace(c)
  return math.floor(c / 13)
end

function DealCards(param)
  for i = 0, 24 do
    local o = param.cards[i]
    if (0 == Good.GetVisible(o)) then
      local c = DealOneCard()
      if (-1 == c) then
        return -- no more new card
      end
      local p = Good.GetParam(o)
      p.c = c
      local ox = CX_CARD * GetNumber(c)
      local oy = CY_CARD * GetFace(c)
      Good.SetDim(o, ox, oy, CX_CARD, CY_CARD)
      Good.SetVisible(o, 1)
      Good.AddChild(-1, o) -- make sure it it visible
    end
  end
end

function CheckClearPair(param)
  local mx, my = Input.GetMousePos()
  local p1 = Good.GetParam(param.cards[param.sel])
  for i = 0, 24 do
    local x, y = GetCardPos(i)
    if (PtInRect(mx, my, x, y, x + CX_CARD, y + CY_CARD)) then
      local p2 = Good.GetParam(param.cards[i])
      if (param.sel ~= i and GetNumber(p1.c) == GetNumber(p2.c)) then
        local col1, row1 = param.sel % 5, math.floor(param.sel / 5)
        local col2, row2 = i % 5, math.floor(i / 5)
        --Good.Trace(col1 .. ":" .. row1 .. ", " .. col2 .. ":" .. row2)
        if (1 >= math.abs(col1 - col2) and 1 >= math.abs(row1 - row2)) then
          Good.SetVisible(param.cards[param.sel], 0)
          Good.SetVisible(param.cards[i], 0)
          return true
        else
          return false
        end
      else
        return false
      end
    end
  end

  return false
end

function Rearrange(param)
  for i = 0, 24 do
    local o = param.cards[i]
    if (0 == Good.GetVisible(o)) then
      for j = i + 1, 24 do
        local o2 = param.cards[j]
        if (0 ~= Good.GetVisible(o2)) then
          local x1, y1 = Good.GetPos(o)
          local x2, y2 = Good.GetPos(o2)
          Good.SetPos(o, x2, y2)
          Good.SetPos(o2, x1, y1)
          param.cards[i] = o2
          param.cards[j] = o
          break
        end
      end
    end
  end
end

function NewGame(param)
  local id = param._id

  -- delete all objects except the bkgnd.
  local nc = Good.GetChildCount(id)
  for i = nc,0,-1 do
    local idChild = Good.GetChild(id, i)
    if (IDBKGND ~= idChild) then
      Good.KillObj(idChild)
    end
  end

  -- create the NewGame lable.
  GenStrObj(-1, 515, 60, 'NewGame')

  -- create cards.
  PrepareCards()

  param.cards = {} -- store object handles

  for i = 0, 24 do
    local x, y = GetCardPos(i)

    -- gen empty slot
    local o = Good.GenObj(-1, TEX_EMPTY)
    Good.SetPos(o, x, y)

    -- gen card
    local c = DealOneCard()
    local ox = CX_CARD * GetNumber(c)
    local oy = CY_CARD * GetFace(c)
    o = GenTexObj(-1, TEX_CARDS, CX_CARD, CY_CARD, ox, oy)
    Good.SetPos(o, x, y)

    local p = Good.GetParam(o)
    p.c = c

    param.cards[i] = o
  end

  Game.OnStep = sWaitForDrag
end

Game = {}

Game.OnStep = function(param)
end

Game.OnCreate = function(param)
  NewGame(param)
end

function sWaitForDrag(param)
  if (not Input.IsKeyDown(Input.LBUTTON) and
      not Input.IsKeyPushed(Input.LBUTTON)) then
    return
  end

  local mx, my = Input.GetMousePos()

  if (Input.IsKeyPushed(Input.LBUTTON) and
      PtInRect(mx, my, 515, 60, 515 + 8 * CHW, 60 + CHH)) then
    NewGame(param)
    return
  end

  for i = 0, 24 do
    local x, y = GetCardPos(i)
    if (PtInRect(mx, my, x, y, x + CX_CARD, y + CY_CARD)) then
      param.lastx, param.lasty = mx, my
      param.offsetx = mx - x
      param.offsety = my - y
      param.sel = i
      Good.AddChild(param._id, param.cards[i]) -- make sure it is visible
      Game.OnStep = sDragging
      return
    end
  end
end

function sDragging(param)
  -- handle mouse up
  if (not Input.IsKeyDown(Input.LBUTTON)) then
    if (CheckClearPair(param)) then
      Rearrange(param)
      DealCards(param)
    else -- undo move
      local x, y = GetCardPos(param.sel)
      Good.SetPos(param.cards[param.sel], x, y)
      Game.OnStep = sWaitForDrag
    end
    return
  end

  -- handle dragging
  local mx, my = Input.GetMousePos()
  if (mx ~= param.lastx or my ~= param.lasty) then
    local offsetx = mx - param.lastx
    local offsety = my - param.lasty
    local sel = param.cards[param.sel]
    local x, y = Good.GetPos(sel)
    Good.SetPos(sel, x + offsetx, y + offsety)
    param.lastx, param.lasty = mx, my
  end
end
