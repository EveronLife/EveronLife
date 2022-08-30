[EL_DbName(EL_TimeAndWeatherSaveData, "TimeAndWeather"), BaseContainerProps()]
class EL_TimeAndWeatherSaveData : EL_EntitySaveDataBase
{
	string m_sWeatherState;
	bool m_bWeatherLooping;

	int m_iYear, m_iMonth, m_iDay, m_iHour, m_iMinute, m_iSecond;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull IEntity worldEntity)
	{
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		TimeAndWeatherManagerEntity timeAndWeatherManager = TimeAndWeatherManagerEntity.Cast(worldEntity);
		if (!timeAndWeatherManager) return false;

		SetId(persistenceComponent.GetPersistentId());
		m_iLastSaved = persistenceComponent.GetLastSaved();

		WeatherState currentWeather();
		timeAndWeatherManager.GetCurrentWeatherState(currentWeather);
		m_sWeatherState = currentWeather.GetStateName();
		m_bWeatherLooping = timeAndWeatherManager.IsWeatherLooping();

		timeAndWeatherManager.GetDate(m_iYear, m_iMonth, m_iDay);
		timeAndWeatherManager.GetHoursMinutesSeconds(m_iHour, m_iMinute, m_iSecond);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull IEntity worldEntity)
	{
		TimeAndWeatherManagerEntity timeAndWeatherManager = TimeAndWeatherManagerEntity.Cast(worldEntity);
		if (!timeAndWeatherManager) return false;

		timeAndWeatherManager.ForceWeatherTo(m_bWeatherLooping, m_sWeatherState);

		timeAndWeatherManager.SetDate(m_iYear, m_iMonth, m_iDay);
		timeAndWeatherManager.SetHoursMinutesSeconds(m_iHour, m_iMinute, m_iSecond);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		saveContext.WriteValue("m_sId", GetId());
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);

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
		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);

		string id;
		loadContext.ReadValue("m_sId", id);
		SetId(id);

		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);

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
}
