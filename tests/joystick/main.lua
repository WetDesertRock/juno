-- if love then
--   function love.load()
--     joystick = nil
--   end
--
--
--   function love.update(dt)
--     if love.joystick.getJoystickCount() >= 1 and joystick == nil then
--       joystick = love.joystick.getJoysticks( )[1]
--       print(string.format("Opening joystick with name %s reported id is %d %d",joystick:getName(),joystick:getID()))
--     end
--   end
--
--
--   function love.joystickhat(...)
--     print(...)
--   end
-- end
function juno.onLoad()
  for _,joystick in ipairs(juno.joystick.joysticks) do
    print(string.format("Joystick with name '%s' reported id is %d",juno.joystick.getName(joystick:index()),joystick:index()))
    print(string.format("  Axes count: %d",joystick:getAxisCount()))
    print(string.format("  Hat count: %d",joystick:getHatCount()))
    print(string.format("  Ball count: %d",joystick:getBallCount()))
    print(string.format("  Button count: %d",joystick:getButtonCount()))
  end
  juno.joystick.open(22)
end

function juno.onDraw()
  -- sdf
end

function juno.onJoyHatMotion(joystick,hat,state)
  print("Hat Motion:",joystick:index(),hat)
  for d,v in pairs(state) do
    print("  ",d,v)
  end
end
