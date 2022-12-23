# New garage
A garage needs an Entity with the <EL_GarageManagerComponent> to open the garage.
This Entity needs to be a child of another Entity e.g. a building. 
The Building needs to have one or more spawn points (Prefab: <EL_VehicleSpawnPoint>)


## Vehicle Spawn Point Prefab
The trigger box size can be changed but the Entity itself needs to be **outside** the box to not falsely trigger it. (e.g Box Mins Y:-0.5)