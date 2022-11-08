[BaseContainerProps()]
class EL_ResourceDestructionTool
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Tool to destroy the resource", "et")]
	ResourceName m_rTool;
	
	[Attribute("20", UIWidgets.Auto, "Amount of damage to the resource on hit (x/100)", "1 100 1")]
	int m_fHitDamage;
}

class EL_DestructibleResourceComponentClass : SCR_DestructionMultiPhaseComponentClass
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Particle to spawn on hit", "et", category: "Damage FX")]
	ResourceName m_rHitEffect;
	
	[Attribute("", UIWidgets.Auto, "Usable tools to destroy the resource", category: "Tools")]
	ref array<ref EL_ResourceDestructionTool> m_aTools;
}

class EL_DestructibleResourceComponent : SCR_DestructionMultiPhaseComponent
{
}
