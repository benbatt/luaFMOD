--[[
This sample requires the following .bank files from the FMOD Engine Windows package:
  * Master.bank
  * Master.strings.bank
  * Music.bank
  * SFX.bank
  * Vehicles.bank
--]]

package.cpath = package.cpath .. ";..\\bin\\?.dll;..\\external\\luaTextLoop\\bin\\?.dll"

require("luaFMOD")
require("luaTextLoop")

local system = FMOD.Studio.System.create()

local coreSystem = assert(system:getCoreSystem())
coreSystem:setSoftwareFormat(0, FMOD.SPEAKERMODE['5POINT1'], 0)

system:initialize(1024, FMOD.Studio.INIT.LIVEUPDATE, FMOD.INIT.NORMAL)

local attributes = FMOD._3D_ATTRIBUTES.new()
attributes.forward.z = 1
attributes.up.y = 1
assert(system:setListenerAttributes(0, attributes))

local bankPaths = {
  "Master.bank",
  "Master.strings.bank",
  "Music.bank",
  "SFX.bank",
  "Vehicles.bank",
}

for _,path in ipairs(bankPaths) do
  local bank = assert(system:loadBankFile(path, FMOD.Studio.LOAD_BANK.NORMAL))
end

local ambience = assert(system:getEvent("event:/Ambience/Forest"))
local ambienceInstance = assert(ambience:createInstance())

assert(ambienceInstance:start())

local cancel = assert(system:getEvent("event:/UI/Cancel"))
local cancelInstance = assert(cancel:createInstance())

local explosion = assert(system:getEvent("event:/Weapons/Explosion"))
assert(explosion:loadSampleData())

local mower = assert(system:getEvent("event:/Vehicles/Ride-on Mower"))
local mowerInstance = assert(mower:createInstance())

assert(mowerInstance:setParameterByName("RPM", 650))

local mowerAttributes = FMOD._3D_ATTRIBUTES.new()
mowerAttributes.forward.z = 1
mowerAttributes.up.y = 1
mowerAttributes.position.z = 2
assert(mowerInstance:set3DAttributes(mowerAttributes))

local footsteps = assert(system:getEvent("event:/Character/Player Footsteps"))

local surface = footsteps:getParameterDescriptionByName("Surface")

local footstepsInstance = assert(footsteps:createInstance())

local surfaceValue = 1
assert(footstepsInstance:setParameterByID(surface.id, surfaceValue))

local music = assert(system:getEvent("event:/Music/Level 01"))
local musicInstance = assert(music:createInstance())

assert(musicInstance:setCallback(
  function(type, event, parameters)
    print(string.format("Music callback: type = %s", tostring(type)))

    local userData = event:getUserData()

    print(string.format("  message %d is \"%s\"", userData.count, userData.messages[userData.count]))

    userData.count = (userData.count % #userData.messages) + 1
  end))

assert(musicInstance:setUserData{
    sound = "test.wav",
    count = 1,
    messages = {
      "one",
      "two",
      "three",
    },
  })

local menu = {
  "=============== FMOD Example ===============",
  ". A: toggle Ambience | R: toggle Rain      .",
  ". C: play Cancel     | E: play Explosion   .",
  ". F: play Footsteps  | +/-: change surface .",
  ". M: toggle Mower    | U: toggle Music     .",
  ". Left/Right/Up/Down: move Mower           .",
  ". Escape: quit                             .",
  "============================================",
}

menu.x = 80 - #menu[1]

TextLoop.setOverlay(menu)

local ambienceStarted = true
local rain = false
local mowerStarted = false
local musicStarted = false

TextLoop.start(10, function(keyCodes)
    for _,key in ipairs(keyCodes) do
      local KeyCode = TextLoop.KeyCode

      if key == KeyCode.Escape then
        return false
      elseif key == KeyCode.C then
        print("Playing Cancel")
        assert(cancelInstance:start())
      elseif key == KeyCode.E then
        print("Playing Explosion")
        local instance = assert(explosion:createInstance())
        assert(instance:start())
        assert(instance:release())
      elseif key == KeyCode.F then
        print("Playing Footsteps")
        assert(footstepsInstance:start())
      elseif key == KeyCode.Plus then
        surfaceValue = math.min(surfaceValue + 1, surface.maximum)
        print(string.format("Setting surface to %d", surfaceValue))
        assert(footstepsInstance:setParameterByID(surface.id, surfaceValue))
      elseif key == KeyCode.Minus then
        surfaceValue = math.max(surface.minimum, surfaceValue - 1)
        print(string.format("Setting surface to %d", surfaceValue))
        assert(footstepsInstance:setParameterByID(surface.id, surfaceValue))
      elseif key == KeyCode.M then
        if mowerStarted then
          print("Stopping mower")
          assert(mowerInstance:stop(FMOD.Studio.STOP.ALLOWFADEOUT))
        else
          print(string.format("Starting mower at (%.f, %.f)", mowerAttributes.position.x, mowerAttributes.position.z))
          assert(mowerInstance:start())
        end

        mowerStarted = not mowerStarted
      elseif key == KeyCode.Left then
        mowerAttributes.position.x = mowerAttributes.position.x - 1
        print(string.format("Moving mower to (%.f, %.f)", mowerAttributes.position.x, mowerAttributes.position.z))
        assert(mowerInstance:set3DAttributes(mowerAttributes))
      elseif key == KeyCode.Right then
        mowerAttributes.position.x = mowerAttributes.position.x + 1
        print(string.format("Moving mower to (%.f, %.f)", mowerAttributes.position.x, mowerAttributes.position.z))
        assert(mowerInstance:set3DAttributes(mowerAttributes))
      elseif key == KeyCode.Up then
        mowerAttributes.position.z = mowerAttributes.position.z + 1
        print(string.format("Moving mower to (%.f, %.f)", mowerAttributes.position.x, mowerAttributes.position.z))
        assert(mowerInstance:set3DAttributes(mowerAttributes))
      elseif key == KeyCode.Down then
        mowerAttributes.position.z = mowerAttributes.position.z - 1
        print(string.format("Moving mower to (%.f, %.f)", mowerAttributes.position.x, mowerAttributes.position.z))
        assert(mowerInstance:set3DAttributes(mowerAttributes))
      elseif key == KeyCode.U then
        if musicStarted then
          print("Stopping music")
          assert(musicInstance:stop(FMOD.Studio.STOP.ALLOWFADEOUT))
        else
          print("Starting music")
          assert(musicInstance:start())
        end

        musicStarted = not musicStarted
      elseif key == KeyCode.R then
        rain = not rain
        print(string.format("Turning rain %s", rain and "on" or "off"))
        assert(ambienceInstance:setParameterByName("Rain", rain and 1 or 0))
      elseif key == KeyCode.A then
        if ambienceStarted then
          print("Stopping ambience")
          assert(ambienceInstance:stop(FMOD.Studio.STOP.ALLOWFADEOUT))
        else
          print("Starting ambience")
          assert(ambienceInstance:start())
        end

        ambienceStarted = not ambienceStarted
      end
    end

    system:update()

    return true
  end)

system:release()
