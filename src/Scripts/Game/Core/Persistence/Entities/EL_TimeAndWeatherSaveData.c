[BaseContainerProps()]
class EL_TimeAndWeatherSaveDataClass : EL_EntitySaveDataClass
{
};

[EL_DbName("TimeAndWeather")]
class EL_TimeAndWeatherSaveData : EL_EntitySaveData
{
	string m_sWeatherState;
	bool m_bWeatherLooping;

	int m_iYear, m_iMonth, m_iDay, m_iHour, m_iMinute, m_iSecond;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity entity, EL_EntitySaveDataClass attributes)
	{

		ReadMetaData(EL_Component<EL_PersistenceComponent>.Find(entity));

		TimeAndWeatherManagerEntity timeAndWeatherManager = TimeAndWeatherManagerEntity.Cast(entity);
		WeatherState currentWeather = timeAndWeatherManager.GetCurrentWeatherState();
		m_sWeatherState = currentWeather.GetStateName();
		m_bWeatherLooping = timeAndWeatherManager.IsWeatherLooping();

		timeAndWeatherManager.GetDate(m_iYear, m_iMonth, m_iDay);
		timeAndWeatherManager.GetHoursMinutesSeconds(m_iHour, m_iMinute, m_iSecond);

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity entity, EL_EntitySaveDataClass attributes)
	{
		TimeAndWeatherManagerEntity timeAndWeatherManager = TimeAndWeatherManagerEntity.Cast(entity);
		timeAndWeatherManager.ForceWeatherTo(m_bWeatherLooping, m_sWeatherState);
		timeAndWeatherManager.SetDate(m_iYear, m_iMonth, m_iDay);
		timeAndWeatherManager.SetHoursMinutesSeconds(m_iHour, m_iMinute, m_iSecond);

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		SerializeMetaData(saveContext);

		saveContext.WriteValue("m_sWeatherState", m_sWeatherState);
		saveContext.WriteValue("m_bWeatherLooping", m_bWeatherLooping);
		saveContext.WriteValue("m_iYear", m_iYear);
		saveContext.WriteValue("m_iMonth", m_iMonth);
		saveContext.WriteValue("m_iDay", m_iDay);
		saveContext.WriteValue("m_iHour", m_iHour);
		saveContext.WriteValue("m_iMinute", m_iMinute);
		saveContext.WriteValue("m_iSecond", m_iSecond);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		DeserializeMetaData(loadContext);

		loadContext.ReadValue("m_sWeatherState", m_sWeatherState);
		loadContext.ReadValue("m_bWeatherLooping", m_bWeatherLooping);
		loadContext.ReadValue("m_iYear", m_iYear);
		loadContext.ReadValue("m_iMonth", m_iMonth);
		loadContext.ReadValue("m_iDay", m_iDay);
		loadContext.ReadValue("m_iHour", m_iHour);
		loadContext.ReadValue("m_iMinute", m_iMinute);
		loadContext.ReadValue("m_iSecond", m_iSecond);

		return true;
	}
};
