local player = scene:FindFirstChild("Player")
local ground = scene:FindFirstChild("Ground")
local goalmesh = scene:FindFirstChild("GoalMesh");
local camera = scene.CurrentCamera

local jumpable = false;

local player_velocity = Vector3.new(0, 0, 0);

local camera_angle = 0;
function updateGoalPosition()
if not goalmesh.Collider then return end
goalmesh.Collider.Position = Vector3.new(math.random(-30, 10), -5, math.random(-10, 10))
goalmesh.Position = goalmesh.Collider.Center;
end

updateGoalPosition();
function isTouchingAnyGround()
if not player.Collider then return end;
	for i, obj in pairs(ground:GetChildren()) do
		if obj.ClassName == "AABBCollision3D" then
			if obj:IsTouching(player.Collider) then
				return true;
			end
		end
	end

	return false;
end
function Tick(deltaTime)
camera = scene.CurrentCamera

if (camera and player.Collider) then
	camera.Position = player.Collider.Center + Vector3.new(0, 3, -3) * Matrix3.RotateY(camera_angle)
	camera.Rotation = Vector3.new(0.5, -camera_angle, 0);

	player.Rotation = Vector3.new(0, -camera_angle, 0);
	player.Position = player.Collider.Center;
end
player_velocity.Y = player_velocity.Y + (-30 * deltaTime);

if (KeyboardData:IsKeyDown(Enum.KeyCode.Left)) then
	camera_angle = camera_angle + (4 * deltaTime);
end
if (KeyboardData:IsKeyDown(Enum.KeyCode.Right)) then
	camera_angle = camera_angle - (4 * deltaTime);
end
if jumpable and KeyboardData:IsKeyDown(Enum.KeyCode.Spacebar) then
	player_velocity.Y = 15;
	jumpable = false;
end

local moveVector = Vector3.new(
	(KeyboardData:IsKeyDown(Enum.KeyCode.D) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.A) and 1 or 0),
	0,
	(KeyboardData:IsKeyDown(Enum.KeyCode.W) and 1 or 0) - (KeyboardData:IsKeyDown(Enum.KeyCode.S) and 1 or 0)
);

player_velocity = player_velocity + (moveVector * (150 * deltaTime) * Matrix3.RotateY(camera_angle));
player_velocity = player_velocity * Vector3.new(0.9, 1, 0.9);
local step = 0.25;
local pv_dt = player_velocity * deltaTime;

jumpable = false;

if player.Collider then
for i = 1, 4 do
	--X
	player.Collider:SetX(player.Collider.Position.X + (pv_dt.X * step));
	if isTouchingAnyGround() then
		player.Collider:SetX(player.Collider.Position.X - (pv_dt.X * step));
		player_velocity.X = 0;
	end
	--Y
	player.Collider:SetY(player.Collider.Position.Y + (pv_dt.Y * step));
	if isTouchingAnyGround() then
		player.Collider:SetY(player.Collider.Position.Y - (pv_dt.Y * step))
		if (player_velocity.Y < 0) then
			jumpable = true;
		end
		player_velocity.Y = 0;
		
	end
	--Z
	player.Collider:SetZ(player.Collider.Position.Z + (pv_dt.Z * step));
	if isTouchingAnyGround() then
		player.Collider:SetZ(player.Collider.Position.Z - (pv_dt.Z * step));
		player_velocity.Z = 0;
	end
end
end

if goalmesh.Collider and player.Collider then
if (player.Collider:IsTouching(goalmesh.Collider)) then
	updateGoalPosition();
end
end
end