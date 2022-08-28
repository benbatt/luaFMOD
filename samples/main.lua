--[[
This sample requires the following files from the FMOD Engine Windows package:
  * Master.bank
  * Master.strings.bank
  * SFX.bank
  * Vehicles.bank
  * sequence-one.ogg
  * sequence-two.ogg
  * sequence-three.ogg
  * sequence-four.ogg
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

local dialogue = assert(system:getEvent("event:/Character/Dialogue"))
local dialogueInstance = assert(dialogue:createInstance())

assert(dialogueInstance:setCallback(
  function(type, event, parameters)
    print(string.format("Dialogue callback: type = %s", tostring(type)))

    local userData = event:getUserData()

    if type == FMOD.Studio.EVENT_CALLBACK.CREATE_PROGRAMMER_SOUND then
      print(string.format("  Returning sound %d", userData.count))
      parameters.sound = userData.sounds[userData.count]
      userData.count = (userData.count % #userData.sounds) + 1
    end
  end))

local dialogueData = {
  count = 1,
  sounds = {
    assert(coreSystem:createSound("sequence-one.ogg", FMOD.MODE.DEFAULT)),
    assert(coreSystem:createSound("sequence-two.ogg", FMOD.MODE.DEFAULT)),
    assert(coreSystem:createSound("sequence-three.ogg", FMOD.MODE.DEFAULT)),
    assert(coreSystem:createSound("sequence-four.ogg", FMOD.MODE.DEFAULT)),
  },
}

assert(dialogueInstance:setUserData(dialogueData))

local menu = {
  "=============== FMOD Example ===============",
  ". A: toggle Ambience | R: toggle Rain      .",
  ". C: play Cancel     | E: play Explosion   .",
  ". F: play Footsteps  | +/-: change surface .",
  ". M: toggle Mower    | D: play Dialogue    .",
  ". Left/Right/Up/Down: move Mower           .",
  ". Escape: quit                             .",
  "============================================",
}

menu.x = 80 - #menu[1]

TextLoop.setOverlay(menu)

local ambienceStarted = true
local rain = false
local mowerStarted = false

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
      elseif key == KeyCode.D then
        print("Playing dialogue")
        assert(dialogueInstance:start())
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
