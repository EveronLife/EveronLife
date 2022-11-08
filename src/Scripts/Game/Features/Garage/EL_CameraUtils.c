class EL_CameraUtils
{	
	//------------------------------------------------------------------------------------------------
	static void DestroyCamera(SCR_ManualCamera camera)
	{
		if (!camera)
			return;
		
		IEntity cameraParent = camera.GetParent();
		if (cameraParent)
			cameraParent.RemoveChild(camera);

		delete camera;
		
	}
	
	//------------------------------------------------------------------------------------------------
	static SCR_ManualCamera CreateCamera(ResourceName cameraPrefab, IEntity parent, vector position, vector angles)
	{
		BaseWorld baseWorld;
		// Spawn camera
		if (parent)
			baseWorld = parent.GetWorld();
			
		SCR_ManualCamera manualCamera = SCR_ManualCamera.Cast(GetGame().SpawnEntityPrefabLocal(Resource.Load(cameraPrefab), baseWorld));
		
		vector mat[4];
		Math3D.MatrixIdentity3(mat);
		mat[3] = position;
		
		manualCamera.SetTransform(mat);
		manualCamera.SetAngles(angles);
		
		if (parent)
			parent.AddChild(manualCamera, -1, EAddChildFlags.AUTO_TRANSFORM);
		
		return manualCamera;
	}	
	
	//------------------------------------------------------------------------------------------------
	static SCR_ManualCamera CreateAndSetCamera(ResourceName cameraPrefab, IEntity parent, vector position, vector angles)
	{
		SCR_ManualCamera manualCam = CreateCamera(cameraPrefab, parent, position, angles);
		
		CameraManager camManager = GetGame().GetCameraManager();
		camManager.SetCamera(manualCam);
		
		return manualCam;
	}
}