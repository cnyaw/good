Level = {}

Level.OnCreate = function(param)
  Stge.RunScript('stge1')
end

Level.OnNewParticle = function(param, particle)
  local u1 = Stge.GetUserData(particle, 0)
  local o
  if (1 == u1) then
    o = GenColorObj(-1, 8, 8, 0xff00ff00)
  else
    o = GenColorObj(-1, 8, 8, 0xffff0000)
  end
  Stge.BindParticle(particle, o)
end

Level.OnKillParticle = function(param, particle)
  Good.KillObj(Stge.GetParticleBind(particle))
end
