[BaseContainerProps()]
class EL_IDCardGeneratorGeneric: EL_IDCardGeneratorBase
{
	protected string surname, lastname, birthday
	protected array<enum> licenses = {};
	void EL_IDCardGeneratorGeneric(string _surname, string _lastname, string _birthday)
	{
		surname = _surname;
		lastname = _lastname;
		birthday = _birthday;
		
		licenses.Insert(EL_Licenses.Small_Driving_License);
		
		return string.Format("%1 %2 %3", surname, lastname, birthday);
	}
	
	override string GenerateIDCard()
	{
	EL_IDCardGeneratorGeneric Test_Player = new EL_IDCardGeneratorGeneric("Tombo", "TheFreaks", "19.06.2002");
	}
	

	
}
