if mod then return end  -- avoid loading twice the same module
mod = {}  -- create a table to represent the module

function mod.tick(a)
	print('mod.lua (lua)')
end

--function open (mod)
--  local n, f = next(mod, nil)
--  while n do
--    setglobal(n, f)
--    n, f = next(mod, n)
--  end
--end


