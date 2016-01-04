-- Demo of using bytes library

local var1 = bytes.new(8)
print(var1)

local var2 = bytes.new(8,"0B 79 52 40 CB 70 49 B0 1C 19 B3 3E 32 80 4F 0B")
print(var2)

local var3 = bytes.concat(var2, "AABBCCDDEEFF")
print (var3)

local bytes1 = "11 22 33"
local bytes2 = "11 22 33"

if (bytes1 == bytes2) then
print("equal")
else
print("not equal")
end