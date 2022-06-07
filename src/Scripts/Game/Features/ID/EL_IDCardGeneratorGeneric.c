[BaseContainerProps()]
class EL_IDCardGeneratorGeneric: EL_IDCardGeneratorBase
{
	protected string surname, lastname, birthday;
	//protected array<string> m_SmallDriversLicence[1];
	protected bool SmallDriversLicence, Weapon_License, Wood_Processing_License;
	void EL_IDCardGeneratorGeneric(string _surname, string _lastname, string _birthday, bool _SmallDriversLicence, bool _Weapon_License, bool _Wood_Processing_License)
	{
		surname = _surname;
		lastname = _lastname;
		birthday = _birthday;
		SmallDriversLicence = _SmallDriversLicence;
		Weapon_License = _Weapon_License;
		Wood_Processing_License = _Wood_Processing_License;
		

		

	}
	
}
