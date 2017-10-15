local AA = Graphics.GetAntiAlias()
local SysFontText = nil

function GenSysFontText()
  if (nil ~= SysFontText) then
    Good.KillObj(SysFontText)
    SysFontText = nil
  end
  SysFontText = Good.GenDummy(-1)
  local s = string.format('SYSTEM FONT string test: anti-alias=%d', AA)
  local str2 = Good.GenTextObj(SysFontText, s, 20)
  Good.SetPos(str2, 1, 80)
  str2 = Good.GenTextObj(SysFontText, '測試开始', 128)
  Good.SetPos(str2, 1, 100)
  str2 = Good.GenTextObj(SysFontText, '텍스트 테스트', 64)
  Good.SetPos(str2, 1, 230)
  str2 = Good.GenTextObj(SysFontText, 'これはテストのテキストです', 28)
  Good.SetPos(str2, 1, 300)
end

Level = {}

Level.OnCreate = function(param)
  -- Bitmap font.
  local str1 = GenStrObj(-1, 1, 1, 'BITMAP FONT string test', 0)

  -- System font.
  GenSysFontText()
end

Level.OnStep = function(param)
  if (Input.IsKeyPushed(Input.LBUTTON)) then
    if (0 == AA) then
      AA = 1
    else
      AA = 0
    end
    Graphics.SetAntiAlias(AA)
    GenSysFontText()
  end
end
