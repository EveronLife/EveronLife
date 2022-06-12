class EL_BaseSirenManagerComponentClass: GameComponentClass
{
};

class EL_BaseSirenManagerComponent: GameComponent
{
	proto external bool IsSirenOn();
	
	// callbacks
	
	/*!
	Called during EOnInit.
	\param owner Entity this component is attached to.
	*/
	event protected void OnInit(IEntity owner);
	/*!
	Called during EOnFrame.
	\param owner Entity this component is attached to.
	\param timeSlice Delta time since last update.
	*/
	event protected void OnFrame(IEntity owner, float timeSlice);
};