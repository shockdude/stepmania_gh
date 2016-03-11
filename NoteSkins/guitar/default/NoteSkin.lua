local ret = ... or {};
-- This is mostly a redo of KB7's default

ret.RedirTable =
{
	Fret1 = "Fret1",
	Fret2 = "Fret2",
	Fret3 = "Fret3",
	Fret4 = "Fret4",
	Fret5 = "Fret5",
	-- should work? doesn't though.
	StrumDown = GAMESTATE:IsSideJoined('PlayerNumber_P2') and "Strum" or "Strum",
};

local OldRedir = ret.Redir;
ret.Redir = function(sButton, sElement)
	sButton, sElement = OldRedir(sButton, sElement);

	-- Instead of separate hold heads, use the tap gem graphics.
	if sElement == "Hold Head Inactive" or
	   sElement == "Hold Head Active" or
	   sElement == "Roll Head Inactive" or
	   sElement == "Roll Head Active" or
	   sElement == "Hold Gemhead Inactive" or
	   sElement == "Hold Gemhead Active" or
	   sElement == "Roll Gemhead Inactive" or
	   sElement == "Roll Gemhead Active"
	then
		sElement = "Tap Gem";
	elseif sElement == "Hold HOPOhead Inactive" or
	   sElement == "Hold HOPOhead Active" or
	   sElement == "Roll HOPOhead Inactive" or
	   sElement == "Roll HOPOhead Active"
	then
		sElement = "Tap HOPO";
	end

	sButton = ret.RedirTable[sButton];

	return sButton, sElement;
end

-- To have separate graphics for each hold part:
--[[
local OldRedir = ret.Redir;
ret.Redir = function(sButton, sElement)
	-- Redirect non-hold, non-roll parts.
	if string.find(sElement, "hold") then
		return sButton, sElement;
	end
	return OldRedir(sButton, sElement);
end]]

local OldFunc = ret.Load;
function ret.Load()
	local t = OldFunc();

	-- The main "Explosion" part just loads other actors; don't rotate
	-- it.  The "Hold Explosion" part should not be rotated.
	if Var "Element" == "Explosion" or
	   Var "Element" == "Roll Explosion" or
	   Var "Element" == "Hold Explosion" then
		t.BaseRotationZ = nil;
	end
	return t;
end

ret.PartsToRotate =
{
	["Go Receptor"] = true,
	["Ready Receptor"] = true,
	["Tap Explosion Bright"] = true,
	["Tap Explosion Dim"] = true,
	["Tap Note"] = true,
	["Hold Head Active"] = true,
	["Hold Head Inactive"] = true,
	["Roll Head Active"] = true,
	["Roll Head Inactive"] = true,
};
ret.Rotate =
{
	Fret1 = 0,
	Fret2 = 0,
	Fret3 = 0,
	Fret4 = 0,
	Fret5 = 0,
	StrumDown = 0,
};

--
-- If a derived skin wants to have separate UpLeft graphics,
-- use this:
--
-- ret.RedirTable.UpLeft = "UpLeft";
-- ret.RedirTable.UpRight = "UpLeft";
-- ret.Rotate.UpLeft = 0;
-- ret.Rotate.UpRight = 90;
--
ret.Blank =
{
	["Hold Topcap Active"] = true,
	["Hold Topcap Inactive"] = true,
	["Roll Topcap Active"] = true,
	["Roll Topcap Inactive"] = true,
	["Hold Tail Active"] = true,
	["Hold Tail Inactive"] = true,
	["Roll Tail Active"] = true,
	["Roll Tail Inactive"] = true,
};

return ret;
