class EL_CharacterOwnerComponentClass: ScriptComponentClass
{
};


class EL_CharacterOwnerComponent: ScriptComponent
{
	protected string m_sCharacterId;
	
	//------------------------------------------------------------------------------------------------
	void SetCharacterOwner(string characterId)
	{
		m_sCharacterId = characterId;
	}	
	
	//------------------------------------------------------------------------------------------------
	string GetCharacterOwner()
	{
		return m_sCharacterId;
	}
};

