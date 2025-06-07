define NORTHWIND_GUARDIAN 20805 -- Northwind Guardian
define WARP_SCROLL 39043 -- Scroll of the Location
define CONFUCIUS 20809 -- Confucius
define THIRD_HAND_3D 76053 -- Third Hand (3d)
define EXP_REWARD 250000

quest northwind_lost_souls begin
	state run begin
		when letter begin
			local v = find_npc_by_vnum(NORTHWIND_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(NORTHWIND_GUARDIAN))
			end
			send_letter(c_locale(13099)) -- Lost Souls
		end

		when button or info begin
			say_title(c_locale(13099)) -- Lost Souls
			say(c_locale(13100)) -- The Northwind Guardian is looking for you.
		end

		-- Lost Souls
		when NORTHWIND_GUARDIAN.chat.c_locale(13099) begin
			target.delete("__TARGET__")

			say_title(c_locale(13076)) -- Northwind Guardian
			-- There you are. I warned people. The souls in Northwind Canyon are getting more and more riled up.
			-- I'm not sure what we can do, to tell the truth. It's never been this bad...
			say(c_locale(13101))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- Please help us. Report what's going on here to Confucius and that we need his advice.
			-- Here, take this. It'll make your journey easier.
			say(c_locale(13102))
			wait()

			if not pc.enough_inventory(GLIMMERSTONE) then
				say_title(c_locale(13076)) -- Northwind Guardian
				say(c_locale(13080)) -- Your inventory is full.
				return
			end

			pc.give_item2(WARP_SCROLL, 1)

			say_title(c_locale(13076)) -- Northwind Guardian
			say_item_vnum(WARP_SCROLL) -- Scroll of the Location
			say(c_locale(13103)) -- You have received a Scroll of the Location.

			-- I have to report what's happening in Northwind Canyon to Confucius.
			notice_multiline(c_locale(13104), notice)

			set_state("goto_confucius")
		end
	end

	state goto_confucius begin
		when letter begin
			local v = find_npc_by_vnum(CONFUCIUS)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(CONFUCIUS))
			end
			send_letter(c_locale(13099)) -- Lost Souls
		end

		when button or info begin
			say_title(c_locale(13099)) -- Lost Souls
			say(c_locale()) -- The Northwind Guardian wants you to report to Confucius.
		end

		-- Lost Souls
		when NORTHWIND_GUARDIAN.chat.c_locale(13099) begin
			-- Report to Confucius as quickly as possible.
			say(c_locale(13634))
		end

		-- Lost Souls
		when CONFUCIUS.chat.c_locale(13099) begin
			target.delete("__TARGET__")

			say_title(c_locale(13106)) -- Confucius
			-- Ah, there you are again. You've been gone a long time. What happened?
			say(c_locale(13107))

			say_title(c_locale(13108)) -- Confucius listens to the report.
			-- Hmm... I understand. That reminds me of an old legend I heard about Northwind Canyon.
			say(c_locale(13109))
			wait()

			say_title(c_locale(13106)) -- Confucius
			-- I fear I know who could be responsible for this trouble.
			-- Let us hope that I am wrong. In the canyon you'll find Northwind Shelter, the site of a fierce battle...
			-- Take this letter to the Northwind Guardian.
			-- It details how this place can be found and what you need to watch out for.
			say(c_locale(13110))

			-- I've received a letter from Confucius and am supposed to take it to the Northwind Guardian.
			notice_multiline(c_locale(13111), notice)
			set_state("goto_northwind_guardian")
		end
	end

	state goto_northwind_guardian begin
		when letter begin
			local v = find_npc_by_vnum(NORTHWIND_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(NORTHWIND_GUARDIAN))
			end
			send_letter(c_locale(13099)) -- Lost Souls
		end

		when button or info begin
			say_title(c_locale(13099)) -- Lost Souls
			say(c_locale(13112)) -- I should get this letter to the Northwind Guardian as soon as I can.
		end

		-- Lost Souls
		when CONFUCIUS.chat.c_locale(13099) begin
			say_title(c_locale(13106)) -- Confucius
			-- Please take this letter to the Northwind Guardian.
			say(c_locale(13635))
		end

		-- Lost Souls
		when NORTHWIND_GUARDIAN.chat.c_locale(13099) begin
			target.delete("__TARGET__")

			say_title(c_locale(13076)) -- Northwind Guardian
			-- And? What did Confucius say? A letter? Wait, let me read it through quickly.
			say(c_locale(13113))

			set_state("reward")
		end
	end

	state reward begin
		when letter begin
			local v = find_npc_by_vnum(NORTHWIND_GUARDIAN)
			if v ~= 0 then
				target.vid("__TARGET__", v, c_mob_name(NORTHWIND_GUARDIAN))
			end
			send_letter(c_locale(13099)) -- Lost Souls
		end

		when button or info begin
			say_title(c_locale(13099)) -- Lost Souls
			say(c_locale(13114)) -- Return to the Northwind Guardian.
		end

		when NORTHWIND_GUARDIAN.chat.c_locale(13099) begin
			target.delete("__TARGET__")

			say_title(c_locale(13076)) -- Northwind Guardian
			-- Oh... That does not sound good at all. You have to carry out a critical mission for us.
			say(c_locale(13115))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- Get yourself good and ready. You'll have to forge deeper into the canyon than anyone has done in centuries.
			say(c_locale(13116))
			wait()

			say_title(c_locale(13076)) -- Northwind Guardian
			-- Here, this is a reward for your efforts.
			say(c_locale(13117))
			wait()

			if not pc.enough_inventory(GLIMMERSTONE) then
				say_title(c_locale(13076)) -- Northwind Guardian
				say(c_locale(13080)) -- Your inventory is full.
				return
			end

			pc.give_item2(THIRD_HAND_3D, 1)
			pc.give_conqueror_exp2(EXP_REWARD)

			say_title(c_locale(13076)) -- Northwind Guardian
			say_item_vnum(THIRD_HAND_3D) -- Third Hand (3d)
			say(c_locale()) -- You have received Third Hand.

			set_state("__COMPLETE__")
			set_quest_state("northwind_canyon_depths", "run")
		end

		when leave begin
			q.done()
		end
	end

	state __COMPLETE__ begin
	end
end
