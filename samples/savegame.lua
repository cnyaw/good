
function LoadPos()

  local inf = io.open("savegame.sav", "r")
  if (nil == inf) then
    return 0, 0
  end

  local i = inf:read("*a")
  inf:close()

  return i % 10000, math.floor(i / 10000)
end

function SavePos(x, y)
  local outf = io.open("savegame.sav", "w")
  outf:write(x + 10000 * y)
  outf:close()
end

Level = {}

local box

Level.OnCreate = function(param)
  local str = GenStrObj(-1, 3, 26, 'DragMe')
  box = Good.FindChild(param._id, 'box')
  Good.AddChild(box, str)

  local x, y = LoadPos()
  if (0 ~= x and 0 ~= y) then
    Good.SetPos(box, x, y)
  end
end

local dragging = false
local ofx, ofy

Level.OnStep = function(param)
  local mx, my = Input.GetMousePos()
  local x, y = Good.GetPos(box)

  if (not dragging) then
    if (Input.IsKeyDown(Input.LBUTTON) and PtInRect(mx, my, x, y, x + 100, y + 100)) then
      dragging = true
      ofx = x - mx
      ofy = y - my
    end
  else
    if (not Input.IsKeyDown(Input.LBUTTON)) then
      dragging = false
      SavePos(Good.GetPos(box))
    else
      Good.SetPos(box, mx + ofx, my + ofy)
    end
  end

end
