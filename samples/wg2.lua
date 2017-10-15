
-- CONSTANTs
local body_right = Resource.GetSpriteId('b_right')
local body_down = Resource.GetSpriteId('b_down')
local body_left = Resource.GetSpriteId('b_left')
local body_up = Resource.GetSpriteId('b_up')
local head_right = Resource.GetSpriteId('h_right')
local head_down = Resource.GetSpriteId('h_down')
local head_left = Resource.GetSpriteId('h_left')
local head_up = Resource.GetSpriteId('h_up')
local grass = Resource.GetSpriteId('grass')
local flower = Resource.GetSpriteId('flower')
local shit = Resource.GetSpriteId('shit')
local frag = Resource.GetSpriteId('frag')
local tot = Resource.GetSpriteId('tot')
local boom_s = Resource.GetSpriteId('boom_s')
local boom_m = Resource.GetSpriteId('boom_m')
local boom_l = Resource.GetSpriteId('boom_l')
local hand_right = Resource.GetSpriteId('hand_right')
local hand_down = Resource.GetSpriteId('hand_down')
local hand_left = Resource.GetSpriteId('hand_left')
local hand_up = Resource.GetSpriteId('hand_up')

-- GLOBAL
local gameover, finish, running = false, false, false
local level
local ngrass = 0    -- number grass of this level
local movement = 0  -- movement of weeder

-- Weeder's HEAD
Head = {}

Head.OnStep = function(param)
  local id = param._id
  local dir = Good.GetSpriteId(Good.GetParent(id))

  if (Input.IsKeyPushed(Input.LEFT)) then
    if (body_right ~= dir) then
      Good.SetSpriteId(id, head_left)
    end
  elseif (Input.IsKeyPushed(Input.RIGHT)) then
    if (body_left ~= dir) then
      Good.SetSpriteId(id, head_right)
    end
  elseif (Input.IsKeyPushed(Input.UP)) then
    if (body_down ~= dir) then
      Good.SetSpriteId(id, head_up)
    end
  elseif (Input.IsKeyPushed(Input.DOWN)) then
    if (body_up ~= dir) then
      Good.SetSpriteId(id, head_down)
    end
  end

  if (gameover or finish) then
    Good.SetScript(id, '')
  end
end

-- Weeder's BODY
Body = {}

gameIsOver = function(param,x,y)
  local id = param._id
  local map = Good.FindChild(level, 'map')
  local mapx,mapy = Good.GetPos(map)

  local tile = Resource.GetTileByPos(Good.GetMapId(map), x - mapx, y - mapy)
  local spr = boom_m

  if (tile == 59) then -- rock
    spr = boom_l
  elseif (57 == tile) then -- empty ground
    spr = boom_s
  end

  Good.GenObj(id, spr)

  gameover = true
  Good.SetScript(id, '')
end

Body.OnStep = function(param)
  local id = param._id
  local dir = Good.GetSpriteId(id)

  -- first move
  if (not running) then
    if ((body_left == dir and Input.IsKeyPushed(Input.LEFT)) or
      (body_right == dir and Input.IsKeyPushed(Input.RIGHT)) or
      (body_down == dir and Input.IsKeyPushed(Input.DOWN)) or
      (body_up == dir and Input.IsKeyPushed(Input.UP))) then
      running = true
      movement = 0
      Good.SetScript(Good.FindChild(id, 'head'), 'Head')
    end
    return
  end

  -- moving
  local spd = 0.5
  local x,y = Good.GetPos(id)

  if (body_left == dir) then
    x = x - spd
  elseif (body_right == dir) then
    x = x + spd
  elseif (body_up == dir) then
    y = y - spd
  elseif (body_down == dir) then
    y = y + spd
  end

  Good.SetPos(id, x, y)

  -- erase grass
  movement = movement + spd
  if (32 > movement) then
    return
  end

  movement = 0

  Good.KillObj(Good.PickObj(x, y, Good.SPRITE, grass))

  local handid = -1
  local hand = Good.PickObj(x, y, Good.SPRITE, hand_right) -- search hand
  if (-1 ~= hand) then
    handid = Good.GetSpriteId(hand)
    Good.KillObj(hand)
  else
    hand = Good.PickObj(x, y, Good.SPRITE, hand_left)
    if (-1 ~= hand) then
      handid = Good.GetSpriteId(hand)
      Good.KillObj(hand)
    else
      hand = Good.PickObj(x, y, Good.SPRITE, hand_up)
      if (-1 ~= hand) then
        handid = Good.GetSpriteId(hand)
        Good.KillObj(hand)
      else
        hand = Good.PickObj(x, y, Good.SPRITE, hand_down)
        if (-1 ~= hand) then
          handid = Good.GetSpriteId(hand)
          Good.KillObj(hand)
        end
      end
    end
  end

  ngrass = ngrass - 1
  if (0 >= ngrass) then
    finish = true
    Good.SetScript(id, '')
    return
  end

  -- change moving direction
  local headdir

  local head = Good.FindChild(id, 'head')
  if (-1 ~= handid) then
    if (hand_right == handid) then
      headdir = head_right
    elseif (hand_down == handid) then
      headdir = head_down
    elseif (hand_left == handid) then
      headdir = head_left
    elseif (hand_up == handid) then
      headdir = head_up
    end
    Good.SetSpriteId(head, headdir)
  else
    headdir = Good.GetSpriteId(head)
  end

  local spr
  if (head_up == headdir) then
    spr = body_up
  elseif (head_down == headdir) then
    spr = body_down
  elseif (head_left == headdir) then
    spr = body_left
  elseif (head_right == headdir) then
    spr = body_right
  end

  Good.SetSpriteId(id, spr)

  -- test next block
  if (head_left == headdir) then
    x = x - 32
  elseif (head_right == headdir) then
    x = x + 32
  elseif (head_up == headdir) then
    y = y - 32
  elseif (head_down == headdir) then
    y = y + 32
  end

  local hit = Good.PickObj(x, y, Good.SPRITE, grass)
  if (0 >= hit) then
    gameIsOver(param,x,y)
  end
end

-- THE LEVEL, GAME PLAY
Level = {}

InitLevel = function(param)
  local idLvl = param._id
  local map = Good.FindChild(idLvl, 'map')
  local idResMap = Good.GetMapId(map)
  local mapx,mapy = Good.GetPos(map)
  local cx,cy = Resource.GetMapSize(idResMap)
  local body = Good.FindChild(idLvl, 'body')
  local wg = Good.GetSpriteId(body)

  level = Good.GetLevelId(idLvl)

  ngrass = 0

  for i = 1, cx-2 do
    for j = 1, cy-2 do
      local x,y = 32 * i, 32 * j
      local tile = Resource.GetTileByPos(idResMap, x, y)
      if (57 == tile) then -- grass
        x = mapx + x
        y = mapy + y
        if (0 >= Good.PickObj(x, y, Good.SPRITE, wg)) then -- not occupy by a weeder
          ngrass = ngrass + 1
          local idObj = Good.GenObj(idLvl, grass)
          Good.SetPos(idObj, x, y)
          -- make sure hands are over grass
          local hit = Good.PickObj(x, y, Good.SPRITE, hand_left)
          if (0 < hit) then
            Good.AddChild(idLvl, hit)
          else
            hit = Good.PickObj(x, y, Good.SPRITE, hand_right)
            if (0 < hit) then
              Good.AddChild(idLvl, hit)
            else
              hit = Good.PickObj(x, y, Good.SPRITE, hand_down)
              if (0 < hit) then
                Good.AddChild(idLvl, hit)
              else
                hit = Good.PickObj(x, y, Good.SPRITE, hand_up)
                if (0 < hit) then
                  Good.AddChild(idLvl, hit)
                end
              end
            end
          end
        end
      elseif (17 == tile) then -- flower
        local idObj = Good.GenObj(idLvl, flower)
        Good.SetPos(idObj, mapx + x, mapy + y)
      elseif (45 == tile) then -- shit
        local idObj = Good.GenObj(idLvl, shit)
        Good.SetPos(idObj, mapx + x, mapy + y)
      elseif (53 == tile) then -- frag
        local idObj = Good.GenObj(idLvl, frag)
        Good.SetPos(idObj, mapx + x, mapy + y)
      elseif (55 == tile) then -- tot
        local idObj = Good.GenObj(idLvl, tot)
        Good.SetPos(idObj, mapx + x, mapy + y)
      end
    end
  end
end

Level.OnCreate = function(param)
  local id = param._id

 InitLevel(param)

  local head = Good.FindChild(id, 'head')
  Good.SetPos(head, 0, 0)

  local body = Good.FindChild(id, 'body')
  Good.AddChild(body, head)
  Good.SetScript(body, 'Body')

  Good.AddChild(id, body) -- make sure weeder is top most

  gameover, finish, running = false, false, false
end

Level.OnStep = function(param)
  if (gameover) then
    if (Input.IsKeyPressed(Input.RETURN + Input.BTN_A)) then
      Good.GenObj(-1, level)
    end
  end

  if (finish) then
    if (Input.IsKeyPressed(Input.RETURN + Input.BTN_A)) then
      level = Resource.GetNextLevelId(level)
      if (0 < level) then
        Good.GenObj(-1, level)
      end
    end
  end
end
