local t = Def.ActorFrame {
	LoadActor( "Fret1 Ready Receptor" )..{
		InitCommand=NOTESKIN:GetMetricA('ReceptorArrow', 'InitCommand');
		NoneCommand=NOTESKIN:GetMetricA('ReceptorArrow', 'NoneCommand');
	};
	LoadActor( "Fret1 Go Receptor" )..{
		InitCommand=NOTESKIN:GetMetricA('ReceptorOverlay', 'InitCommand');
		PressCommand=NOTESKIN:GetMetricA('ReceptorOverlay', 'PressCommand');
		LiftCommand=NOTESKIN:GetMetricA('ReceptorOverlay', 'LiftCommand');
	};
};
return t;
