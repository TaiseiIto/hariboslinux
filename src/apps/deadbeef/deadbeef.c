int main(void)
{
	*(unsigned int *)0xdeadbeef = 0xdeadbeef;
	return 0;
}

