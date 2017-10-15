
Level = {}

Level.OnCreate = function(param)
  param.mario = Good.GenObj(-1, Resource.GetSpriteId('stand_R'), 'Mario')
end

Mario = {}

Mario.OnCreate = function(param)
  local id = param._id

  Good.SetPos(id, 120, 200 - 32)
  param.move = false
  param.jump = false
  param.cntJump = 0 -- jump meter, start inc when press jump
  param.vecJump = 3
  param.dir = 1  -- 0 = left, 1 = right
end

Mario.OnStep = function(param)
  local id = param._id

  -- handle move
  if (Input.IsKeyDown(Input.LEFT)) then
    if (not param.move) then
      param.move = true
      param.dir = 0
      Good.SetSpriteId(id, Resource.GetSpriteId('run_L'))
      Good.PlayAnim(id)
    end
  elseif (Input.IsKeyDown(Input.RIGHT)) then
    if (not param.move) then
      param.move = true
      param.dir = 1
      Good.SetSpriteId(id, Resource.GetSpriteId('run_R'))
      Good.PlayAnim(id)
    end
  else
    if (param.move) then
      if (1 == param.dir) then
        Good.SetSpriteId(id, Resource.GetSpriteId('stand_R'))
      else
        Good.SetSpriteId(id, Resource.GetSpriteId('stand_L'))
      end
      Good.StopAnim(id)
      param.move = false
    end
  end

  -- handle jump
  if (Input.IsKeyDown(Input.BTN_A)) then
    if (not param.jump) then
      param.cntJump = 6
      param.vecJump = -3
      param.jump = true
    else
      if (0 < param.cntJump) then
        param.cntJump = param.cntJump - 1
        param.vecJump = param.vecJump - 0.5
      elseif (3 > param.vecJump) then
        param.vecJump = param.vecJump + 0.5
      end
    end
  else
    if (3 > param.vecJump) then
      param.vecJump = param.vecJump + 1
    else
      param.jump = false
    end
  end

  -- handle move
  local x,y = Good.GetPos(id)

  if (param.move) then
    if (1 == param.dir) then
      x = x + 1
    else
      x = x - 1
    end
    if (0 > x) then
      x = 0
    elseif (256 - 16 < x) then
      x = 256 - 16
    end
  end

  -- handle jump
  y = y + param.vecJump
  if (200 - 32 < y) then
    y = 200 - 32
  end

  local a = Good.PickObj(x, y, Good.SPRITE, 6)
  if (-1 ~= a) then
    local x1,y1 = Good.GetPos(a)
    y = y1 + 16
  end
  a = Good.PickObj(x + 16, y, Good.SPRITE, 6)
  if (-1 ~= a) then
    local x1,y1 = Good.GetPos(a)
    y = y1 + 16
  end

  a = Good.PickObj(x, y + 32, Good.SPRITE, 6)
  if (-1 ~= a) then
    local x1,y1 = Good.GetPos(a)
    y = y1 - 32
  end
  a = Good.PickObj(x + 16, y + 32, Good.SPRITE, 6)
  if (-1 ~= a) then
    local x1,y1 = Good.GetPos(a)
    y = y1 - 32
  end

  Good.SetPos(id, x,y)
end
