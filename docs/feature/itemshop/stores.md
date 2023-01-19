# Store Configs
Item prices are set in `Configs\ItemShop\Shops\XYStoreConfig.conf`

## XYStoreConfig.conf
Each store type (General,Weapon,Clothing) can have its own config for a better overview and easier sorting/replacing of items.

## Creating a new store
1. Create a new config with the `EL_ItemShopConfig` class in the `Configs/ItemShop` folder.
2. Open the newly created config and add all items you want to sell in that store. \
	-> Multi Buy/Sell enables 10x and buy/sell all actions

## Adding a store item to the world
1. If not already done, create a new **inherited** ShopItem prefab from `Prefabs\ItemShop\ShopItem_Base.et` \
	-> e.g `Prefabs/ItemShop/ShopItem_GeneralStore.et`. \
	-> This prefab will be used for every item in a store type e.g General Store.
2. Move the newly created prefab in the world where you want to sell that item.
3. Set the store config you want to use in the `EL_ShopItemComponent` on the newly created entity and to make things easier apply it to the prefab. \
	-> Drag and drop it from the Resource Browser to where it says `set class`. \
	-> Now every time you do 2. the correct store config is already on the entity.
4. Now also set the prefab you want to sell in the `EL_ShopItemComponent` under `Shop Item Prefab`. \
	-> The entity visuals automaticly change to look like this prefab.
4. (Optional) Set another mesh in the `MeshObject` to change the items visuals (e.g. Apple Crate for apples)\
	-> You can also add child Entities (e.g. Apples in the apple crate)
5. Goto 2. but skip 3. to add more shop items
