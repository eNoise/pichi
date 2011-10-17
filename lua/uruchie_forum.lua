
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

pichi:setListener( "command_help", "uforum",
	function( help )
		local new_help = {
			forum = "!forum - это подсказка\n",
			forum_whoami = "!forum whoami - к какому аккаунту я привязан\n",
			forum_login = "!forum login user password - связать себя с аккаунтом форума\n",
			forum_getpost = "!forum getpost [тема] - получить посты\n",
			forum_setkarma = "!forum setkarma user +1|-1 - поднять, опустить карму\n",
			forum_getkarma = "!forum getkarma user - получить карму пользователя\n",
			forum_setrating = "!forum setrating post +1|-1 - поднять, опустить рейтинг\n"
		}
		
		for k,v in pairs(help) do
			new_help[k] = v
		end
		return new_help
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
!forum getkarma user - получить карму пользователя
!forum setrating post +1|-1 - поднять, опустить рейтинг
]] )
	end
end

function PichiCommandsAsync.forum ( arg, pichiobject, env )
	local command, args
	command, args = string.match(arg, "(%a+) (.+)")
	if not command then
		command = string.match(arg, "(%a+)")
	end
	if command and UruchieForumCommandsAsync[command] then
		UruchieForumCommandsAsync[command]( args, pichiobject, env )
		freeMessageEnv(env)
	end
end

function UruchieForumCommands.whoami ( args, pichiobject )
	local user = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_user" )
	local isFind = false
	for k,v in pairs(user) do
		isFind = true
		SendAnswer( pichiobject, "Ты " .. v)
	end
	if not isFind then
		SendAnswer( pichiobject, "Вы не связаны с аккаунтом форума")
	end
end

function UruchieForumCommands.login ( args, pichiobject )
	local user, password
	if type(args) == "string" then
		user, password = string.match(args, "([^%s]+) ([^%s]+)")
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
  return string.gsub( text, "\[/?[A-Za-z_]+\]" , "" )
end

function UruchieForumCommandsAsync.getpost( args, pichiobject, env )
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
	 SendAnswer(pichiobject, answer, env)
end

function UruchieForumCommandsAsync.setkarma( args, pichiobject, env )
	 local target, sub
	 if type(args) == "string" then
		target, sub = string.match(args, "([^%s]+) ([1+-]+)")
	 end
	 if sub ~= "+1" and sub ~= "-1" then
		SendAnswer(pichiobject, "проверьте введенные параметры", env)
		return
	 end
	 local user = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_user" )
	 local password = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_password" )
	 user = user.uforum_user
	 password = password.uforum_password
	 if not user or not password then
		SendAnswer(pichiobject, "установите свои данные", env)
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
		SendAnswer(pichiobject, "неверный ник", env)
	 end
	 SetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_current_userid", target ) -- Выставляем текущего пользователя
	 local karma = ReadUrl("http://uruchie.org/api.php", {
			module = "forum",
			action = (sub == "+1" and "pluskarma") or "minuskarma",
			username = user,
			password = md5sum(password),
			targetuserid = target
		})
	karma = JsonDecode(Utf8Decode(karma))
	if karma.error.error == "true" then
		SendAnswer(pichiobject, "произошла ошибка: " .. karma.error.description, env)
	else
		SendAnswer(pichiobject, "Карма успешно изменена", env)
	end
end

function UruchieForumCommandsAsync.getkarma( args, pichiobject, env )
	 local userid
	 if type(args) == "string" then
		userid = string.match(args, "(%d+)")
	 end
	 if not userid and type(args) == "string" then
		-- Пытаемся вытащить из последнего
		userid = string.match(args, "([^%s]+)")
		if userid then
			local gertarget = ReadUrl("http://uruchie.org/api.php", {
				module = "forum",
				action = "username2userid",
				username = userid
			})
			target = JsonDecode(gertarget)
			userid = target.userid
		end
	 end
	 if not userid then
		-- Пытаемся вытащить из последнего
		local user = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_current_userid" )
		for k,v in pairs(user) do
			userid = v
		end
	 end
	 if not userid then
		SendAnswer(pichiobject, "проверьте введенных корректность данных", env)
		return
	 end
	 SetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_current_userid", userid ) -- Выставляем текущего пользователя
	 local user = ReadUrl("http://uruchie.org/api.php", {
			module = "forum",
			action = "getuserinfo",
			userid = userid
		})
	user = JsonDecode(Utf8Decode(user))
	if user.error.error == "true" then
		SendAnswer(pichiobject, "произошла ошибка: " .. user.error.description, env)
	else
		SendAnswer(pichiobject, "Карма пользователя " .. user.user.username .. ": " .. user.user.karma .. ".", env)
	end
end

function UruchieForumCommandsAsync.setrating( args, pichiobject, env )
	 local target, sub
	 if type(args) == "string" then
		target, sub = string.match(args, "(%d+) ([1+-]+)")
	 end
	 if sub ~= "+1" and sub ~= "-1" then
		SendAnswer(pichiobject, "проверьте введенные параметры", env)
		return
	 end
	 target = tonumber(target)
	 local user = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_user" )
	 local password = GetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_password" )
	 user = user.uforum_user
	 password = password.uforum_password
	 if not user or not password then
		SendAnswer(pichiobject, "установите свои данные", env)
		return
	 end
	 SetJIDinfo( pichiobject, GetLastJID( pichiobject ), "uforum_current_postid", target ) -- Выставляем текущий пост
	 local rating = ReadUrl("http://uruchie.org/api.php", {
			module = "forum",
			action = (sub == "+1" and "plusrating") or "minusrating",
			username = user,
			password = md5sum(password),
			postid = target
		})
	rating = JsonDecode(Utf8Decode(rating))
	if rating.error.error == "true" then
		SendAnswer(pichiobject, "произошла ошибка: " .. rating.error.description, env)
	else
		SendAnswer(pichiobject, "Рейтинг изменен", env)
	end
end

function UruchieForumCommands.test( args, pichiobject )
end
