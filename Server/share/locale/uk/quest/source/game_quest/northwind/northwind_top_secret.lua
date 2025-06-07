define SOUL_GORGE_GUARDIAN 10142 -- Soul Gorge Guardian
define KILL_COUNT 300
define REWARD_ITEM 39028 -- Enchant Item
define REWARD_ITEM_COUNT 1
define REWARD_EXP 410000

quest northwind_top_secret begin
	state run begin
		when letter begin
			local v = find_npc_by_vnum(SOUL_GORGE_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(SOUL_GORGE_GUARDIAN))
			end
			send_letter(c_locale(13154)) -- Top Secret
		end

		when button or info begin
			say_title(c_locale(13154)) -- Top Secret
			say(c_locale(13155)) -- The Soul Gorge Guardian is looking for you.
		end

		-- Top Secret
		when NORTHWIND_GUARDIAN.chat.c_locale(13154) begin
			target.delete("__TARGET__")

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- There are more and more monsters... And this terrible cold on top of it.
			say(c_locale(13156))
			wait()

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- Frenzied creatures are slipping out of the Soul Gorge in every greater numbers.
			-- Please, help me stop them. It doesn't bear thinking about what will happen if they manage to escape out of the canyon...
			-- Things have been getting worse and worse for some time.
			say(c_locale(13157))
			wait()

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- Please kill 300 monsters in the area so the people feel safe again.
			say(c_locale(13158))

			-- Kill 300 monsters for the Soul Gorge Guardian.
			notice_multiline(c_locale(13159), notice)
			set_state("kill_monsters")
		end
	end

	state kill_monsters begin
		when enter begin
			pc.setqf("kill_count", 0)
		end

		when letter begin
			send_letter(c_locale(13154)) -- Top Secret
			q.set_counter(c_locale(13087), KILL_COUNT - pc.getqf("kill_count")) -- Monsters Remaining
		end

		when button or info begin
			say_title(c_locale(13154)) -- Top Secret
			say(c_locale(13155)) -- The Soul Gorge Guardian is looking for you.
		end

		-- Top Secret
		when NORTHWIND_GUARDIAN.chat.c_locale(13154) begin
			say_title(c_locale(13149)) -- Soul Gorge Guardian
			say(c_locale(13160)) -- Please hurry! Don't let them get away!
		end

		when kill begin
			pc.setqf("kill_count", pc.getqf("kill_count") + 1)

			local count = pc.getqf("kill_count")
			q.set_counter(c_locale(13087), KILL_COUNT - count) -- Monsters Remaining

			-- You have killed %d monsters. Monsters remaining: %d
			syschat(c_locale(13658, count, KILL_COUNT - count))

			if count >= KILL_COUNT then

				-- I've killed all the monsters and should return to the Soul Gorge Guardian.
				notice_multiline(c_locale(13161), notice)

				set_state("reward")
			end
		end
	end

	state reward begin
		when letter begin
			local v = find_npc_by_vnum(SOUL_GORGE_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(SOUL_GORGE_GUARDIAN))
			end
			send_letter(c_locale(13154)) -- Top Secret
		end

		when button or info begin
			say_title(c_locale(13154)) -- Top Secret
			-- I've killed all 300 monsters just like the Soul Gorge Guardian asked me to.
			-- I should go back to him.
			say(c_locale(13162)) -- The Soul Gorge Guardian is looking for you.
		end

		-- Top Secret
		when NORTHWIND_GUARDIAN.chat.c_locale(13154) begin
			target.delete("__TARGET__")

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			-- I saw you coming. Thank you so much! Together we've been able to stop them for now.
			say(c_locale(13163))
			wait()

			if not pc.enough_inventory(REWARD_ITEM) then
				say_title(c_locale(13149)) -- Soul Gorge Guardian
				say(c_locale(13080)) -- Your inventory is full.
				return
			end

			pc.give_item2(REWARD_ITEM, REWARD_ITEM_COUNT)
			pc.give_conqueror_exp2(REWARD_EXP)

			say_title(c_locale(13149)) -- Soul Gorge Guardian
			say_item_vnum(REWARD_ITEM) -- Glimmerstone
			say(c_locale(13164)) -- You earned 1x Enchant Item as a quest reward.

			set_state("__COMPLETE__")
			set_quest_state("northwind_shelter", "run")
		end
	end

	state __COMPLETE__ begin
	end
end
