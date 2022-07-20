modded class SCR_PickUpItemAction
{
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		EL_InventoryQuantityComponent quantityComponent = EL_InventoryQuantityComponent.Cast(pOwnerEntity.FindComponent(EL_InventoryQuantityComponent));
		if (!quantityComponent)
		{
			super.PerformActionInternal(manager, pOwnerEntity, pUserEntity);
			return;
		}

		manager.InsertItem(pOwnerEntity);

		// Replace the owner to fix VM null exception
		pOwnerEntity = pUserEntity;
		if (pOwnerEntity)
		{
			return;
		}

		//! COPY-PASTE of 'super'

		// Play sound
		RplComponent rplComp = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		if (rplComp)
			manager.PlayItemSound(rplComp.Id(), "SOUND_PICK_UP");
		else
		{
			SimpleSoundComponent simpleSoundComp = SimpleSoundComponent.Cast(pOwnerEntity.FindComponent(SimpleSoundComponent));
			if (simpleSoundComp)
			{
				vector mat[4];
				pOwnerEntity.GetWorldTransform(mat);
				
				simpleSoundComp.SetTransformation(mat);
				simpleSoundComp.PlayStr("SOUND_PICK_UP");
			}
		}
	}
};
