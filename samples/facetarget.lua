local rot_obj_id = 2
local blue_obj_id = 3

local new_dir
local last_tx, last_ty

function AcAnimDone(param)
  param.k = nil
  Good.SetScript(param._id, '')
end

AnimRot = {}

AnimRot.OnStep = function(param)
  if (nil == param.k) then
    local loop1 = ArAddLoop(nil)
    ArAddMoveTo(loop1, 'Rot', 0.5, new_dir)
    ArAddCall(loop1, 'AcAnimDone', 0)
    param.k = ArAddAnimator({loop1})
  else
    ArStepAnimator(param, param.k)
  end
end

function CalcNewDir(tx, ty)
  local ox, oy = Good.GetPos(rot_obj_id)
  local a = math.atan2(tx - ox, oy - ty)
  new_dir = a * 180 / math.pi
  if (0 > new_dir) then
    new_dir = new_dir + 360
  end
  local new_rot_sign = RotSign(last_tx, last_ty, tx - ox, ty - oy)
  last_tx = tx - ox
  last_ty = ty - oy
  local dir = Good.GetRot(rot_obj_id)
  if (180 < math.abs(dir - new_dir)) then
    if (1 == new_rot_sign) then
      new_dir = new_dir + 360
    else
      new_dir = new_dir - 360
    end
  end
end

function FaceToTarget(tx, ty)
  CalcNewDir(tx, ty)
  Good.SetScript(rot_obj_id, 'AnimRot')
end

FaceTarget = {}

FaceTarget.OnCreate = function(param)
  local l,t,w,h = Good.GetDim(blue_obj_id)
  param.bw = w
  param.bh = h
  local ox, oy = Good.GetPos(rot_obj_id)
  last_tx = 0
  last_ty = -1
  FaceToTarget(Good.GetPos(blue_obj_id))
end

FaceTarget.OnStep = function(param)
  if (Input.IsKeyPushed(Input.LBUTTON)) then
    AcAnimDone(Good.GetParam(rot_obj_id))
    local x, y = Input.GetMousePos()
    local tx, ty = x - param.bw/2, y - param.bh/2
    Good.SetPos(blue_obj_id, tx, ty)
    FaceToTarget(tx, ty)
  end
end
