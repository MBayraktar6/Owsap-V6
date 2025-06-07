-- Compatibility: Lua-5.0
function split(str, delim, maxNb)
	-- Eliminate bad cases...
	if str == nil then
		return str
	end
	if string.find(str, delim) == nil then
		return { str }
	end
	if maxNb == nil or maxNb < 1 then
		maxNb = 0 -- No limit
	end
	local result = {}
	local pat = "(.-)" .. delim .. "()"
	local nb = 0
	local lastPos
	for part, pos in string.gfind(str, pat) do
		nb = nb + 1
		result[nb] = part
		lastPos = pos
		if nb == maxNb then
			break
		end
	end
	-- Handle the last field
	if nb ~= maxNb then
		result[nb + 1] = string.sub(str, lastPos)
	end
	return result
end

function select_random_subtable(from_table, from_key)
	local keys = {}
	for key in pairs(from_table) do
		table.insert(keys, key)
	end

	local subtable_keys = {}
	for key in pairs(from_table[from_key]) do
		table.insert(subtable_keys, key)
	end

	local selected_subtable_key = number(1, table.getn(subtable_keys))
	local selected_subtable = from_table[from_key][selected_subtable_key]

	return selected_subtable
end

table_length = function(t)
	local count = 0
	for _ in pairs(t) do count = count + 1 end
	return count
end
