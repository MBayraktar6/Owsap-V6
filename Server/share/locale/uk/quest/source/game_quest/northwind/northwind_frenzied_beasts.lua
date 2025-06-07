define NORTHWIND_GUARDIAN 20805 -- Northwind Guardian
define KILL_COUNT 300
define DRAGON_CHEST 51639 -- Dragon Chest
define DRAGON_CHEST_COUNT 1
define EXP_REWARD 250000

quest northwind_frenzied_beasts begin
	state run begin
		when letter begin
			local v = find_npc_by_vnum(NORTHWIND_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(NORTHWIND_GUARDIAN))
			end
			send_letter(c_locale(13082)) -- Frenzied Beasts
		end

		when button or info begin
			say_title(c_locale(13082)) -- Frenzied Beasts
			say(c_locale(13083)) -- The Northwind Guardian is looking for you.
		end

		-- Frenzied Beasts
		when NORTHWIND_GUARDIAN.chat.c_locale(13082) begin
			target.delete("__TARGET__")

			say_title(c_locale(13076)) -- Northwind Guardian
			-- How are you? Take care that you don't get lost.
			-- The canyon is haunted by the souls of those who froze to death here.
			say(c_locale(13084))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- You came here to help, right? The lost souls sometimes possess animals who then become aggressive.
			-- What if they attack someone? Please[ENTER]kill 300 of them and make the area safe again.
			say(c_locale(13085))

			-- Hunt 300 frenzied beasts in the area.
			notice_multiline(c_locale(13086), notice)
			set_state("frenzied_beasts_hunt")
		end
	end

	state frenzied_beasts_hunt begin
		when enter begin
			pc.setqf("kill_count", 0)
		end

		when letter begin
			send_letter(c_locale(13082)) -- Frenzied Beasts
			q.set_counter(c_locale(13087), KILL_COUNT - pc.getqf("kill_count")) -- Monsters Remaining
		end

		when button or info begin
			say_title(c_locale(13082)) -- Frenzied Beasts
			say(c_locale(13088)) -- Kill frenzied beasts in the area.
			say_reward(c_locale(13089, KILL_COUNT - pc.getqf("kill_count"))) -- Monsters Remaining
		end

		-- Frenzied Beasts
		when NORTHWIND_GUARDIAN.chat.c_locale(13082) begin
			say_title(c_locale(13076)) -- Northwind Guardian
			-- Please kill these beasts as quickly as you can.
			say(c_locale(13090))
		end

		when kill begin
			pc.setqf("kill_count", pc.getqf("kill_count") + 1)

			local count = pc.getqf("kill_count")
			q.set_counter(c_locale(13087), KILL_COUNT - count) -- Monsters Remaining

			-- You have killed %d monsters. Monsters remaining: %d
			syschat(c_locale(13658, count, KILL_COUNT - count))

			if count >= KILL_COUNT then

				-- I killed all of the frenzied beasts for the Northwind Guardian.
				-- I should return to him.
				notice_multiline(c_locale(13091), notice)

				set_state("frenzied_beasts_reward")
			end
		end
	end

	state frenzied_beasts_reward begin
		when letter begin
			local v = find_npc_by_vnum(NORTHWIND_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(NORTHWIND_GUARDIAN))
			end
			send_letter(c_locale(13082)) -- Frenzied Beasts
		end

		when button or info begin
			say_title(c_locale(13082)) -- Frenzied Beasts
			-- All of the frenzied beasts have been killed. Return to the Northwind Guardian.
			say(c_locale(13092))
		end

		-- Frenzied Beasts
		when NORTHWIND_GUARDIAN.chat.c_locale(13082) begin
			target.delete("__TARGET__")

			say_title(c_locale(13076)) -- Northwind Guardian
			-- You're already done? I knew I could count on you.
			-- Tell me everything that happened.
			say(c_locale(13090))
			wait()

			say_pc_name()
			-- Their numbers are growing, aren't they?
			-- The people who live here have noticed that already too.
			say(c_locale(13094))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- They're straying further and further out of the canyon.
			-- Something unholy is happening there.
			say(c_locale(13095))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- We cannot ignore this. The people's safety is at stake.
			-- I'll warn them to keep a greater distance from the canyon.
			say(c_locale(13096))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- You just rest in the meantime and take this as a thank-you for your help.
			say(c_locale(13097))
			wait()

			if not pc.enough_inventory(GLIMMERSTONE) then
				say_title(c_locale(13076)) -- Northwind Guardian
				say(c_locale(13080)) -- Your inventory is full.
				return
			end

			pc.give_item2(DRAGON_CHEST, DRAGON_CHEST_COUNT)
			pc.set_conqueror_exp2(EXP_REWARD)

			say_title(c_locale(13076)) -- Northwind Guardian
			say_item_vnum(DRAGON_CHEST) -- Dragon Chest
			say(c_locale(13098)) -- You have received a Dragon Chest

			set_state("__COMPLETE__")
			set_quest_state("northwind_lost_souls", "lost_souls")
		end

		when leave begin
			q.done()
		end
	end
end
