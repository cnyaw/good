local SCR_W, SCR_H = Good.GetWindowSize()

local bullet = Resource.GetSpriteId('bullet')
local bullet2 = Resource.GetSpriteId('bullet2')
local my_bullet = Resource.GetSpriteId('mybullet')
local boss = Resource.GetSpriteId('boss')

Level = {}

Level.OnCreate = function(param)
  param.player = Good.GenObj(-1, Resource.GetSpriteId('player'), 'Player')
  param.task = Stge.RunScript('boss')
end

Level.OnNewParticle = function(param, particle)
  local obj = -1
  local t = Stge.GetUserData(particle, 0)
  if (0 == t) then
    obj = Good.GenObj(-1, bullet)
  elseif (1 == t) then
    obj = Good.GenObj(-1, bullet2)
  elseif (10 == t) then
    obj = Good.GenDummy(-1)
    dummy = Good.GenObj(obj, boss)
  elseif (100 == t) then
    obj = Good.GenObj(-1, my_bullet)
  end
  Stge.BindParticle(particle, obj)
end

Level.OnKillParticle = function(param, particle)
  Good.KillObj(Stge.GetParticleBind(particle))
end

Player = {}

Player.OnCreate = function(param)
  local id = param._id
  Good.SetPos(id, SCR_W/2, 3*SCR_H/4)
  param.fire = -1

  param.dummy = Good.GenDummy(id)

  Stge.SetPlayer(param.dummy)
end

Player.OnStep = function(param)
  local id = param._id
  local x,y = Good.GetPos(id)
  local l,t,w,h = Good.GetDim(id)
  local sx,sy = 2,2

  -- move plane
  if (Input.IsKeyDown(Input.LEFT)) then
    x = x - sx
  elseif (Input.IsKeyDown(Input.RIGHT)) then
    x = x + sx
  end
  if (Input.IsKeyDown(Input.UP)) then
    y = y - sy
  elseif (Input.IsKeyDown(Input.DOWN)) then
    y = y + sy
  end

  if (0 > x) then
    x = 0
  elseif (SCR_W - w < x) then
    x = SCR_W - w
  end
  if (0 > y) then
    y = 0
  elseif (SCR_H - h < y) then
    y = SCR_H - h
  end

  Good.SetPos(id, x,y)

  -- fire bullet
  if (Input.IsKeyDown(Input.BTN_A)) then
    if (-1 == param.fire) then
      param.fire = Stge.RunScript('weapon_1')
      Stge.BindTask(param.fire, param.dummy)
    end
  elseif (-1 ~= param.fire) then
    Stge.KillTask(param.fire)
    param.fire = -1
  end
end
