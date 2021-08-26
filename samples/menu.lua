
local CHW, CHH = 16, 32

local MENUX, MENUY = 50, 25
local MENUW, MENUH = 130, 40
local NUM_MENU = 11

local ICONS = Resource.GetTexId('icons')
local ICONW, ICONH = 128, 128

local ENTERX = ICONW
local ENTERTIME = 15

local Packages = {'mmc', 'mario', 'stge', 'puzzle', 'weeder', 'breakout', 'zelda', 'life', 'mine', 'link', 'animator'}

function PlayGame(sel)
  Good.CallPackage(Packages[1 + sel] .. '.txt')
end

Icon = {}

function Icon.sCreate(param)
  local id = param._id

  local x, y = Good.GetPos(id)
  x = x + ENTERX
  Good.SetPos(id, x, y)

  Good.SetVisible(id, Good.VISIBLE)
  Good.SetBgColor(id, 0xffffff)

  param.time = ENTERTIME
  param.stage = Icon.sEnter
end

function Icon.sEnter(param)
  local id = param._id

  local x, y = Good.GetPos(id)
  x = x - ENTERX/ENTERTIME
  Good.SetPos(id, x, y)

  local alpha = math.floor(256 * (1 - param.time / ENTERTIME))
  Good.SetBgColor(id, 0xffffff + alpha * 0x1000000)

  param.time = param.time - 1
  if (0 >= param.time) then
    param.stage = Icon.sIdle
  end
end

function Icon.sIdle(param)
end

Icon.OnCreate = function(param)
  param.stage = Icon.sCreate
end

Icon.OnStep = function(param)
  param.stage(param)
end

Menu = {}

function genIcon(param)
  Good.KillObj(param.icon)
  if (8 <= param.menu) then
    return
  end
  local o = GenTexObj(-1, ICONS, ICONW, ICONH, ICONW * (param.menu % 4), ICONH * math.floor(param.menu / 4), 'Icon')
  Good.SetPos(o, MENUX + MENUW, MENUY + param.menu * MENUH - MENUH/2)
  Good.SetVisible(o, Good.INVISIBLE)
  param.icon = o
end

Menu.OnCreate = function(param)
  for i = 1, NUM_MENU do
    GenStrObj(-1, MENUX, (i - 1) * MENUH + MENUY, Packages[i], 1, 16, 32, 0xffff0000)
  end

  param.curTargetX = MENUX - CHW - 20
  param.curTargetY = MENUY
  param.cursor = GenStrObj(-1, param.curTargetX, param.curTargetY, '>', 1, 16, 32, 0xffff0000)
  param.curTimeX = 0

  param.menu = 0
  param.icon = -1
  genIcon(param)
end

function animCursor(param)
  param.curTimeX = param.curTimeX + 1
  if (120 < param.curTimeX) then -- animation period 2 seconds
    param.curTimeX = 0
  end

  local x,y = Good.GetPos(param.cursor)
  x = param.curTargetX + 10 * math.sin(math.pi * param.curTimeX / 60)

  if (y > param.curTargetY) then
    y = y - (y - param.curTargetY) / (60 * 0.15)
    if (y < param.curTargetY) then
      y = param.curTargetY
    end
  elseif (y < param.curTargetY) then
    y = y + (param.curTargetY - y) / (60 * 0.15)
    if (y > param.curTargetY) then
      y = param.curTargetY
    end
  end

  Good.SetPos(param.cursor, x,y)
end

function handleMoveCursor(param)
  -- by kbd
  if (Input.IsKeyPushed(Input.DOWN)) then
    if (NUM_MENU > param.menu + 1) then
      param.menu = param.menu + 1
      param.curTargetY = param.curTargetY + MENUH
      genIcon(param)
    end
  elseif (Input.IsKeyPushed(Input.UP)) then
    if (0 < param.menu) then
      param.menu = param.menu - 1
      param.curTargetY = param.curTargetY - MENUH
      genIcon(param)
    end
  end

  -- by mouse
  if (Input.IsKeyPushed(Input.LBUTTON)) then
    local x, y = Input.GetMousePos()
    if (PtInRect(x, y, MENUX, MENUY, MENUX + MENUW, MENUY + NUM_MENU * MENUH)) then
      local idx = math.floor((y - MENUY) / MENUH)
      if (param.menu ~= idx) then
        param.curTargetY = MENUY + idx * MENUH
        param.menu = idx
        genIcon(param)
      else
        PlayGame(param.menu)
      end
    end
  end
end

Menu.OnStep = function(param)
  animCursor(param)
  handleMoveCursor(param)

  if (Input.IsKeyPressed(Input.RETURN + Input.BTN_A)) then
    PlayGame(param.menu)
  end
end
