local speed = 2.4
local gravity = 0.5
local moveVector = Vector3.new();
local jumpable = false;

local model1 = scene:FindFirstChild("Model1")
model1.Position = Vector3.new(2.0, 2.0, 0.0)
local camera = scene.CurrentCamera;
MouseData:SetMouseInputMode(true);

local value = StringValue.new();
value.Name = "Folder Placeholder"
value.Parent = scene;
for i, v in pairs(scene:GetChildren()) do
	print(v.Name.." - "..v.ClassName);
end

for i = 1, 10, 1 do
	local m = model1:Clone();
	m.Parent = value;

	local b = FloatValue.new();
	b.Parent = m;
	b.Value = i * 36;
end
local g = GamepadData:Get(0);
local useController = g ~= nil;
function Tick(deltaTime)
	

	if KeyboardData:IsKeyDown(Enum.KeyCode.E) then
		MouseData:SetMouseInputMode(false);
	elseif KeyboardData:IsKeyDown(Enum.KeyCode.Q) then
		MouseData:SetMouseInputMode(true);
	end
	
	if (KeyboardData:IsKeyDown(Enum.KeyCode.LeftShift)) then
		speed = 0.2
	else
		speed = 2
	end
	if (useController == false) then
	moveVector = moveVector + Vector3.new(
		(KeyboardData:IsKeyDown(Enum.KeyCode.D) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.A) and 1 or 0), 
		0, 
		(KeyboardData:IsKeyDown(Enum.KeyCode.W) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.S) and 1 or 0)
	) * speed * deltaTime * camera.MoveDirection2D;
	else
		moveVector = moveVector + Vector3.new(g.LeftJoystick.X, 0, -g.LeftJoystick.Y) * speed * deltaTime * camera.MoveDirection2D;
	end
	moveVector.X = moveVector.X * 0.8;
	moveVector.Z = moveVector.Z * 0.8;
	moveVector.Y = moveVector.Y - (gravity * deltaTime);
	camera.Position = camera.Position + moveVector

	if (camera.Position.Y < 0) then
		camera.Position.Y = 0;
		moveVector.Y = 0;
		jumpable = true;
	else
		jumpable = false;
	end
	
	if (jumpable == true) then
		if (useController == false) then
			if (KeyboardData:IsKeyDown(Enum.KeyCode.Spacebar)) then
				moveVector.Y = 0.1
			end
		elseif (g:IsButtonDown(Enum.GamepadButton.ACross)) then
			moveVector.Y = 0.1
		end
	end

	if useController == false then
		if (MouseData:IsRawMouseInput()) then
		camera.Rotation = Vector3.new(MouseData.Position.Y / 300, MouseData.Position.X / 300, 0);
		end
	else
		camera.Rotation = camera.Rotation + Vector3.new(g.RightJoystick.Y, g.RightJoystick.X, 0) * math.pi * deltaTime;
	end

	for i, v in pairs(value:GetChildren()) do
		local b = v:FindFirstChild("Value");
		b.Value = b.Value + deltaTime * 2;
		v.Position = Vector3.new(10 * math.cos(b.Value), 0, 10 * math.sin(b.Value));
	end
end