print("LuaSCard")

local card = pcsc_card

readers = card.list_readers()
print(readers)

msg = card.connect_reader("O2Micro CCID SC Reader 0")
print(msg)

atr = card.get_atr()
print("ATR " .. atr)


sw, result = card.send("00 A4 ")
print(string.format("sw = %x", sw)) 
print(result)

print("stringify_sw = " .. card.stringify_sw(sw))
	
card.warm_reset()
	
local loop = 0
for i=0,loop do
	sw, result = card.send("00 A4 04 00 08 A0 00 00 00 03 00 00 00 00")
	print(string.format("sw = %x", sw)) 
	print("stringify_sw = " .. card.stringify_sw(sw))
	print("Result = ", tostring(result))
end
--card.send("00 A4 04 00 08 A0 00 00 00 03 00 00 00 00")
card.warm_reset()

msg=card.disconnect_reader()
print("Card ".. msg)
