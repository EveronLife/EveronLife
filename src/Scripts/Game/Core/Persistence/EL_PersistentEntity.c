class EL_PersistentComponent
{
	//Always belong to an owner entity so they do not need their own id
}

[BaseContainerProps()]
class EL_PersistentEntity : EL_DbEntity
{
	ResourceName m_Prefab;
	
	ref map<typename, ref EL_PersistentComponent> m_Components;
	
	bool ReadFrom(IEntity worldEntity)
	{
		//m_Prefab = ...
		//m_Components = ...
		return true;
	}

	bool ApplyTo(IEntity worldEntity)
	{
		return true;
	}
	
	IEntity Spawn()
	{
		//spawn entity back and then call to ApplyTo
	}
}

/*

	Spawn entity from base class logic / externally (same where we create the scripted instance)?
	
	Scripted classes explictly load the db record and process it just as struct?
		- who has the logic to do save on autosave ...
		- maybe those classes just need to have public fields so applyto in the struct can fill it out, or it has a function that accepts the data manually or the whole struct so they can stay private.

	void ReadFrom(IEntity worldEntity);

	void ApplyTo(IEntity worldEntity)
	{
		//do base class stuff ...
		super.ApplyTo(worldEntity);

		//now do derived stuff
	}
	
	---

	multiple scripted states, how do they know who is who ... maybe every scripted state must be the db entity being loaded?
		- see how to implement a dummy singleton with data from the db
	
	---

	create of scripted state track via constructor
	delete of scripted state untrack via destructor?
	do we still need a handler?

	without a handler where to add auto untrack on delete vs - not do that for vehicles?
	
	---

	- db records must be instanced so they can call super
	- entity manager needed that handles create+update, remove of the entity and creates the record instance and calls ReadFrom(GetOwner()) on it
		- How does the entity manager know which record type to spawn?
			- record types get decorated with attributes to be selectable?
				- most just need a generic world entity record ...
				- maybe player is has some more logic on it
				- can not use the object instance directly from attribute, we get the typename and spawn a new one for save process?

	---

	toplevel objects. on unregister save those being unregisterd. when they are tracked again, remove from that list
	on global save mass remove all toplevel entities that do not exist anymore, then iterate all existing objects

	better:
	have the persistence track ALL enties with their ids. hierarchy change only removes it from toplevel objects being saved.
	in the backend the existing object instance can be found on save of container and their parent updated for example.
	toplevel object is only list of ids there
	deleted are only objects that ondelete is called without persistence being in shutdown mode

*/
