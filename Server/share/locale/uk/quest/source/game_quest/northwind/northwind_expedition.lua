define SERPENT_GUARDIAN 20804 -- Serpent Guardian
define NORTHWIND_MAP_INDEX 388
define NORTHWIND_GUARDIAN 20805 -- Northwind Guardian
define GLIMMERSTONE 50926 -- Glimmerstone
define GLIMMERSTONE_COUNT 100

quest northwind_expedition begin
	state run begin
		when letter begin
			local v = find_npc_by_vnum(SERPENT_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(SERPENT_GUARDIAN))
			end
			send_letter(c_locale(13066)) -- Where the Northwind Howls
		end

		when button or info begin
			say_title(c_locale(13066)) -- Where the Northwind Howls
			say(c_locale(13067)) -- The Serpent Guardian is looking for you.
		end

		-- Where the Northwind Howls
		when SERPENT_GUARDIAN.chat.c_locale(13066) begin
			target.delete("__TARGET__")

			say_title(c_locale(13068)) -- Serpent Guardian
			-- There's a dangerous canyon a long way from here.
			-- Can you travel there? The people there are in danger and I wish I could go there and help them.
			-- But my duty is to guard this place.
			say(c_locale(13069))
			wait()

			say_title(c_locale(13068)) -- Serpent Guardian
			-- If you go to the canyon, you'll meet an old friend of mine.
			-- Just tell him that Confucius sent you. He's the reason you came to me too, right?
			say(c_locale(13070))
			wait()

			say_title(c_locale(13068)) -- Serpent Guardian
			-- Please travel to Northwind Canyon on the other side of this valley and speak to the Northwind Guardian.
			-- I'd also be really grateful if you could give him my greetings too.
			say(c_locale(13071))

			-- Go to Northwind Canyon as the Serpent Guardian asked.
			notice_multiline(c_locale(13072), notice)
			set_state("goto_northwind_guardian")
		end
	end

	state goto_northwind_guardian begin
		when letter begin
			local v = find_npc_by_vnum(NORTHWIND_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(NORTHWIND_GUARDIAN))
			end
			send_letter(c_locale(13066)) -- Where the Northwind Howls
		end

		when button or info begin
			say_title(c_locale(13066)) -- Where the Northwind Howls
		end

		when login with pc.get_map_index() == NORTHWIND_MAP_INDEX begin
			-- I've arrived in Northwind Canyon and should look around for the Northwind Guardian.
			notice_multiline(c_locale(13073), notice)
		end

		-- Where the Northwind Howls
		when SERPENT_GUARDIAN.chat.c_locale(13066) begin
			say_title(c_locale(13068)) -- Serpent Guardian
			-- Shouldn't you have left for the Northwind Canyon by now? Please hurry!
			say(c_locale(13633))
		end

		-- Where the Northwind Howls
		when NORTHWIND_GUARDIAN.chat.c_locale(13066) begin
			target.delete("__TARGET__")

			say_title(c_locale(13076)) -- Northwind Guardian
			-- What brings you here to the middle of nowhere? The animals around here are very dangerous,
			-- not to mention the weather in the canyon. If you've got yourself lost, you should turn around quickly.
			say(c_locale(13077))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- Wait, you've come from the Serpent Temple? Did my friend send you by any chance?
			-- True, you look very battle-hardened up close.
			say(c_locale(13078))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- I could definitely do with your help. But in the meantime, take a seat and rest from your journey.
			say(c_locale(13079))
			wait()

			if not pc.enough_inventory(GLIMMERSTONE) then
				say_title(c_locale(13076)) -- Northwind Guardian
				say(c_locale(13080)) -- Your inventory is full.
				return
			end

			pc.give_item2(GLIMMERSTONE, GLIMMERSTONE_COUNT)

			say_title(c_locale(13076)) -- Northwind Guardian
			say_item_vnum(GLIMMERSTONE) -- Glimmerstone
			say(c_locale(13081)) -- You earned 100 Glimmerstones as a quest reward.

			set_state("__COMPLETE__")
			set_quest_state("northwind_frenzied_beasts", "run")
		end

		when leave begin
			q.done()
		end
	end

	state __COMPLETE__ begin
	end
end
