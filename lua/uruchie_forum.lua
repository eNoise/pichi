
if not PichiCommands then
    PichiCommands = {}
end

if not PichiCommandsAsync then
    PichiCommandsAsync = {}
end

--local json = require("dkjson")

local UruchieForumCommands = {}
local UruchieForumCommandsAsync = {}

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
!forum whoami - к какому аккаунту я привязан
!forum login user password - связать себя с аккаунтом форума
]] )
	end
end

function PichiCommandsAsync.forum ( arg, pichiobject )
	local command, args
	command, args = string.match(arg, "(%a+) (.+)")
	if not command then
		command = string.match(arg, "(%a+)")
	end
	if command and UruchieForumCommandsAsync[command] then
		UruchieForumCommandsAsync[command]( args, pichiobject )
	end
end

function UruchieForumCommands.whoami ( args, pichiobject )
	local user = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_user" )
	for k,v in pairs(user) do
		SendAnswer( pichiobject, "Ты " .. v)
	end
end

function UruchieForumCommands.login ( args, pichiobject )
	local user, password
	if type(args) == "string" then
		user, password = string.match(args, "(%a+) (%a+)")
	end
	if not user or not password then
		SendAnswer( pichiobject, "введите полную информацию и попробуйте опять")
		return
	end
	SetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_user", user )
	SetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_password", password )
	SendAnswer( pichiobject, "установлено")
end

function UruchieForumCommands.test( args, pichiobject )
	io.write(ReadUrl( "http://uruchie.org", {arg1 = "aaa", arg2 = "bbb"} ))
end
