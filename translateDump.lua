headerfile = "/usr/include/linux/input.h" 

f = io.open(headerfile)

if not f then error("error opening file: '" .. headerfile .. "'") end

dump = io.open(arg[1])

if not dump then error("usage: " .. arg[-1] .. " " .. arg[0] .. " <filename>") end

t = {}

for line in f:lines() do
   _, _, keyname, scancode = string.find(line, "^#define KEY_([^%s]+)%s+(%d+)")
   if keyname and scancode then
      t[scancode] = keyname
   end
end

s = {}

for line in dump:lines() do
   _, _, key1, key2, key3, amount, latency = string.find(line, "(%d+)%s(%d+)%s(%d+)%s(%d+)%s(%d+)")
   if key1 and key2 and key3 and amount and latency then
      if tonumber(amount) > 0 then
	 if t[key1] and t[key2] and t[key3] then
	    triple = t[key1] .. " " .. t[key2] .. " " .. t[key3]
	    entry = {}
	    entry.triple = triple
	    entry.amount = tonumber(amount)
	    entry.latency = latency
	    table.insert(s, entry)
	 end
      end
   end
end

table.sort(s, function(a,b) return a.amount > b.amount end)

for _,triple in ipairs(s) do
   print(triple.triple,triple.amount,triple.latency)
end