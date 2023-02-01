[BaseContainerProps()]
class EL_LicensePlateGeneratorGeneric: EL_LicensePlateGeneratorBase
{
	override string GenerateLicensePlate()
	{
		string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		string p1 = alphabet.Get(Math.RandomInt(0, 25)) + alphabet.Get(Math.RandomInt(0, 25));
		string p2 = alphabet.Get(Math.RandomInt(0, 25)) + alphabet.Get(Math.RandomInt(0, 25));
		string p3 = Math.RandomInt(100, 9999).ToString();
		
		return string.Format("%1 %2 %3", p1, p2, p3);
	}
}
