local bullet = Resource.GetSpriteId('bullet')
local script = {"a", "b", "c"}

Level = {}

Level.OnCreate = function(param)
  local o = GenColorObj(param._id, 30, 40, 0xffffffff)
  Good.SetPos(o, 5, 5)

  local names = {"Typhoon", "Highway", "Sewing machine"}
  for i = 0, 2 do
    GenStrObj(-1, 20, 10 + 40 * i, names[1 + i], 2, 16, 32, 0xffff0000)
  end

  param.o = o
  param.cursor = 0

  param.task = Stge.RunScript('a')
end

Level.OnStep = function(param)
  local newsel = -1
  if (Input.IsKeyPushed(Input.UP) and 0 < param.cursor) then
    param.cursor = param.cursor - 1
    newsel = param.cursor
  end

  if (Input.IsKeyPushed(Input.DOWN) and 2 > param.cursor) then
    param.cursor = param.cursor + 1
    newsel = param.cursor
  end

  if (Input.IsKeyPushed(Input.LBUTTON)) then
    local x, y = Input.GetMousePos()
    for i = 0, 2 do
      if (PtInRect(x, y, 5, 5 + 40 * i, 180, 45 + 40 * i)) then
         if (i ~= param.cursor) then
           param.cursor = i
           newsel = i
         end
         break
      end
    end
  end

  if (-1 ~= newsel) then
    Good.SetPos(param.o, 5, 5 + 40 * param.cursor)
    Stge.KillAllParticle()
    Stge.KillAllTask()
    param.task = Stge.RunScript(script[1 + param.cursor])
  end
end

Level.OnNewParticle = function(param, particle)
  local obj = Good.GenObj(-1, bullet)
  Stge.BindParticle(particle, obj)
end

Level.OnKillParticle = function(param, particle)
  Good.KillObj(Stge.GetParticleBind(particle))
end
