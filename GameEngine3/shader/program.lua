function Tick(deltaTime)
	this.Parent.Rotation = Vector3.new(-MouseData.Position.Y / 300, -MouseData.Position.X / 300, 0)
end