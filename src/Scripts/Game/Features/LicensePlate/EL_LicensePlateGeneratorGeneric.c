[BaseContainerProps()]
class EL_LicensePlateGeneratorGeneric: EL_LicensePlateGeneratorBase
{
	override string GenerateLicensePlate()
	{
		const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		string p1 = ALPHABET.Get(Math.RandomInt(0, ALPHABET.Length() - 1)) + ALPHABET.Get(Math.RandomInt(0, ALPHABET.Length() - 1));
		string p2 = Math.RandomInt(1000, 9999).ToString();
		
		return string.Format("%1 %2", p1, p2);
	}
}
