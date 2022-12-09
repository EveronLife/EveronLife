#Store Config

## Default store configs
`Configs/ItemShop/EL_Shops.conf`
`Configs/ItemShop/Shops/EL_ClothingStore.conf`
`Configs/ItemShop/Shops/EL_GeneralStore.conf`
`Configs/ItemShop/Shops/EL_WeaponStore.conf`

## EL_Shops.conf
The main shop config file. All store configs are referenced here.
Every shop item searches through all referenced configs to find its price entry.

## EL_XYStore.conf
Each Store type has its own config for a better overview and easier sorting/replacing of items.

## Adding a store item to the world
1. Create a new Entity using the `Prefabs/ItemShop/ShopItem.et` prefab.
2. Set the item to buy/sell in the `EL_ShopItemComponent -> Shop Item Prefab`
3. (Optional) Set another mesh in the `MeshObject` to change the items visuals (e.g. Apple Crate for apples)
	-> You can also add child Entities (e.g. Apples in the apple crate)

## Creating a new store
1. Create a new config with the `EL_ShopConfig` Class in the `Configs/ItemShop/Shops/` folder.
2. Open the main store config `Configs/ItemShop/EL_Shops.conf` and add the newly created config in the stores array (drag and drop).


## Different Prices for different shops
To achieve this, a new main shops config is needed (like `Configs/ItemShop/EL_Shops.conf`)

1. Create a new config with the `EL_ShopsConfig` Class in the `Configs/ItemShop/` folder.
2. Create a new store config (see "Creating a new store")
3. Duplicate `Prefabs/ItemShop/ShopItem.et`
4. Add the newly created main shop config to the duplicated prefab.




