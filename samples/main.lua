--[[
This sample requires the following .bank files from the FMOD Engine Windows package:
  * Master.bank
  * Master.strings.bank
  * SFX.bank
  * Vehicles.bank
--]]

package.cpath = package.cpath .. ";..\\bin\\?.dll;..\\external\\luaTextLoop\\bin\\?.dll"

require("luaFMOD")
require("luaTextLoop")

print("FMOD Test")

local system

local ambienceInstance
local cancelInstance
local explosion
local mowerInstance
local mowerAttributes
local footstepsInstance
local surface
local surfaceValue

local ambienceStarted = true
local rain = false

local function quit()
  if system then
    system:release()
  end
  os.exit()
end

local KeyCode = TextLoop.KeyCode

local function handleKey(key)
end

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
ambienceInstance = assert(ambience:createInstance())

assert(ambienceInstance:start())

local cancel = assert(system:getEvent("event:/UI/Cancel"))
cancelInstance = assert(cancel:createInstance())

explosion = assert(system:getEvent("event:/Weapons/Explosion"))
assert(explosion:loadSampleData())

local mower = assert(system:getEvent("event:/Vehicles/Ride-on Mower"))
mowerInstance = assert(mower:createInstance())

assert(mowerInstance:setParameterByName("RPM", 650))

mowerAttributes = FMOD._3D_ATTRIBUTES.new()
mowerAttributes.forward.z = 1
mowerAttributes.up.y = 1
mowerAttributes.position.z = 2
assert(mowerInstance:set3DAttributes(mowerAttributes))

local footsteps = assert(system:getEvent("event:/Character/Player Footsteps"))

surface = footsteps:getParameterDescriptionByName("Surface")
surfaceID = surface.id

local guid = surface.guid
print("Parameter GUID:", guid.Data1, guid.Data2, guid.Data3,
  guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
  guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7])

print("Parameter type:", surface.type)
print("Parameter flags:", surface.flags)
print("Parameter is discrete?", tostring(surface.flags * FMOD.Studio.PARAMETER_FLAGS.DISCRETE))
print("Parameter is readonly?", tostring(surface.flags * FMOD.Studio.PARAMETER_FLAGS.READONLY))

footstepsInstance = assert(footsteps:createInstance())

surfaceValue = 1
assert(footstepsInstance:setParameterByID(surfaceID, surfaceValue))

TextLoop.start(10, function(keyCodes)
    for _,key in ipairs(keyCodes) do
      if key == KeyCode.Escape then
        return false
      elseif key == KeyCode.C then
        assert(cancelInstance:start())
      elseif key == KeyCode.E then
        local instance = assert(explosion:createInstance())
        assert(instance:start())
        assert(instance:release())
      elseif key == KeyCode.F then
        assert(footstepsInstance:start())
      elseif key == KeyCode.Plus then
        surfaceValue = math.min(surfaceValue + 1, surface.maximum)
        assert(footstepsInstance:setParameterByID(surface.id, surfaceValue))
      elseif key == KeyCode.Minus then
        surfaceValue = math.max(surface.minimum, surfaceValue - 1)
        assert(footstepsInstance:setParameterByID(surface.id, surfaceValue))
      elseif key == KeyCode.M then
        assert(mowerInstance:start())
      elseif key == KeyCode.Left then
        mowerAttributes.position.x = mowerAttributes.position.x - 1
        assert(mowerInstance:set3DAttributes(mowerAttributes))
      elseif key == KeyCode.Right then
        mowerAttributes.position.x = mowerAttributes.position.x + 1
        assert(mowerInstance:set3DAttributes(mowerAttributes))
      elseif key == KeyCode.Up then
        mowerAttributes.position.z = mowerAttributes.position.z + 1
        assert(mowerInstance:set3DAttributes(mowerAttributes))
      elseif key == KeyCode.Down then
        mowerAttributes.position.z = mowerAttributes.position.z - 1
        assert(mowerInstance:set3DAttributes(mowerAttributes))
      elseif key == KeyCode.R then
        rain = not rain
        assert(ambienceInstance:setParameterByName("Rain", rain and 1 or 0))
      elseif key == KeyCode.S then
        if ambienceStarted then
          assert(ambienceInstance:stop(FMOD.Studio.STOP.ALLOWFADEOUT))
        else
          assert(ambienceInstance:start())
        end

        ambienceStarted = not ambienceStarted
      end
    end

    system:update()

    return true
  end)
