
if not PichiCommands then
    PichiCommands = {}
end

-- register module
pichi:setListener( "init", "example",
	function( pichiobject )
		RegisterModule(pichiobject, "Example", "Simple Example", "1.0", "DEg", "deg@uruchie.org")
	end
)

function PichiCommands.example ( arg, pichiobject )
		SendAnswer( pichiobject, "Simple Example" )
end
