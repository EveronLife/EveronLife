//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/Destruction", description: "Multi-Phase destruction component, for objects that go through several damage phases")]
class EL_DestructionMultiPhaseComponentClass: SCR_DestructionMultiPhaseComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Multi-Phase destruction component, for objects that go through several destruction states
class EL_DestructionMultiPhaseComponent : SCR_DestructionMultiPhaseComponent
{
	[Attribute("0", uiwidget: UIWidgets.ComboBox, desc: "Logging area size", "", ParamEnumArray.FromEnum(EWeaponType))]
	EWeaponType effectiveWeaponType;
	[Attribute("0.1", UIWidgets.EditBox, desc: "Damage multiplier other weapons do")]
	float otherWeaponDmgMultiplier;
	
	//------------------------------------------------------------------------------------------------
	//! Damage
	override void OnDamage(EDamageType type, float damage, HitZone pHitZone, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		Print(instigator);
		BaseWeaponManagerComponent weaponManager = BaseWeaponManagerComponent.Cast(instigator.FindComponent(BaseWeaponManagerComponent));
		EWeaponType currentWeaponType = weaponManager.GetCurrentWeapon().GetWeaponType();
		
		//Reduce any other weapon damage
		if (effectiveWeaponType != EWeaponType.WT_NONE && currentWeaponType != effectiveWeaponType)
			damage *= otherWeaponDmgMultiplier;
		
		super.OnDamage(type, damage, pHitZone, instigator, hitTransform, speed, colliderID, nodeID);
	}
}
