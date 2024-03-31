modded class SCR_InvCallBack
{
	//------------------------------------------------------------------------------------------------
	protected override void OnComplete()
	{
		// TODO: Needed until https://feedback.bistudio.com/T167936 is fixed, because item is merged on quantity pickup auto combine
		if (!m_pItem) return;
		super.OnComplete();
	}
}
