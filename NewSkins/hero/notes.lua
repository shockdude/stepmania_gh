local skin_name= Var("skin_name")
return function(button_list, stepstype)
	local tap_redir= {
		Fret1= "Fret1", Fret2= "Fret2", Fret3= "Fret3", Fret4= "Fret4", 
		Fret5= "Fret5", Fret6= "Fret1", StrumDown= "Strum", Left= "Fret1",
		Down= "Fret2", Up= "Fret3", Right= "Fret4"
	}
	local tap_width= {
		-- the weird spacing is a work around to have the strum column in the middle and hidden
		Fret1= 52, Fret2= 52, Fret3= 52, Fret4= 52, 
		Fret5= 52, Fret6= 52, StrumDown= 1, Left= 58,
		Down= 58, Up= 58, Right= 58
	}
	local hold_redir= {
		Fret1= "Fret", Fret2= "Fret", Fret3= "Fret", Fret4= "Fret", 
		Fret5= "Fret", Fret6= "Fret", StrumDown= "Strum", Left= "Fret",
		Down= "Fret", Up= "Fret", Right= "Fret"
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
			taps= {
				NewSkinTapPart_Tap= {
					state_map= tap_state_map,
					-- if tap_redir[button] == "Strum" then
						-- actor= Def.Sprite{Texture= "Strum Tap Gem.png",
						-- InitCommand= function(self) self:rotationz(0) end}
					-- else
						actor= Def.Sprite{Texture= tap_redir[button].." Tap Note.png",
						InitCommand= function(self) self:rotationz(0) end}
					-- end
					},
				NewSkinTapPart_Mine= {
					state_map= mine_state_map,
					actor= Def.Sprite{Texture= "mine.png"}},
				NewSkinTapPart_Lift= { -- fuck lifts
					state_map= lift_state_map,
					actor= Def.Sprite{Texture= tap_redir[button].." Tap HOPO.png",
						InitCommand= function(self) self:rotationz(0) end}},
				NewSkinTapPart_Gem= {
					state_map= tap_state_map,
					actor= Def.Sprite{Texture= tap_redir[button].." Tap Gem.png",
						InitCommand= function(self) self:rotationz(0) end}},
				NewSkinTapPart_HOPO= {
					state_map= lift_state_map,
					actor= Def.Sprite{Texture= tap_redir[button].." Tap HOPO.png",
						InitCommand= function(self) self:rotationz(0) end}},
			},
			holds= {
				TapNoteSubType_Hold= {
					{
						state_map= inactive_state_map,
						textures= {hold_tex},
						flip= TexCoordFlipMode_None,
						length_data= hold_length,
					},
					{
						state_map= active_state_map,
						textures= {hold_tex},
						flip= TexCoordFlipMode_None,
						length_data= hold_length,
					},
				},
				TapNoteSubType_Roll= {
					{
						state_map= inactive_state_map,
						textures= {roll_tex},
						flip= TexCoordFlipMode_None,
						length_data= hold_length,
					},
					{
						state_map= active_state_map,
						textures= {roll_tex},
						flip= TexCoordFlipMode_None,
						length_data= hold_length,
					},
				},
			},
			reverse_holds= {
				TapNoteSubType_Hold= {
					{
						state_map= inactive_state_map,
						textures= {hold_tex},
						flip= TexCoordFlipMode_None,
						length_data= hold_length,
					},
					{
						state_map= active_state_map,
						textures= {hold_tex},
						flip= TexCoordFlipMode_None,
						length_data= hold_length,
					},
				},
				TapNoteSubType_Roll= {
					{
						state_map= inactive_state_map,
						textures= {roll_tex},
						flip= TexCoordFlipMode_None,
						length_data= hold_length,
					},
					{
						state_map= active_state_map,
						textures= {roll_tex},
						flip= TexCoordFlipMode_None,
						length_data= hold_length,
					},
				},
			},
		}
	end
	return {
		columns= columns,
		vivid_operation= true, -- output 200%
	}
end
