class TestCallback : RestCallback
{
    override void OnSuccess( string data, int dataSize )
    {
        Print(" !!! OnSuccess() size=" + dataSize );
        if( dataSize > 0 )
            Print(data); // !!! NOTE: Print() will not output string longer than 1024b, check your dataSize !!!
    };

}