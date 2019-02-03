count=1


F10::
	Global count
	FileEncoding, UTF-8
	MouseClick, left
	Sleep 200
	Send, ^c
	FileAppend,%clipboard%,%A_ScriptDir%\Data\%count%.txt
	count++
	return