define SOUL_GORGE_GUARDIAN 10142 -- Soul Gorge Guardian
define GENERAL_KOHAN 20808 -- General Kohan

quest northwind_shelter begin
	state run begin
		when letter begin
			local v = find_npc_by_vnum(SOUL_GORGE_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(SOUL_GORGE_GUARDIAN))
			end
			send_letter(c_locale(13165)) -- Northwind Shelter
		end

		-- Northwind Shelter
		when SOUL_GORGE_GUARDIAN.chat.c_locale(13165) begin
			target.delete("__TARGET__")

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- You're back. I think you've earned the right to find out what's going on.
			-- As already know, something happened here a long time ago that changed the canyon.
			say(c_locale(13166))
			wait()

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- A fearsome white dragon once brought great terror upon this region until heroes were finally able to defeat him.
			-- Confucius would like us to look at the spot where he fell. Maybe the cause of all this can be found there.
			-- Will you dare to go there?
			say(c_locale(13167))
			if select(c_locale(13168), c_locale()) == 1 then -- Yes / No
				say_title(c_locale(13149)) -- Soul Gorge Guardian
				-- Thank you, your courage is astounding. The way there is hard and full of dangerous monsters.
				-- I wish you lots of luck. An old friend of mine guards the Northwind Shelter.
				-- Maybe he can help you some more?
				say(c_locale(13170))

				-- Find General Kohan at the Northwind Shelter.
				notice_multiline(c_locale(13171), notice)
				set_state("goto_general_kohan")
			else
				
			end
		end
	end

	state goto_general_kohan begin
		when letter begin
			local v = find_npc_by_vnum(GENERAL_KOHAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(GENERAL_KOHAN))
			end
			send_letter(c_locale(13165)) -- Northwind Shelter
		end

		when button or info begin
			say_title(c_locale(13165)) -- Northwind Shelter
			-- Go to the entrance to the Northwind Shelter and talk to General Kohan there.
			say(c_locale(13173))
		end

		-- Northwind Shelter
		when SOUL_GORGE_GUARDIAN.chat.c_locale(13165) begin
			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- I still haven't heard anything from the General.
			-- Please travel to the Northwind Shelter as quickly as you can.
			say(c_locale(13636))
		end

		-- Northwind Shelter
		when GENERAL_KOHAN.chat.c_locale(13165) begin
			target.delete("__TARGET__")

			say_title(c_locale(13172)) -- The Dutiful General
			-- What are you doing here? My duty is to guard this place.
			-- I don't let anyone in carelessly because it's too dangerous.
			-- And I make sure that nothing finds its way out...

			-- I sent a scout inside a few days ago. But I've heard nothing from him since then.
			-- You willingly came all the way here at Confucius' command? Impressive.

		end
	end
end