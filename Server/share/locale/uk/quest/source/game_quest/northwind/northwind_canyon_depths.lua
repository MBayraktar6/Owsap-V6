define NORTHWIND_GUARDIAN 20805 -- Northwind Guardian
define SOUL_GORGE_MAP_INDEX 389
define SOUL_GORGE_GUARDIAN 10142 -- Soul Gorge Guardian
define GLIMMERSTONE 50926 -- Glimmerstone
define GLIMMERSTONE_COUNT 100
define EXP_REWARD 290000

quest northwind_canyon_depths begin
	state run begin
		when letter begin
			local v = find_npc_by_vnum(NORTHWIND_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(NORTHWIND_GUARDIAN))
			end
			send_letter(c_locale(13140)) -- In the Canyon Depths
		end

		when button or info begin
			say_title(c_locale(13140)) -- In the Canyon Depths
			say(c_locale(13141)) -- The Northwind Guardian is looking for you.
		end

		-- In the Canyon Depths
		when NORTHWIND_GUARDIAN.chat.c_locale(13140) begin
			target.delete("__TARGET__")

			say_title(c_locale(13076)) -- Northwind Guardian
			-- Deep in the canyon there's a sinister place we call the Soul Gorge.
			-- Confucius writes that it's important you take a look around there.
			-- I'm guessing he's already sent someone ahead who can explain everything you need to know when you get there.
			say(c_locale(13142))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- We normally keep our distance from the Soul Gorge.
			-- That place is corrupted ever since a fearsome creature was slain there long ago.
			-- That place is dangerous, so make sure you're prepared. When you arrive there,
			-- there should already be someone there who can help you.
			say(c_locale(13143))

			-- Go to the Soul Gorge that the Northwind Guardian told you about.
			notice_multiline(c_locale(13144), notice)
			set_state("goto_soul_gorge_guardian")
		end
	end

	state goto_soul_gorge_guardian begin
		when letter begin
			local v = find_npc_by_vnum(SOUL_GORGE_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(SOUL_GORGE_GUARDIAN))
			end
			send_letter(c_locale(13140)) -- In the Canyon Depths
		end

		when button or info begin
			say_title(c_locale(13140)) -- In the Canyon Depths
			say(c_locale(13148)) -- Find the Soul Gorge Guardian
		end

		when login with pc.get_map_index() == begin
			-- I've reached the Soul Gorge and should look for the Soul Gorge Guardian.
			notice_multiline(c_locale(13145), notice)
		end

		-- In the Canyon Depths
		when NORTHWIND_GUARDIAN.chat.c_locale(13140) begin
			say_title(c_locale(13076)) -- Northwind Guardian
			-- You haven't forgotten what your mission is, have you?
			say(c_locale(13142))
		end

		-- In the Canyon Depths
		when SOUL_GORGE_GUARDIAN.chat.c_locale(13140) begin
			target.delete("__TARGET__")

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- Hey, you! Stop! It's dangerous here. Get out of here, go on!
			say(c_locale(13150))
			wait()

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- Oh, excuse me. You're the one Confucius told me about, right?
			-- He seems to think very highly of you.
			-- Did he tell you a fearsome creature was killed here centuries ago?
			-- That's when it all started.
			say(c_locale(13151))
			wait()

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- I've secured this little patch, so you can take a little moment to rest here.
			say(c_locale(13152))
			wait()

			if not pc.enough_inventory(GLIMMERSTONE) then
				say_title(c_locale(13149)) -- Soul Gorge Guardian
				say(c_locale(13080)) -- Your inventory is full.
				return
			end

			pc.give_item2(GLIMMERSTONE, GLIMMERSTONE_COUNT)
			pc.give_conqueror_exp2(EXP_REWARD)

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			say_item_vnum(GLIMMERSTONE) -- Glimmerstone
			say(c_locale(13153)) -- You earned 100 Glimmerstones as a quest reward.

			set_state("__COMPLETE__")
			set_quest_state("northwind_top_secret", "run")
		end

		when leave begin
			q.done()
		end
	end

	state __COMPLETE__ begin
	end
end
