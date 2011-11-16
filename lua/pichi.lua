-- Pichi Bot
-- help function by DEg

if not pichi then
    pichi = {}
    pichi._listeners = {}
end

pichi.setListener = function( this, ltype, id, func )
    if not pichi._listeners[ltype] then
	pichi._listeners[ltype] = {}
    end
    pichi._listeners[ltype][id] = func
end

pichi.getListeners = function( this, ltype )
    if pichi._listeners[ltype] then
	return pichi._listeners[ltype]
    else
	return {}
    end
end

-- Lets start
if not PichiLua then
    PichiLua = {}
end

function PichiLua.init ( pichiobject )
	for k,f in pairs(pichi:getListeners("init")) do
		f( pichiobject )
	end
end

-- PichiCore
if not PichiCore then
    PichiCore = {}
end

function PichiCore.recive_message_start ( pichiobject )
	for k,f in pairs(pichi:getListeners("recive_message_start")) do
		f( pichiobject )
	end
end

function PichiCore.recive_message_end ( pichiobject )
	for k,f in pairs(pichi:getListeners("recive_message_end")) do
		f( pichiobject )
	end
end
-- end PichiCore

-- Help Command mod
if not PichiCommand then
    PichiCommand = {}
end

function PichiCommand.help ( )
	local help = {}
	for k,f in pairs(pichi:getListeners("command_help")) do
		help = f( help )
	end
	return help
end
-- end help command