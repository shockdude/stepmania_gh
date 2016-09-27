local skin_name= Var("skin_name")
-- Thank you Kyzentun for your continual improvements to the newskins
return function(button_list, stepstype)
	local tap_redir= {
		Fret1= "Fret1", Fret2= "Fret2", Fret3= "Fret3", Fret4= "Fret4", 
		Fret5= "Fret5", Fret6= "Fret1", StrumDown= "Strum", Left= "Fret1",
		Down= "Fret2", Up= "Fret3", Right= "Fret4"
	}
	local tap_width= {
		Fret1= 52, Fret2= 52, Fret3= 52, Fret4= 52, 
		Fret5= 52, Fret6= 52, StrumDown= 260, Left= 58,
		Down= 58, Up= 58, Right= 58
	}
	local hold_redir= {
		Fret1= "Fret", Fret2= "Fret", Fret3= "Fret", Fret4= "Fret", 
		Fret5= "Fret", Fret6= "Fret", StrumDown= "Strum", Left= "Fret",
		Down= "Fret", Up= "Fret", Right= "Fret"
	}
	local x_reposition= {
		Fret1= -104, Fret2= -52, Fret3= 0, Fret4= 52, Fret5= 104, StrumDown= 0,
		Left= -87, Down= -29, Up= 29, Right= 87
	}
	local parts_per_beat= 48
	local tap_state_map= {
		parts_per_beat= parts_per_beat, quanta= {
			{per_beat= 1, states= {1}}, -- One state to rule them all
		},
	}
	local lift_state_map= {
		parts_per_beat= parts_per_beat, quanta= {
			{per_beat= 1, states= {1}}, -- One state to find them
		},
	}
	local hold_length= {
		start_note_offset= -.5,
		end_note_offset= .5,
		head_pixs= 32,
		body_pixs= 64,
		tail_pixs= 32
	}
	-- Mines only have a single frame in the graphics.
	local mine_state_map= {
		parts_per_beat= 1, quanta= {
			{per_beat= 1, states= {1}}, -- One state to bring them all
		},
	}
	local active_state_map= {
		parts_per_beat= parts_per_beat, quanta= {
			{per_beat= 1, states= {1}}, -- And in the darkness
		},
	}
	local inactive_state_map= {
		parts_per_beat= parts_per_beat, quanta= {
			{per_beat= 1, states= {2}}, -- Bind them
		},
	}
	local columns= {}
	for i, button in ipairs(button_list) do
		local hold_tex= tap_redir[button].." Hold 2x1.png"
		local roll_tex= tap_redir[button].." Roll 2x1.png"
		columns[i]= {
			width= tap_width[button],
			anim_time= 1,
			anim_uses_beats= true,
			padding= 0,
			custom_x= x_reposition[button],
			hold_gray_percent= .25,
			use_hold_heads_for_taps_on_row= false,
			taps= {
				NoteSkinTapPart_Tap= {
					state_map= tap_state_map,
					actor= Def.Sprite{Texture= tap_redir[button].." Tap Note.png",
					InitCommand= function(self) self:rotationz(0) end}
					},
				NoteSkinTapPart_Mine= {
					state_map= mine_state_map,
					actor= Def.Sprite{Texture= "mine"}},
				NoteSkinTapPart_Lift= { -- fuck lifts
					state_map= lift_state_map,
					actor= Def.Sprite{Texture= tap_redir[button].." Tap HOPO.png",
						InitCommand= function(self) self:rotationz(0) end}},
				NoteSkinTapPart_Gem= {
					state_map= tap_state_map,
					actor= Def.Sprite{Texture= tap_redir[button].." Tap Gem.png",
						InitCommand= function(self) self:rotationz(0) end}},
				NoteSkinTapPart_HOPO= {
					state_map= lift_state_map,
					actor= Def.Sprite{Texture= tap_redir[button].." Tap HOPO.png",
						InitCommand= function(self) self:rotationz(0) end}},
			},
			holds= {
				TapNoteSubType_Hold= {
					{
						state_map= inactive_state_map,
						textures= {hold_tex},
						flip= "TexCoordFlipMode_None",
						length_data= hold_length,
					},
					{
						state_map= active_state_map,
						textures= {hold_tex},
						flip= "TexCoordFlipMode_None",
						length_data= hold_length,
					},
				},
				TapNoteSubType_Roll= {
					{
						state_map= inactive_state_map,
						textures= {roll_tex},
						flip= "TexCoordFlipMode_None",
						length_data= hold_length,
					},
					{
						state_map= active_state_map,
						textures= {roll_tex},
						flip= "TexCoordFlipMode_None",
						length_data= hold_length,
					},
				},
			},
		}
		columns[i].reverse_holds= columns[i].holds
	end
	return {
		columns= columns,
		vivid_operation= true, -- output 200%
	}
end
