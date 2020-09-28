#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

FILE* pipe;
FILE* logfile;
char* root;
char* output;
char* command;
char* cfile;
char* pwd;
char* logpath = NULL;
struct memrgstr
{
	char** locs;
	int num;
} reg;

void close(void);
int xmlexport(char* name);
int xmlparse(char* name);
void setprefixes(void);
int apksort(char* path);
int apklocate(char* path);
char* pthpkgr(char* path);
void shrtcmdargs(int c, char** args);
char* strcnct(int vnum, ...);
char* syscmdread(FILE*);
char* syscmd(char*);
char* flname(char*);
void rgstrclr(void);
void rgstr(char*);
void apkgroup(void);
char* strextract(char, char, char*);
char* strstore(char*, char*);
void logs(char*);

int main(int argc, char *argv[]) 
{	
	reg.num = 0;
	shrtcmdargs(argc, argv);
	setprefixes();
	char* tmp = strcnct(3 , "find ", pthpkgr(root), " -name *.apk");
	command = calloc(strlen(tmp)+1, 1);
	memcpy(command, tmp, strlen(tmp)+1);
	rgstrclr();
	FILE* find = popen(command, "r");
	do
	{	
		tmp = syscmdread(find);
		cfile = realloc(cfile, strlen(tmp)+1);
		memcpy(cfile, tmp, strlen(tmp)+1);
		if(cfile[0] == -1)
		{
			printf("Finalizing execution... \n");
			break;
		}
		logs(strcnct(3 ,"\nLocated: ", cfile, "\n"));
		rgstrclr();
	apksort(cfile);
	}while(cfile[0] != -1);
	apkgroup();
	close();
	return 0;
}

void shrtcmdargs(int c, char** args)
{
	for(int i = 1; i < c; i++)
	{
		if(*args[i] == '-' && (*(args[i]+2) == '=' || (strlen(args[i]) == 2)))
		{
			int s;
			char opt;
			char* arg;
			s = strlen(args[i])-3;
			opt = *(args[i]+1);
			arg = calloc(s+3, 1);
			if(s > 3)
			{
				memcpy(arg, args[i]+3, s);
			}
			switch(opt)
			{
					case 114:
						;
						command = calloc(s+3, 1);
						memcpy(command, "cd ", 3);
						arg = pthpkgr(arg);
						strcat(command, arg);
						pipe = popen(command,"r");
						if(pclose(pipe) == 0)
						{
							root = calloc(s+3, 1);
							memcpy(root, arg, s+3);
						}
						else 
						{
							printf("%s", arg);
							printf(" is not accessible..\n");
							close();
						}
						//free(arg);
						break;
					case 111:
						;
						command = calloc(s+5, 1);
						memcpy(command, "cd \"", 4);
						strcat(command, arg);
						strcat(command, "\"");
						pipe = popen(command,"r");
						if(pclose(pipe) == 0)
						{
							output = calloc(s+3, 1);
							memcpy(output, arg, s+3);
						}
						else 
						{
							printf("%s", arg);
							printf(" is not accessible.\n");
							close();
						}
						free(arg);
						break;
					case 108:
						free(arg);
						arg = args[i]+3;
						command = strcnct(2, "touch ", arg);
						logpath = calloc(strlen(arg)+1, 1);
						memcpy(logpath, arg, strlen(arg));
						pipe = popen(command,"r");
						logfile = fopen(logpath, "a");
						if(pclose(pipe) != 0 || logfile == NULL)
						{
							printf("%s", logpath);
							printf(" is not accessible.\n");
							printf("Error Number: %d\n", errno);
							perror(logfile);
							close();
						}
						rgstrclr();
						break;
					case 109:
						//reg = reg | 0x4;
						break;
					default:
						printf("%s", args[i]);
						printf(" is not a valid option.\n");
						//free(arg);
						close();
						break;
			}
			rgstrclr();
		}
		else
		{
			printf("%s", args[i]);
			printf(" is not a valid option.\n");
			close();
		}
	}
}
void close(void)
{
	printf("Program exiting...\n");
	free(output);
	free(root);
	free(command);
	exit(0);
}
char* pthpkgr(char* path)
{
	char* ptemp = calloc((strlen(path)+3), 1);
	strcat(ptemp, "\"");
	strcat(ptemp, path);
	strcat(ptemp, "\"");
	//printf("pthpkgr Output: %s\n", ptemp);
	rgstr(ptemp);
	return ptemp;
}
void setprefixes(void)
{
	int n;
	char* temp;
	if(root == NULL)
	{
		pipe = popen("pwd", "r");
		temp = syscmdread(pipe);
		root = calloc(strlen(temp)+2, 1);
		memcpy(root, temp, strlen(temp));
		strcat(root, "/");
		free(temp);
		pclose(pipe);
	}
	else
	{
		if(root[0] == 34)
		{
			for(int i = 0; i < strlen(root)+2; i++)
			{
				if(root[i+1] != 34)
				{
					root[i] = root[i+1];
				}
				else 
				{
					root[i] = 0;
					root = realloc(root, strlen(root)+1);
					break;
				}
			}
		}
	}
	if(output == NULL)
	{
		pipe = popen("pwd", "r");
		temp = syscmdread(pipe);
		pclose(pipe);
		output = calloc(strlen(temp)+1, 1);
		memcpy(output, temp, strlen(temp)+1);
		output = realloc(output, strlen(output)+strlen("/Output/")+1);
		strcat(output, "/Output/");
		//rgstrclr();
		temp = strcnct(2 ,"mkdir -v ", pthpkgr(output));
		pipe = popen(temp, "r");
		temp = syscmdread(pipe);
		if(strstr(temp, ": Permission denied") != 0)
		{
			printf("Output path: %s", output);
			printf(" cannot be accessed.\n Program exiting...");
			close();
		}
	}
	else
	{
		if(output[0] == 34)
		{
			for(int i = 0; i < strlen(root); i++)
			{
				if(output[i+1] != 34)
				{
					output[i] = output[i+1];
				}
				else 
				{
					output[i] = 0;
					output = realloc(output, strlen(output)+1);
					break;
				}
			}
		}
	}
	temp = syscmd("pwd");
	pwd = calloc(strlen(temp)+2, 1);
	memcpy(pwd, temp, strlen(temp)+1);
	strcat(pwd, "/");
	printf("PWD: %s\n", pwd);
	printf("Root Path: %s\n", root);
	printf("Output Path: %s\n\n", output);
}
int apksort(char* filepath)
{
	if(strstr(filepath, output) == 0)
	{
		char* ascmd = strcnct(2,"mkdir -p ", pthpkgr(strcnct(2, output, ".tmp/")));
		char* astmp = strcnct(2, output, ".tmp/");
		char* tmpfldr = calloc(strlen(astmp)+1, 1);
		memcpy(tmpfldr, astmp, strlen(astmp)+1);
		//rgstrclr();
		char* tmpflpth;
		astmp = strcnct(2,tmpfldr,flname(filepath));
		tmpflpth = calloc(strlen(astmp)+1, 1);
		memcpy(tmpflpth, astmp, strlen(astmp)+1);
		//rgstrclr();
		char* pkgname = 0;
		char* vrsnname = 0;
		char* vrsncode = 0;
		char split = 0;
		FILE* sort = popen(ascmd,"r");
		rgstrclr();
		ascmd = syscmdread(sort);
		if(pclose(sort) != 0)
		{
			ascmd = strcnct(4,"rm -r ", pthpkgr(strcnct(2, output, ".tmp/")), " && mkdir ", pthpkgr(strcnct(2, output, ".tmp/")));
			sort = popen(ascmd, "r");
			ascmd = syscmdread(sort);
			rgstrclr();
			pclose(sort);
			if(strstr(ascmd, ": cannot"))
			{
				logs("Unable to create temporary folder. \n Program Exiting...");
				close();
			}
			rgstrclr();
		}
		astmp = syscmd(strcnct(4,"cp -n ", pthpkgr(filepath)," ", pthpkgr(tmpfldr)));
		if(strstr(astmp, "cannot"))
			{
				logs("Unable to copy file:");
				logs(strcnct(2, filepath, "\n"));
				logs("Exiting program...");
				close();
			}
			rgstrclr();
			ascmd = strcnct(5, "unzip -o -qq ", pthpkgr(tmpflpth)," AndroidManifest.xml ", "-d ", pthpkgr(tmpfldr));
			sort = popen(ascmd, "r");
			astmp = syscmdread(sort);
			if(strstr(astmp, "cannot"))
			{
				logs("Unable to extract Manifest:");
				logs(strcnct(2, astmp, "\n"));
				logs("Exiting program...");
				close();
			}
			rgstrclr();
			pclose(sort);
			//ascmd = strcnct(3, pthpkgr(strcnct(2, pwd, "axmldec"))," ", pthpkgr(strcnct(3, tmpfldr,"AndroidManifest.xml")));
			ascmd = strcnct(4, strcnct(2, "cd ", pthpkgr(tmpfldr))," && ",pthpkgr(strcnct(2, pwd, "axmldec")), " AndroidManifest.xml");
			sort = popen(ascmd, "r");
			char* ploc = 0;
			char* vcloc = 0;
			char* vnloc = 0;
			int len;
			char* pkgfldrpth;
			do
			{
				astmp = syscmdread(sort);
				//printf("astmp: %s\n", astmp);
				ploc = strstr(astmp, "package=");
				if(ploc != 0)
				{
					len = strlen(&ploc[9]);
					for(int i = 0; i < len; i++)
					{
						if(ploc[9+i] == 34)
						{
							pkgname = calloc(i, 1);
							memcpy(pkgname, &ploc[9], i);
							break;
						}
					}
				}
				vcloc = strstr(astmp, "android:versionCode=");
				if(vcloc != 0)
				{
					len = strlen(&vcloc[21]);
					for(int i = 1; i < len; i++)
					{
						if(vcloc[21+i] == 34)
						{
							vrsnname = calloc(i, 1);
							memcpy(vrsnname, &vcloc[21], i);
							break;
						}
					}
				}
				vnloc = strstr(astmp, "android:versionName=");
				if(vnloc != 0)
				{
					len = strlen(&vnloc[21]);
					for(int i = 1; i < len; i++)
					{
						if(vnloc[21+i] == 34)
						{
							vrsncode = calloc(i, 1);
							memcpy(vrsncode, &vnloc[21], i);
							break;
						}
					}
				}
				if(strstr(astmp, "android:isSplitRequired=\"true\"") || strstr(astmp, "configForSplit="))
				{
					split = 1;
				}
				
				if(pkgname != 0 && vrsnname != 0 && split)
				{
					//rgstrclr();
					break;
				}
			}	while(*astmp != -1);
			rgstrclr();
			pclose(sort);
			logs(strcnct(3,"Package Name: ", pkgname, "\n"));
			logs(strcnct(3,"Version Name: ", vrsnname, "\n"));
			logs(strcnct(3,"Version Code: ", vrsncode, "\n"));
			//logs(strcnct(3,"Split: ", split, "\n"));
			rgstrclr();
			/*printf("Package Name: %s\n", pkgname);
			printf("Version Name: %s\n", vrsnname);
			printf("Version Code: %s\n", vrsncode);
			printf("Split: %i\n", split);*/
			
			if(pkgname == 0)
			{
				printf("This APK is damaged. Moving to folder \"_Damaged\".\n");
				ascmd = strcnct(2,"mkdir ", pthpkgr(strcnct(2, output, "_Damaged/")));
				syscmd(ascmd);
				astmp = pthpkgr(strcnct(2, output, "_Damaged/"));
				system(strcnct(4 ,"mv ", pthpkgr(tmpflpth)," ", pthpkgr(astmp)));
				printf("Move Complete. \n");
				return 1;
			}
			
			if(vrsncode == 0)
			{
				vrsncode = "_";
			}
			if(vrsnname == 0)
			{
				vrsnname = "_";
			}
			astmp = strcnct(3, output, pkgname, "/");
			pkgfldrpth = calloc(strlen(astmp)+1, 1);
			memcpy(pkgfldrpth, astmp, strlen(astmp)+1);
			sort = popen(strcnct(2,"mkdir -p ", pthpkgr(pkgfldrpth)), "r");
			ascmd = syscmdread(sort);
			
			if(pclose(sort))
			{
				printf("Unable to create package folder. \n Program Exiting...");
				close();
			}
			rgstrclr();
			if(split)
			{
				logs("Split: Yes\n");
				astmp = strcnct(6, pkgname,"-",vrsncode,"(",vrsnname,")");
				logs(strcnct(3, "Package renamed to: ", astmp, "\n"));
				astmp = pthpkgr(strcnct(5, output, pkgname,"/",astmp,"/"));
				//astmp = pthpkgr(strcnct(9, output, pkgname,"/", pkgname,"-",vrsncode,"(",vrsnname,")"));
				sort = popen(strcnct(2,"mkdir -p ", astmp), "r");
				ascmd = syscmdread(sort);
				pclose(sort);
				sort = popen(strcnct(4, "mv -n ", pthpkgr(tmpflpth), " -t ", astmp), "r");
				logs(strcnct(3, "Package moved to: ", astmp,"\n"));
				ascmd = syscmdread(sort);
				pclose(sort);
				rgstrclr();
			}
			else
			{
				logs("Split: No\n");
				astmp = strcnct(8 , tmpfldr, pkgname, "-", vrsncode,"(",vrsnname,")", ".apk");
				system(strcnct(4 ,"mv ", pthpkgr(tmpflpth)," ", pthpkgr(astmp)));
				free(tmpflpth);
				tmpflpth = calloc((strlen(astmp)+1), 1);
				memcpy(tmpflpth, astmp, strlen(astmp)+1);
				rgstrclr();
				astmp = pthpkgr(pkgfldrpth);
				sort = popen(strcnct(4, "mv -n ", pthpkgr(tmpflpth), " -t ", astmp), "r");
				logs(strcnct(3, "Package moved to: ", astmp,"\n"));
				ascmd = syscmdread(sort);
				pclose(sort);
				rgstrclr();
			}
			ascmd = strcnct(2,"rm -r ", pthpkgr(strcnct(2, output, ".tmp/")));
			sort = popen(ascmd, "r");
			ascmd = syscmdread(sort);
			rgstrclr();
			pclose(sort);
	}
	return 0;
}
char* strcnct(int vnum, ...)
{
	char* str;
	char* va;
	va_list arglist;
	va_start(arglist, vnum);
	va = va_arg(arglist, char*);
	//rgstr(va);
	str = calloc(strlen(va)+1, 1);
	memcpy(str, va, (strlen(va))+1);
	for(int i = 1; i < vnum; i++)
	{
		va = va_arg(arglist, char*);
		if(va == NULL)
		{
			continue;
		}
		str = realloc(str, (strlen(str) + strlen(va))+ 1);
		strcat(str, va);
		//rgstr(va);
		//free(tmp);
	}
	va_end(arglist);
	//printf("\nstrcnct output: %s\n\n", str);
	rgstr(str);
	return str;
}
char* syscmdread(FILE* cmdpipe)
{
		char* input;
		input = calloc(100, 1);
		int n = 0;
		do
		{
			input[n] = fgetc(cmdpipe);
			//printf("syscmdread char: %i\n", input[n]);
			if(input[n] == -1)
			{
				return input;
			}
			if(((n+3) % 100) == 0)
			{
				input = realloc(input, (n+3+100));
			}
			n++;
		}	while(input[n-1] != -1 && input[n-1] != 10);
		input[n-1] = 0;
		//input[n] = 0;
		input = realloc(input, strlen(input)+1);
		//printf("syscmdread: %s\n", input);
		rgstr(input);
		return input;
}
char* syscmd(char* cmd)
{
	FILE* proc = popen(cmd, "r");
	char* response = syscmdread(proc);
	pclose(proc);
	return response;
}
char* flname(char* path)
{
	char* nm;
	int len = strlen(path);
	for(int i = len; i >= 0; i--)
	{
		if(path[i] == 47 && i != len)
		{
			nm = calloc(strlen(&path[i+1])+1, 1);
			strcpy(nm, &path[i+1]);
			break;
		}
	}
	//printf("\n flname Output: %s\n", nm);
	rgstr(nm);
	return nm;
}
void rgstr(char* loc)
{
	short exists = 0;
	/*printf("\nAttempting to Register Address: %p", loc);
	printf(" as Index Number: %i", reg.num);
	printf(" containing string: %s\n", loc);*/
	for(int i = reg.num-1; i >= 0; i--)
	{
		if(loc == reg.locs[i] || loc == *reg.locs[0])
		{
			//printf("reg.locs[i] = %p\n", reg.locs[i]);
			/*printf("Registration of %p", loc);
			printf(" failed. (Redundant to index: ");
			printf("%i", i);
			printf(")\n\n");*/
			exists = 1;		
		}
	}
	if(exists != 1)
	{
		reg.locs = realloc(reg.locs, ((reg.num+1) * sizeof(void*)));
		reg.locs[reg.num] = loc;
		reg.num++;
		/*printf("%p", loc);
		printf(" registered. \n\n");*/
	}
}
void rgstrclr(void)
{
	if(reg.num > 1)
	{
		printf("\n");
		for(int i = reg.num-1; i >= 0; i--)
		{
			/*printf("Freeing Pointer: %p", reg.locs[i]);
			printf(" Index: %i", i);
			printf(" containing string: %s\n", reg.locs[i]);*/
			//printf("reg.locs address: %p\n", *reg.locs);
			
			if(i != 0)
			{
				free(reg.locs[i]);
			}
			//else free(*reg.locs[i]);
		}
		//free(*reg.locs[0]);
		reg.num = 0;
	}
}
void apkgroup(void)
{
	char* agtmp = 0;
	char* fldr = 0;
	char* agpkgnm = 0;
	char* agpkgvn = 0;
	char* agpkgvc = 0;
	char* start = 0;
	char* stop = 0;
	FILE* agfind = popen(strcnct(3, "find ", pthpkgr(output), " -type d -name \"*_*\""), "r");
	agtmp = syscmdread(agfind);
	while(*agtmp != -1)
	{
		fldr = calloc(strlen(agtmp)+2, 1);
		strcpy(fldr, agtmp);
		strcat(fldr, "/");
		agpkgnm = strstore(agpkgnm, strextract(47, 45, agtmp));
		agpkgvn = strstore(agpkgvn, strextract(40, 41, agtmp));
		agpkgvc = strstore(agpkgvc, strextract(45, 40, agtmp));
		if(agpkgnm == 0)
		{
			free(fldr);
			agtmp = syscmdread(agfind);
			continue;
		}
		rgstrclr();
		FILE* agfind2 = popen(strcnct(7, "find ", pthpkgr(output), " -type d -name \"*", agpkgnm, "*\" -name \"*",agpkgvn,"*\""), "r");
		agtmp = syscmdread(agfind2);
		while(*agtmp != -1)
		{
			char* hyphen = strrchr(agtmp, 45);
			char* undrscr = strrchr(agtmp, 95);
			if(undrscr == 0 || undrscr < hyphen)
			{
				FILE* agfindapk = popen(strcnct(3 , "find ", pthpkgr(fldr), " -name \"*.apk\""), "r");
				char* agapk = syscmdread(agfindapk);
				while(*agapk != -1)
				{	
					syscmd(strcnct(4,"mv -n ", pthpkgr(agapk), " -t ", pthpkgr(strcnct(2, agtmp, "/")), "r"));
					logs(strcnct(5 , "\nPackage located at: ", pthpkgr(agapk), "\nmoved to:\n", pthpkgr(strcnct(2, agtmp, "/")), "\n"));
					agapk = syscmdread(agfindapk);
				}
				syscmd(strcnct(2,"rm -r ", pthpkgr(fldr)));
			}
			rgstrclr();
			agtmp = syscmdread(agfind2);	
		}
		rgstrclr();
		agtmp = syscmdread(agfind);
		free(fldr);
	}
}
char* strextract(char beg, char end, char* string)
{
	char* token;
	int se_len;
	char* start = strrchr(string, beg);
	char* stop = strrchr(string, end);
	if(start != 0 && stop != 0)
	{
		se_len = (stop-start-1);
		if(se_len > 0)
		{
			token = calloc(se_len+1, 1);
			memcpy(token, (start+1), se_len);
			rgstr(token);
			return token;
		}
		else return NULL;
	}
	else 
	return NULL;
}
char* strstore(char* dest, char* src)
{
	if(src != 0)
	{
		dest = realloc(dest, strlen(src)+1);
		memcpy(dest, src, strlen(src)+1);
		return dest;
	}
	else return NULL;
}
void logs(char* entry)
{
	printf("%s", entry);
	fputs(entry, logfile);
}
