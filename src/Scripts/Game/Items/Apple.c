class AppleClass : GenericEntityClass
{

};

class Apple : GenericEntity
{
	void Apple(IEntitySource src, IEntity parent)
	{
		Print("Apple created");
	}

	void ~Apple()
	{
		Print("Apple destroyed");
	}
};
