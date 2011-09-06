
if not PichiCommands then
    PichiCommands = {}
end

UruchieForumCommands = {}

-- register module
pichi:setListener( "init", "uforum",
	function( pichiobject )
		RegisterModule(pichiobject, "UruchieForum", "Uruchie Forum Module", "0.1", "DEg", "deg@uruchie.org")
	end
)

function PichiCommands.forum ( arg, pichiobject )
	local command, args
	command, args = string.match(arg, "(%a+) (.+)")
	if not command then
		command = string.match(arg, "(%a+)")
	end
	if command then
		if UruchieForumCommands[command] then
			UruchieForumCommands[command]( args, pichiobject )
		else
			SendAnswer( pichiobject, "Такой команды нет.")
		end
	else
		SendAnswer( pichiobject, [[

!forum - это подсказка
!forum whoami - к какому акаунту я привязан
!forum login user password - связать себя с аккаунтом форума
]] )
	end
end

function UruchieForumCommands.whoami ( args, pichiobject )
	SendAnswer( pichiobject, "Ты никто!!!")
end
