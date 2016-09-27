return function(button_list, stepstype, skin_parameters)
	local ret= {}
	local rots= {}
	-- Adding dance buttons temporarily to test skin
	local tap_redir= {
		Fret1= "Fret1", Fret2= "Fret2", Fret3= "Fret3", Fret4= "Fret4", 
		-- Strum receptor is hidden, so it doesn't matter
		Fret5= "Fret5", Fret6= "Fret1", StrumDown= "Strum", Left= "Fret1",
		Down= "Fret2", Up= "Fret3", Right= "Fret4"
	}
	for i, button in ipairs(button_list) do
		ret[i]= Def.ActorFrame{
			Def.Sprite{
			Texture= tap_redir[button].." Ready Receptor", InitCommand= function(self)
				self:rotationz(0):effectclock("beat")
					:draworder(notefield_draw_order.receptor)
				if tap_redir[button] == "Strum" then
					self:visible(false)
				end
			end,
			WidthSetCommand= function(self, param)
				param.column:set_layer_fade_type(self, "FieldLayerFadeType_Receptor")
			end,
			},
			Def.Sprite{
			Texture= tap_redir[button].." Go Receptor", InitCommand= function(self)
				self:rotationz(0):effectclock("beat")
					:draworder(notefield_draw_order.receptor):visible(false)
			end,
			WidthSetCommand= function(self, param)
				param.column:set_layer_fade_type(self, "FieldLayerFadeType_Receptor")
			end,
			BeatUpdateCommand= function(self, param)
				-- Strum should never be visible
				if param.pressed and tap_redir[button] ~= "Strum" then
					self:visible(true)
				end
				-- Yo Kyzentun, you forgot to mention this nifty param
				if param.lifted then
					self:visible(false)
				end
			end,
			}
			}
	end
	return ret
end
