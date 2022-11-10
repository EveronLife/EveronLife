modded class EL_ProcessAction
{
	//------------------------------------------------------------------------------------------------
	//! TODO: Remove once https://feedback.bistudio.com/T168813 is fixed
	override void ConsumeInput(IEntity input, IEntity pUserEntity, InventoryStorageManagerComponent inventoryManager)
	{
		if (input.FindComponent(EL_HandCarryComponent)) 
		{
			EL_HandInventoryStorageComponent handStorage = EL_ComponentFinder<EL_HandInventoryStorageComponent>.Find(pUserEntity);
			handStorage.Delete(input);
			return;
		}
		
		super.ConsumeInput(input, pUserEntity, inventoryManager);
	}
}
