//EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
//PrintFormat("%1: %2 from: %3", owner.GetName(), settings.m_bRegisterAutomatically, settings);
class EL_PersistenceComponentClass : ScriptComponentClass
{
	[Attribute(desc:"Type of save structure to represent persistent data of this entity.")]
	ref EL_PersistentEntity m_pSaveStructType;
}

class EL_PersistenceComponent : ScriptComponent
{
	string m_PersistenceId;
	
	protected DateTimeUtcAsInt m_LastSaved;
	
	DateTimeUtcAsInt GetLastSaved()
	{
		return m_LastSaved;
	}
	
	void Save()
	{
		m_LastSaved = EL_Utils.GetCurrentUtcAsInt();
	}
	
	override void OnPostInit(IEntity owner)
	{
		EL_PersistenceManager.GetInstance().StartTacking(owner);
	}
	
	void OnParentChanged(IEntity owner, IEntity newParent)
	{
		if(newParent)
		{
			//Stop tracking, parent is not responsible to tracking/persisting children
			EL_PersistenceManager.GetInstance().StopTacking(owner);
		}
		else
		{
			//No parent = toplevel world object, owner needs to be tracked again so this component recieves Save() invoke during auto-save.
			EL_PersistenceManager.GetInstance().StartTacking(owner);
		}
	}
	
	override void OnDelete(IEntity owner)
    {
		EL_PersistenceManager.GetInstance().StopTacking(owner);
    }
}

/*

	- Each world entity saves itself
		- easy logic, but now way to later do bulk operations?
	
	- Manager handles persistence?
		- all entities must register to it

	---

	Entity registers itself if toplevel object.
	If put inside another object then it removes itself
	Toplevel objects are responsible to save anything they contain.
		- this makes sense since nested objects will not be interesting to be loaded unless the container containing them is also loaded
		- reduces amount of entity save to db calls to just one container 
	
	how does a prefab define the handler how to save the entity?!?
		- dropdown for type and see if it can be overridden in child types?
			-> adding a component would mean having to override the whole thing ... meh
				-> no because components are handled invidiually -> typename of component maps to persister typename.
					-> if a sub mod wants to change how a component is persisted the mod the persister class through modded. no real disadvantage over inherit and "priority" ...
	
	pass entity into handler functions because the instance might/should be re-used!
	
	!!! entites without component or with disable component ARE NOT SAVED even if part of persistent container
	!!! items loaded back into world after update that are no longer supposed to be persistent are delete upon load (also from DB)
		-> check if we can know if component is present or disabled from prefab source, without having to spawn it first (saving performance). cache info to not recheck on every entity.

	!!! container saves slot idx + EL_PersistentEntity the instance of which he gets from the world entity persistence component -> and then its manager that produces it.

	!!! if toplevel container is not persistent, then items stored are not as well. add validator utility for all prefabs to see if there are any containers that do not save something
	
	!!! toplevel manager handles autosave aka triggers save on the objects and handles baked map object load to entity id lookup from db via one batch call. (name -> id as array result)
		-> Track & Untrack only really for autosave. (this includes baked map objects since they have components too, they just generally never untrack)
	
	!!! entity persistance handler gets events for hierarchy change, create, delete and handles db stuff itself. 
		-> vehicles handler can somehow check if object was meant to be put into garage and not delete and instead save. or on delete does nothing on the handler and the garage logic handles it?

	----

	Handlers or does a world entity simply define the save struct? 
	What value provides a handler over EL_PersistentEntity::From()
		- Control how to handle create / hierachy change / delete?
		- from is just on save ... so who deletes?
		- attach handler for existing scriptedstate. scripted state can not become its own record - why?

	----

	how to deal with static inherit...
*/
