# Database
The database framework allows AR to use external databases, adding, updating, querying, and deleting generic entities.

## EL_DbContext
The basis for all database communication is the [`EL_DbContext`](https://wb.reforger.dev/redirect?to=enfusion://ScriptEditor/Scripts/Game/Core/Database/EL_DbContext.c). It is opened based on a data source provided.  
To retrieve a context always use the `EL_DbContextFactory::GetContext()` function.

## Drivers
The data source selected for a context contains a connection string in URI form that determines which driver is selected for the actual database software communication. 
Currently, these drivers are included by default:

### EL_InMemoryDbDriver
Primarily for use during testing.  
Example connection string:
```
inmemory://EveronLife
```

### EL_JsonFileDbDriver
Locally stored JSON files.  
Options:
- `cache` [`true`|`false`] Enables caching of database entities to reduce the number of file IO calls.  

Example connection string:
```
jsonfile://EveronLife?cache=true
```

### EL_BinaryFileDbDriver
Locally stored binary files.  
Options:
- `cache` [`true`|`false`] Enables caching of database entities to reduce the number of file IO calls.  

Example connection string:
```
binfile://EveronLife?cache=true
```

## EL_DbEntity
The `EL_DbContext` function api works with a base class [`EL_DbEntity`](https://wb.reforger.dev/redirect?to=enfusion://ScriptEditor/Scripts/Game/Core/Database/EL_DbEntity.c) that represents one distinct entity inside the database.  

### Entity ID
Each `EL_DbEntity` is identified with an [UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier) represented as 36 character hexadecimal string, that can be assigned manually via `EL_DbEntity::SetId()` or will be assinged by the `EL_DbContext::AddOrUpdate()` method.  

The UUID is generated in script to avoid any round trips to the database and allow for deterministic async processing of all requests.
| 5a3ea1c5     | - | 0000      | - | 0001     | - | 0000          | - | 0018         | 21094200 |
|--------------|---|-----------|---|----------|---|---------------|---|--------------|----------|
| DateTime UTC |   | Hive High |   | Hive Low |   | Sequence High |   | Sequence Low | Random   |
- `DateTime UTC` (not UNIX, see [`EL_DateTimeUtcAsInt`](https://wb.reforger.dev/redirect?to=enfusion://ScriptEditor/Scripts/Game/Core/EL_DateTimeUtcAsInt.c)) gives the ID general sortability for binary tree-based storage systems and minimizes collisions as each ID becomes unique per second
- `Hive` is an index given to each hive connected to the database system. As long as hives are not needed this is always index `1`.
- `Sequence` gives guaranteed uniqueness to the ID due to the combination with the timestamp. Starts at 0 per session. There can be skips in the sequence number.
- `Random` aims to minimize predictability by malicious actors. 

### Creating your own database entity
To create a database entity just inherit from the `EL_DbEntity` base class. Optionally you can decorate the class with the `EL_DbName` attribute.  
It is used when the type is stored somewhere as a string e.g. JSON serialization.
```cs
[EL_DbName(TAG_MyCustomDbEntity, "MyCustomDbEntity")]
class TAG_MyCustomDbEntity : EL_DbEntity
{
}
```
> **Warning**  
> Because of the serialization support all DB entity classes MUST have 0 parameters in their constructor if a custom one is defined.

## Finding entities
Retrieving an entity from the database is done through the `EL_DbContext::FindAll()` method. Besides the typename of the entity to search for it uses an `EL_DbFindCondition`, result ordering criteria as well as limit and offset for paginated loading.

### EL_DbFindCondition
The [`EL_DbFindCondition`](https://wb.reforger.dev/redirect?to=enfusion://ScriptEditor/Scripts/Game/Core/Database/EL_DbFindCondition.c) the the base class of the very powerfuly scripted query api.

#### Find by ID
To create a condition it is recommended to use the builder for it. A simple find by ID can look like this:
```cs
EL_DbFindCondition condition = EL_DbFind.Id().Equals("UUID-GOES-HERE");
EL_DbFindResults<EL_DbEntity> findResult = m_DbContext.FindAll(TAG_MyCustomDbEntity, condition, limit: 1)
```

#### Find by field
Besides the `Id()` shortcut any other field can be queried using the `Field()` method on the builder.
```cs
EL_DbFindCondition condition = EL_DbFind.Field("m_Name").Equals("Foo");
```
The field name supports "dot-notation" so you can navigate complex types as well as collections using it.
- `EL_DbFind.Field("fieldName.subField")` same as `EL_DbFind.Field("fieldName").Field("subField")`
- `EL_DbFind.Field("floatArray.0")` evalulates the first element of the array on the entity

#### Field condition builder
Summary of the available filed condition builder functions:
| Function           | Used on              | Description                                                        |
|--------------------|----------------------|--------------------------------------------------------------------|
| Not                | *                    | Inverts the next statement                                         |
| Null               | Complex/Collection   | Checks if the field is null                                        |
| Empty              | Primitive/Collection | Checks if the field is empty collection or primitive value default |
| Equals             | *                    | Full equality check                                                |
| EqualsAnyOf        | Primitive            | Equality check comparison against multiple values                  |
| LessThan           | Numeric              | `<` operator                                                       |
| LessThanOrEqual    | Numeric              | `<=` operator                                                      |
| GreaterThan        | Numeric              | `>` operator                                                       |
| GreaterThanOrEqual | Numeric              | `>=` operator                                                      |
| Between            | Numeric              | LOWER `<` VALUE `<` UPPER                                          |
| Contains           | String               | Case-sensitive string in string search                             |
| Contains           | Collection           | Value inside collection                                            |
| ContainsAnyOf      | String               | Same as `Contains` with multiple comparison values                 |
| ContainsAnyOf      | Collection           | Same as `Contains` with multiple comparison values                 |
| ContainsAllOf      | Collection           | Collection must at least contain all the comparison values         |
| Length             | String               | Character count of string comparison                               |
| Count              | Collection           | Number of elements inside the collection                           |
| At                 | Collection           | Get collection item at index                                       |
| FirstOf            | Collection           | Evaluates on the first collection that matches the type            |
| AllOf              | Collection           | Evaluates on all collection items that match the type              |
| Any                | Collection           | True if any collection item matches the condition                  |
| All                | Collection           | True if all collection items match the condition                   |
| Keys               | Collection           | Get the key collection of a map                                    |
| Values             | Collection           | Get the value collection of a map                                  |

To combine multiple conditions together you can use `And()` and `Or()` to build like so
```cs
EL_DbFind.And({
	condition1,
	condition2,
	conditionN
});
```

Combining all of this together provides various ways to write precise database queries:
```cs
EL_DbFindCondition condition = EL_DbFind.Or({
	EL_DbFind.Field("A").Not().Null(),
	EL_DbFind.Field("B").Empty(),
	EL_DbFind.And({
		EL_DbFind.Field("CString").Contains("SubString"),
		EL_DbFind.Field("DFloatArray").Equals(EL_DbValues<bool>.From({true, false, true, true})),
		EL_DbFind.And({
			EL_DbFind.Field("E.m_Numbers").Contains(100),
			EL_DbFind.Field("F.m_ComplexWrapperSet").FirstOf(Class).Field("someNumber").Not().EqualsAnyOf(EL_DbValues<int>.From({1, 2}))
		}),
		EL_DbFind.Or({
			EL_DbFind.Field("G").EqualsAnyOf(EL_DbValues<int>.From({12, 13}))
		})
	})
});
```

> **Note**
> If you get an error like "formula too complex" you need to split up your conditions into a few sub-conditions and join them together separately, as the script VM has some limits on how nested the builder can be.

##### Performance considerations 
If possible make the condition constant inside the function, that way it is created only once, even if the function is executed multiple times. For this to be possible all search conditions must be of a known constant value as well.
You can also cache more complex conditions as class members if you build them dynamically once and re-use them after.  
```cs
void MyFunction()
{
	const EL_DbFindCondition constNameEqualsFoo = EL_DbFind.Field("m_Name").Equals("Foo");
	...
}
```

##### EL_DbValues wrapper
In some of the examples the utility class `EL_DbValues<T>` is used. Especially for const allocations, this ensures that the array data you pass into the builder is handled correctly.
It is generally only needed for the static initializer syntax `{...}`. An already strong typed initialized array from previous code can be passed without the wrapper class safely.
The compiler will complain or the built condition will have no array value if it was not used in a situation where it should have been.

## Result handling
After the find operation concludes the DB context provides the results in three different ways, depending on if the async API was used or not.

> **Warning**  
> The sync API was designed to work together with the `thread` keyword to make results "awaitable". Unfortunately, the keyword is broken (https://feedback.bistudio.com/T167281).
> Until this is fixed or in the case of FileIO-based drivers the operations are blocking the main thread of the game, so use them with care.
> It is recommended to use async callbacks when possible so the game mode scales well with higher loads and when using non-blocking drivers (e.g. web API ones or external async plugins)

### FindAll
`FindAll` returns an `EL_DbFindResults<EL_DbEntity>` instance which contains the status code enum as well as any results from the operation if it was successful.
```cs
EL_DbFindResults<EL_DbEntity> result = dbContext.FindAll(...);
if (result.GetStatusCode() != EL_EDbOperationStatusCode.SUCCESS) return; //Something went wrong
if (!result.Success()) return; //same as above, for convenience
array<ref EL_DbEntity> entities = results.GetEntities();
...
```

### FindAllAsync callback classes
If you want to process the result inside a separate callback class you can create one and implement the functions defined in the base class to get already strong typed results. 
Besides `EL_DbFindCallback<T>` for multiple return values there is also `EL_DbFindCallbackSingle<T>` for expecting a single result as well as `EL_DbFindCallbackSingleton<T>` to create a result instance if nothing was found.
```cs
class MyCustomDbEntityFindCallback : EL_DbFindCallback<TAG_MyCustomDbEntity>
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(Managed context, array<ref TAG_MyCustomDbEntity> resultData);

	//------------------------------------------------------------------------------------------------
	void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode);
}
...
void DoFind()
{
	MyCustomDbEntityFindCallback myCallback();
	dbContext.FindAllAsync(TAG_MyCustomDbEntity, callback: myCallback);
}
```

### FindAllAsync callback method
As an alternative to a separate class, the result can be handled by a dedicated method.
```cs
class MyDbLogic
{
	//------------------------------------------------------------------------------------------------
	void DoFind()
	{
		EL_DbFindCallbackSingle<TAG_MyCustomDbEntity> myCallback();
		myCallback.ConfigureInvoker(this, "HandleResult");
		dbContext.FindAllAsync(TAG_MyCustomDbEntity, callback: myCallback);
	}
  
	//------------------------------------------------------------------------------------------------
	void HandleResult(Managed context, TAG_MyCustomDbEntity result)
	{
		Print(context);
		Print(result);
	}
}
```

### Result ordering
If multiple results are returned by the operation they can be automatically ordered by field values, again using "dot-notation".
The sorting order is given via a nested array, with the second, thrid, ... n-th element only being used if the ordering based on the previous element left two or more entities with the same sorting result.
Options for the sort direction are `ASC`, `DESC` as invariant strings or the `EL_DbEntitySortDirection` "enum".
```cs
auto orderedResults = dbContext.FindAll(..., orderBy: {{"child.subField", "ASC"}, {"thenByField", "DESC"}});
```

## Repositories
To make the handling of database entities easier the framework comes with a utlitly wrapper class called [`EL_DbRepository<T>`](https://wb.reforger.dev/redirect?to=enfusion://ScriptEditor/Scripts/Game/Core/Database/EL_DbRepository.c). 
It contains a few commonly used methods such as `Find()` by id, `FindSingleton()`, `FindFirst()` by condition, as well as providing already casted results of the target entity type.
All DB entities can be handled automatically through the default repository implementation. To get a repository for an entity there is a utility class:
```cs
EL_DbRepository<TAG_MyCustomDbEntity> repository = EL_DbEntityHelper<TAG_MyCustomDbEntity>.GetRepository(dbContext);
```

### Custom repositories
Adding a customized implementation of a repository is also possible. In it, frequently used DB operations can be stored as re-useable methods.
```cs
[EL_DbRepositoryType(TAG_MyCustomDbEntityRepository, TAG_MyCustomDbEntity)]
class TAG_MyCustomDbEntityRepository : EL_DbRepository<TAG_MyCustomDbEntity>
{
	EL_DbFindResult<TAG_MyCustomDbEntity> FindByIntValue(int value)
	{
		return FindFirst(EL_DbFind.Field("m_iIntValue").Equals(value));
	}
}
```

An instance of this repository will be returned by the `EL_DbEntityHelper<T>::GetRepository()` method, however to also strong cast the resulting repository, a different utility class can be used.
```cs
TAG_MyCustomDbEntityRepository repository = EL_DbRepositoryHelper<TAG_MyCustomDbEntityRepository>.Get(dbContext);
```

Manually getting/creating a repository instance is possible through the `EL_DbRepositoryFactory::GetRepository()` function.
