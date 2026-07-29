local speed = 2.4
local gravity = 0.5
local moveVector = Vector3.new();
local jumpable = false;

local model1 = scene:FindFirstChild("Model1")
model1.Position = Vector3.new(2.0, 2.0, 0.0)
local camera = scene.CurrentCamera;
MouseData:SetMouseInputMode(true);
function Tick(deltaTime)
	
	if KeyboardData:IsKeyDown(Enum.KeyCode.E) then
		MouseData:SetMouseInputMode(false);
	elseif KeyboardData:IsKeyDown(Enum.KeyCode.Q) then
		MouseData:SetMouseInputMode(true);
	end
	
	moveVector = moveVector + Vector3.new(
		(KeyboardData:IsKeyDown(Enum.KeyCode.D) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.A) and 1 or 0), 
		0, 
		(KeyboardData:IsKeyDown(Enum.KeyCode.W) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.S) and 1 or 0)
	) * speed * deltaTime * camera.MoveDirection2D;
	
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
	
	if (jumpable == true and KeyboardData:IsKeyDown(Enum.KeyCode.Spacebar)) then
		moveVector.Y = 0.1
	end
	if MouseData:IsRawMouseInput() then
		camera.Rotation = Vector3.new(MouseData.Position.Y / 300, MouseData.Position.X / 300, 0);
	end

	if KeyboardData:IsKeyDown(Enum.KeyCode.L) then
		model1.Rotation.X = model1.Rotation.X + deltaTime;
	end
end