file = open("input.bwt")
lst = list()
for string in file:
	i = 0;
	for char in string:
		i+=1
		#print("%d: %c \t"% (i, char, sorted(lst)), end="")
		lst.append(char)
print()
for i, char in enumerate(sorted(lst)):	
	print("%d: %c\t%c"% (i+1, char,string[i]), end="\t")
	if(i%5 == 0):
		print()
print()
