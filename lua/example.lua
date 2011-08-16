
if not PichiLua then
    PichiLua = {}
end

if not PichiCommands then
    PichiCommands = {}
end

-- register module
function PichiLua.init ( pichi )
		RegisterModule(pichi, "Example", "Simple Example", "1.0", "DEg", "deg@uruchie.org")
end

function PichiCommands.plugins ( pichi, text )
		SendAnswer( pichi, text )
end
