# Conventions and guidelines
Following the conventions and guidelines is a requirement for any contribution to be accepted.
They ensure a consistent [directory structure](#directory_guidelines), [code style](#code_guidelines) and [asset setup](#asset_guidelines) so it is easy for users and other contributors to understand, build-upon and expand the framework.
Not every scenario is covered by this document. If you are unsure how to apply it to your particular situation, please reach out to the maintainers, they will be able to answer all your questions.

## <a name="directory_guidelines"></a> Directory structure
In general everything should follow the [Arma Reforger directory structure](https://community.bistudio.com/wiki/Arma_Reforger:Directory_Structure) to simplify the "Override in Addon" process inside the Workbench. In addition to that each distinct feature should be grouped in a similar way across all asset types. As an example this could be the structure to add a mining system:
```
src/
├─ Assets/
│  ├─ Resources/
│  │  ├─ Mining/
│  │  │  ├─ Data/
│  │  │  │  ├─ MetalOre_01_BCR.edds
│  │  │  │  ├─ MetalOre_01.emat
│  │  │  ├─ MetalOre_01.xob/
├─ Configs/
│  ├─ Feature/
│  │  ├─ Farming/
│  │  │  ├─ Mining/
│  │  │  │  ├─ OreSpawner.conf
├─ Prefabs/
│  ├─ Resources/
│  │  ├─ Mining/
│  │  │  ├─ MetalOre_01.et
├─ Scripts/
│  ├─ Feature/
│  │  ├─ Farming/
│  │  │  ├─ Mining/
│  │  │  │  ├─ EL_OreSpawner.c
│  │  │  │  ├─ EL_MineableComponent.c
│  │  │  │  ├─ UI/
│  │  │  │  │  ├─ MiningGui.c
├─ UI/
│  ├─ Layouts/
│  │  ├─ Feature/
│  │  │  ├─ Farming/
│  │  │  │  ├─ Mining/
│  │  │  │  │  ├─ MiningGui.layout
```

## <a name="code_guidelines"></a> Code Styleguide
As primary reference for how to write your code pelase see these:
- [Conventions](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Conventions)
- [Do's and Don'ts](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Do%27s_and_Don%27ts)
- [Performance](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Performance)
- [Values](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Values)

#### Flavour choices
There are some "flavour" choices left for the individual developer. Please respect other people's choices and do not reformat their code to use your flavor if you do not *need* to touch that piece of code fo the task at hand.

#### Variable names
Keep variable names as short as possible and as long as needed to convey their meaning. E.g. `int vehicleArrayCount` could be turned into `int vehiclesCount` or just `int nVehicles`.

### Type prefixes
All member variables follow the pattern `<m=Member,s=Static>_<one character type><PascalCaseName>` as per the AR guidelines. 
This project diverges slighty from their enforcement. Only primitive types *need* to be prefixed, anything that is not prefixed is assume a complex type.
In case of collections (which are also complex types) `Array` and `Set` share the same prefix **a** to avoid confusion with string primtives and their API is similar.
If the prefix adds no relevant information for complex types it can be omitted e.g. `m_pUserEntity` -> `m_UserEntity` as per AR guidelines.
AR uses some type alias to give primitives more meaning such as `ResourceName`. You can optionally annotate those e.g. `m_rPrefab`. The fallback is the root primitive they are based on.

| Type     | Boolean     | Integer     | Enum        | Float       | String      | Typename     | Vector      | Array       | Set         | Map         | Class       |
|----------|-------------|-------------|-------------|-------------|-------------|--------------|-------------|-------------|-------------|-------------|-------------|
| Optional | No          | No          | No          | No          | No          | No           | No          | **Yes**     | **Yes**     | **Yes**     | **Yes**     |
| Example  | m_**b**Name | m_**i**Name | m_**e**Name | m_**f**Name | m_**s**Name | m_**t**Name  | m_**v**Name | m_**a**Name | m_**a**Name | m_**m**Name | m_**p**Name |

### ComponentEditorProps and EntityEditorProps
The attributes use to decorate `EntityClass` and `ComponentClass` classes follow the feature pattern described in the directory structure. Examples:
- `[EntityEditorProps(category: "EveronLife/Feature/Housing")]`
- `[ComponentEditorProps(category: "EveronLife/Feature/Farming/Mining")]`

### Modded class tagged names
When adding class members or functions throug a `modded` class include the TAG e.g. `EL` in the name to avoid conflicts. E.g. `m_iELModdedCount` or `void EL_Update()`.

### Single line statements
Single line statements that can be used on `if/else`, `for`, `foreach`, `while` are tolerated, but should be kept to a minimum to make the code readable and easy to debug/change.
Good use cases in include:
- `if (condition) return;`
- `if (condition) continue;`
- `if (instance) instance.Init();`

If the code becomes too complex, the line too long or the likely hood of wanting to debug something in there is reasonably high, put it into a code block with curly braces.

### Use of the auto keyword
The keyword `auto` is only allowed if the right hand side expression is disclosing the expected compile time type.  
Bad:
```
auto myVar = instance.GetValue(); 
```
Good:
```
auto myVeryLongComponent = EL_SomeVeryLongComponentName.Cast(entity.FindComponent(EL_SomeVeryLongComponentName));
```
**NOTE:** It does currently break autocompletion inside the Workbench so it is recommended (though not mandatory) to write out the type regardless.

## <a name="asset_guidelines"></a> Assets

### What kind of assets are added to the framework core?
The core of the framework will only ship assets that are needed for the minimal gameplay prototype. More variants or visual styles that do not fit with other existing assets are instead to be included within a seperate or even third party asset pack. Triple-check that the base game or the framework does not already have an asset that could be re-purposed with new materials/textures.

### Shared directories
For some asset types there might be many of the same type. E.g. multiple kinds of food like `Apple`, `Orange`, `Plum`. They are allowed to be put into one directory, then also sharing the same `Data` folder, as they are distinct by their names. If there is a logical grouping of many sub variants that belong together they can of course be put into a separate sub folder that has its own `Data` folder then.

### Data directory
On import the game automatically creates a `data` folder for textures and materials used. Due to a [bug](https://feedback.bistudio.com/T165764) in the workbench the casing is incorrect. Always **manually** create a `Data` folder in the directory you want to import the object into.

### File types
Only use `.fbx` and `.tiff`. Those source files must be included alongside the imported asset and follow their naming.  
E.g. `ExampleObject_01.xob` + `ExampleObject_01.fbx` + `ExampleObject_01_BCR.tiff`.

### File naming
The naming scheme of assets can be derived from [Editor Entity Naming Conventions](https://community.bistudio.com/wiki/Arma_Reforger:Editor_Entity_Naming_Conventions) ignoring part 1, 2 and 7 on the first table. 
An example could be:
- `MyObject_01.xob` (Mesh object)
- `MyObject_Base.et` (Add a base class to prefabs if it is likely that there will be many variants with a shared setup)
- `MyObject_01_Large_Blue.et` (Inherits from `MyObject_Base.et` is scaled to be big and has a blue material assigned.)

### File suffixes
Textures and materials need to be suffixed according to the slot they are made for e.g. `Name_BCR.edds`, `Name_NMO.edds`. The suffix serves a technical purpose for the game to select compression algorithms.  
Check out the vanilla game assets for reference and also read the [guide on texture creation](https://community.bistudio.com/wiki/Arma_Reforger:Textures). 
The [Arma Reforger Discord](https://discord.gg/arma) contains additional (often pinned) information. For textures (and materials) check out `#enfusion_texture_markers`
- [Texture compression explaination](https://discordapp.com/channels/105462288051380224/976231628785475624/981871081046569000)
- [Devs use TIFF with 8Bits and LZW](https://discordapp.com/channels/105462288051380224/976231628785475624/977200057357074432)
- [Substance painter export remarks](https://discordapp.com/channels/105462288051380224/976231628785475624/977154368526753812)

### File sizes
One important factor in deciding if an asset will be included is its file size. Because of the size limitations for console mods the framework needs to shave off every KB it can to give the individual servers as much room for additional asset mods as possible. Metrics for triangle counts and texture sizes depend heavily on the physical size and if it is a hand held item or not.
Most assets (mesh + textures + materials) will be between `100-250kb`. Some might be as big as 1-2mb, but **never more than 5mb**. Get in touch with the maintainers as early as possible to talk about what kind of asset you want to contribute and how much space can be allocated for it. A **few selected** assets might be allowed with larger file sizes if their value for the gameplay allows it.
It is recommended to use techniques such as overlapping UV islands and tiling to reduce the texture sizes.


Another important factor for in-game performance is the **use of LODs**. There should generally be at least two of them. The last LOD should generally have 100 or less triangles.
