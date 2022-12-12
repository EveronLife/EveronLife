# Inventory item handling
The framework is using physical items that often have a virtual quantity attached. There are a few utility functions available that remove the majority of boilerplate attached to inventory operations in Arma Reforger. 
> **Note**  
> All of the following code must be executed **on the server**.

## Adding items
```cs
class EL_AddApplesAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ResourceName itemPrefab = "{C9D661E5B0714711}Prefabs/Items/Food/Apple.et";
		int addAmount = 10;
		int actuallyAdded = EL_InventoryUtils.AddAmount(pUserEntity, itemPrefab, addAmount);
		PrintFormat("(%1/%2) '%3' were given to %4.", actuallyAdded, addAmount, itemPrefab, pUserEntity);
	}
}
```
> SCRIPT: (10/10) '{C9D661E5B0714711}Prefabs/Items/Food/Apple.et' were given to SCR_ChimeraCharacter<0x000002A3CCC681B8>.

## Removing items
```cs
class EL_RemoveApplesAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ResourceName itemPrefab = "{C9D661E5B0714711}Prefabs/Items/Food/Apple.et";
		int removeAmount = 5;
		int actuallyRemoved = EL_InventoryUtils.RemoveAmount(pUserEntity, itemPrefab, removeAmount);
		PrintFormat("(%1/%2) '%3' were removed from %4.", actuallyRemoved, removeAmount, itemPrefab, pUserEntity);
	}
}
```
> SCRIPT: (5/5) '{C9D661E5B0714711}Prefabs/Items/Food/Apple.et' were removed from SCR_ChimeraCharacter<0x000002A3CCC681B8>.

## Counting items
```cs
class EL_CountApplesAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ResourceName itemPrefab = "{C9D661E5B0714711}Prefabs/Items/Food/Apple.et";
		int itemCount = EL_InventoryUtils.GetAmount(pUserEntity, itemPrefab);
		PrintFormat("Player %1 has %2 of %3.", pUserEntity, itemCount, itemPrefab);
	}
}
```
> SCRIPT: Player SCR_ChimeraCharacter<0x000002A3CCC681B8> has 5 of {C9D661E5B0714711}Prefabs/Items/Food/Apple.et.

## Special case: Money
While the above code could be used with the cash prefab manually, there are already some convenience functions to handle this common use-case. It is recommended to rely on those for future-proofing.
```cs
IEntity player = ...;
PrintFormat("Current cash: %1", EL_MoneyUtils.GetCash(player));
PrintFormat("Added cash: %1", EL_MoneyUtils.AddCash(player, 100));
PrintFormat("Removed cash: %1", EL_MoneyUtils.RemoveCash(player, 50));
```
