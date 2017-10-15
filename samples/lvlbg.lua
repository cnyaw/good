
local addred = true
local red = 0

Level = {}

Level.OnStep = function(param)
  if (addred) then
    red = red + 1
    if (128 == red) then
      addred = false
    end
  else
    red = red - 1
    if (0 == red) then
      addred = true
    end
  end

  Good.SetBgColor(param._id, 0xff008080 + red * 0xffff)
end