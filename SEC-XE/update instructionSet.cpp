#include<bits/stdc++.h>
using namespace std;

char s[55];

int main()
{
	int i=0;
	freopen("instructionSet.txt","r",stdin);
	freopen("newInstructionSet.txt","w",stdout);
	while(~scanf("%s",s))
	{
		i++;
		printf("%s",s);
		if(i%4 == 0)printf("\n");
		else printf("\t");
	}
	return 0;
}
