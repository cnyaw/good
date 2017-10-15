
DoMiSo = {}

DoMiSo.OnCreate = function(param)
  local id = param._id
  param.Do = Good.FindChild(id, 'do')
  param.Mi = Good.FindChild(id, 'mi')
  param.So = Good.FindChild(id, 'so')
end

DoMiSo.OnStep = function(param)
  if (Input.IsKeyPressed(Input.LBUTTON)) then
    local x,y = Input.GetMousePos()
    local o = Good.PickObj(x, y, Good.COLBG)
    if (o == param.Do) then
      Sound.PlaySound(Resource.GetSoundId('do'))
    elseif (o == param.Mi) then
      Sound.PlaySound(Resource.GetSoundId('mi'))
    elseif (o == param.So) then
      Sound.PlaySound(Resource.GetSoundId('so'))
    end
  end
end
