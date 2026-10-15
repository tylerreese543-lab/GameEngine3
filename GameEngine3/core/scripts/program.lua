local speed = 2.4
local gravity = 0.5
local moveVector = Vector3.new();
local jumpable = false;


local camera = scene.CurrentCamera;
MouseData:SetMouseInputMode(true);


local g = GamepadData:Get(0);
local useController = g ~= nil;

function Tick(deltaTime)
	

	if KeyboardData:IsKeyDown(Enum.KeyCode.E) then
		MouseData:SetMouseInputMode(false);
	elseif KeyboardData:IsKeyDown(Enum.KeyCode.Q) then
		MouseData:SetMouseInputMode(true);
	end
	
	if (KeyboardData:IsKeyDown(Enum.KeyCode.LeftControl)) then
		speed = 1
	else
		speed = 10
	end

	moveVector = Vector3.new(
		(KeyboardData:IsKeyDown(Enum.KeyCode.D) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.A) and 1 or 0), 
		0, 
		(KeyboardData:IsKeyDown(Enum.KeyCode.W) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.S) and 1 or 0)
	) * speed * deltaTime * camera.LookMatrix;
	moveVector.Y = moveVector.Y + ((KeyboardData:IsKeyDown(Enum.KeyCode.Spacebar) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.LeftShift) and 1 or 0)) * speed * deltaTime
	camera.Position = camera.Position + moveVector

	if (MouseData:IsRawMouseInput()) then

		camera.Rotation = camera.Rotation + Vector3.new(MouseData.Delta.Y / 300, MouseData.Delta.X / 300, 0);
		if camera.Rotation.X > math.pi * 0.5 then
			camera.Rotation.X = math.pi * 0.5
		elseif camera.Rotation.X < -math.pi * 0.5 then
			camera.Rotation.X = -math.pi * 0.5
		end
	end

end