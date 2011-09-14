
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
		elseif not UruchieForumCommandsAsync[command] then
			SendAnswer( pichiobject, "Такой команды нет.")
		end
	else
		SendAnswer( pichiobject, [[

!forum - это подсказка
!forum whoami - к какому аккаунту я привязан
!forum login user password - связать себя с аккаунтом форума
!forum getpost [тема] - получить посты
!forum setkarma user +1|-1 - поднять, опустить карму
!forum setrating post +1|-1 - поднять, опустить рейтинг
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
		user, password = string.match(args, "([%a0-9А-Яа-я]+) ([%a0-9А-Яа-я]+)")
	end
	if not user or not password then
		SendAnswer( pichiobject, "введите полную информацию и попробуйте опять")
		return
	end
	SetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_user", user )
	SetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_password", password )
	SendAnswer( pichiobject, "установлено")
end

local function escapePagetext( text )
  return string.gsub( text, "\[/?[A-Za-z]+\]" , "" )
end

function UruchieForumCommandsAsync.getpost( args, pichiobject )
	  local thread = string.match(args or "", "%d+")
	  local reciveposts = ReadUrl("http://uruchie.org/api.php", {
			module = "forum",
			action = "lastmessages",
			limit = 5,
			structfilter = "posts,user,usergroup,thread",
			threadid = thread or 0
		})
	 local getposts = JsonDecode(Utf8Decode(reciveposts))
	 local answer = "\n"
	 for k,post in pairs(getposts.posts) do
		answer = answer 
			  .. "Тема: " .. post.thread.title .. "\n"
			  .. "Автор поста: " .. post.username .. "\n"
			  .. "Время: " .. os.date("%d.%m.%Y %H:%M", post.dateline) .. "\n"
			  .. "Заголовок: " .. (post.title or "<нет>") .. "\n"
			  .. escapePagetext(post.pagetext) 
			  .. "\n\n"
	 end
	 SendAnswer(pichiobject, answer)
end

function UruchieForumCommandsAsync.setkarma( args, pichiobject )
	 local target, sub
	 if type(args) == "string" then
		target, sub = string.match(args, "(%a+) ([1+-]+)")
	 end
	 if sub ~= "+1" and sub ~= "-1" then
		SendAnswer(pichiobject, "проверьте введенные параметры")
		return
	 end
	 local user = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_user" )
	 local password = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_password" )
	 user = user.uforum_user
	 password = password.uforum_password
	 if not user or not password then
		SendAnswer(pichiobject, "установите свои данные")
		return
	 end
	 local gertarget = ReadUrl("http://uruchie.org/api.php", {
			module = "forum",
			action = "username2userid",
			username = target
		})
	 target = JsonDecode(gertarget)
	 target = target.userid
	 if not target or target == "false" or target == "0" then
		SendAnswer(pichiobject, "неверный ник")
	 end
	 local karma = ReadUrl("http://uruchie.org/api.php", {
			module = "forum",
			action = (sub == "+1" and "pluskarma") or "minuskarma",
			username = user,
			password = md5sum(password),
			targetuserid = target
		})
	karma = JsonDecode(Utf8Decode(karma))
	if karma.error.error == "true" then
		SendAnswer(pichiobject, "произошла ошибка: " .. karma.error.description)
	else
		SendAnswer(pichiobject, "Карма успешно изменена")
	end
end

function UruchieForumCommandsAsync.setrating( args, pichiobject )
	 local target, sub
	 if type(args) == "string" then
		target, sub = string.match(args, "(%d+) ([1+-]+)")
	 end
	 if sub ~= "+1" and sub ~= "-1" then
		SendAnswer(pichiobject, "проверьте введенные параметры")
		return
	 end
	 target = tonumber(target)
	 local user = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_user" )
	 local password = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_password" )
	 user = user.uforum_user
	 password = password.uforum_password
	 if not user or not password then
		SendAnswer(pichiobject, "установите свои данные")
		return
	 end
	 local rating = ReadUrl("http://uruchie.org/api.php", {
			module = "forum",
			action = (sub == "+1" and "plusrating") or "minusrating",
			username = user,
			password = md5sum(password),
			postid = target
		})
	rating = JsonDecode(Utf8Decode(rating))
	if rating.error.error == "true" then
		SendAnswer(pichiobject, "произошла ошибка: " .. rating.error.description)
	else
		SendAnswer(pichiobject, "Рейтинг изменен")
	end
end

function UruchieForumCommands.test( args, pichiobject )
end
