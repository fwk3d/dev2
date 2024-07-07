print('Hello from Lua language')

local function add(a, b)
   return a + b
end

local s = add(1,2)
print(s)

C.app_create(0.75, 0)
while C.app_swap() ~= 0 do
   if C.ui_panel('Lua demo',C.UI_OPEN) ~= 0 then
      if C.ui_button('Click me') ~= 0 then 
         C.ui_notify('Hello from Lua!', 'hi!')
      end
      C.ui_panel_end()
   end
end
