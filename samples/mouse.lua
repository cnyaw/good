
local text = nil

function UpdateMessage()
  local x, y = Input.GetMousePos()
  local str = "Mouse: (" .. x .. ", " .. y .. ")"

  if (Input.IsKeyDown(Input.LBUTTON)) then
    str = str .. " LButton"
  end

  if (Input.IsKeyDown(Input.RBUTTON)) then
    str = str .. " RButton"
  end

  if (Input.IsKeyDown(Input.LEFT)) then
    str = str .. " LEFT"
  end

  if (Input.IsKeyDown(Input.RIGHT)) then
    str = str .. " RIGHT"
  end

  if (Input.IsKeyDown(Input.UP)) then
    str = str .. " UP"
  end

  if (Input.IsKeyDown(Input.DOWN)) then
    str = str .. " DOWN"
  end

  if (Input.IsKeyDown(Input.BTN_A)) then
    str = str .. " BTN_A"
  end

  if (Input.IsKeyDown(Input.BTN_B)) then
    str = str .. " BTN_B"
  end

  if (Input.IsKeyDown(Input.RETURN)) then
    str = str .. " ENTER"
  end

  if (nil ~= text) then
    Good.KillObj(text)
  end

  text = GenStrObj(-1, 32, 120, str)
end

Level = {}

Level.OnStep = UpdateMessage
