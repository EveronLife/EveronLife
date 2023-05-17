class EL_DbEntityIdGenerator
{
	protected static int s_iSequence;
	protected static ref RandomGenerator s_pRandom;

	//------------------------------------------------------------------------------------------------
	static string Generate()
	{
		if (!s_pRandom)
		{
			s_pRandom = new RandomGenerator();
			s_pRandom.SetSeed(System.GetUnixTime());
		}

		//No need to look at past generated ids in db for conflict, because they have older timestamps
		string timeHex = EL_Utils.IntToHex(System.GetUnixTime(), false, 8);

		//Sequence returns to 0 on overflow
		string seqHex = EL_Utils.IntToHex(s_iSequence++, false, 8);
		if (s_iSequence == int.MAX)
			s_iSequence = 0;

		//Random component to ensure that even if all sequentials are generated in the same second it stays unique
		string randHex = EL_Utils.IntToHex((int)(s_pRandom.RandFloatXY(0, int.MAX)), false, 8);
		string randHex2 = EL_Utils.IntToHex((int)(s_pRandom.RandFloatXY(0, int.MAX)), false, 8);

		// TTTT TTTT-SEQ1-SEQ2-RND1-RND1 RND2 RND2
		return string.Format("%1-%2-%3-%4-%5%6",
			timeHex,
			seqHex.Substring(0, 4),
			seqHex.Substring(4, 4),
			randHex.Substring(0, 4),
			randHex.Substring(4, 4),
			randHex2);
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_iSequence = 0;
		s_pRandom = null;
	}
};
