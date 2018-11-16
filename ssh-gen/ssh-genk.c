#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pwd.h>

/*
* 	Program auto generate ssh key and add to config file
*	#gcc ssh-genk.c -o ssh-genk
*/

#define ckey 1024
#define config_file  ".ssh/config"
#define max_path 	128

char path_config[max_path];
char path_folder[max_path];
char identity_file[max_path];

struct keyInfoUser
{
	char host[60];
	char hostname[60];
	char port[7];
	char user[20];
	char passphrase[32];
	char email[50];
};

int check_key_exist(char const *str)
{
	FILE *file;
	struct keyInfoUser tmp;
	
	int step, numkey;
	char c;
	char identity_file_tmp[max_path];

	step = numkey = 0;
	file = fopen(path_config, "r");
	rewind(file);
	while((c = fgetc(file)) != EOF)
	{
		if(c == '#')
		{
			numkey++;
		}
	}

	rewind(file);
	// read data and check exist key
	while(step++ < numkey)
	{
		fscanf(file, "\n# add info %s", tmp.hostname);
		fscanf(file, "\nHost\t%s\n", tmp.host);
		fscanf(file, "\tHostname\t%s\n", tmp.hostname);
		fscanf(file, "\tPort\t\t%s\n", tmp.port);
		fscanf(file, "\tUser\t\t%s\n", tmp.user);
		fscanf(file, "\tIdentityFile\t%s\n", identity_file_tmp);

		if (strcmp(str, tmp.hostname) == 0)
		{
			fclose(file);
			return 1;
		}
	}
	fclose(file);
	return 0;
}

int check_file_exist(char const *path)
{
    struct stat buff;
    int i = stat(path, &buff);
    if(i == 0){
    	return 1;
    }
    else
    {
    	return 0;
    }
}

int check_folder_exits(char const *path)
{
	struct stat buff;
	stat(path, &buff);

	if(S_ISDIR(buff.st_mode))
	{
		return 1;
	}else
	{
		return 0;
	}
}

void input_info(struct keyInfoUser *member)
{
	int p;

	// add and check hostname
	while(1){
		printf("Hostname    [domain/IP] : ");
		fgets(member->hostname, 60, stdin);
		if(strcmp(member->hostname, "\n") == 0)
		{
			printf("Error: hostname cannot null\n");
		}else
		{
			strtok(member->hostname, "\n");
			if (!check_key_exist(member->hostname))
			{
				break;
			}
			else
			{
				printf("Key exits\n");
			}
		}
	}

	// copy hostname to host
	strcpy(member->host, member->hostname);

	// add and check port
	while(1)
	{
		printf("Port        [22]         : ");
		fgets(member->port, 7, stdin);
		if(strcmp(member->port, "\n") == 0)
		{
			strcpy(member->port, "22");
			break;
		}else
		{
			strtok(member->port, "\n");
			p = atoi(member->port);
			if(p > 65535 || p < 1024)
			{
				printf("Error: 1024 < Port <65535\n");
			}else
			{
				break;
			}
		}
	}
	// add and check username
	printf("User        [root]            : ");
	fgets(member->user, 20, stdin);
	if(strcmp(member->user, "\n") == 0)
	{
		strcpy(member->user, "root");
		
	}else
	{
		strtok(member->user, "\n");
	}

	// add and check passphrase
	printf("Passphrase  [null]       : ");
	fgets(member->passphrase, 32, stdin);
	if(strcmp(member->passphrase, "\n") == 0)
	{
		strcpy(member->passphrase, "");
		
	}else
	{
		strtok(member->passphrase, "\n");
	}

	// add and check email info
	printf("Email       [default null]    : ");
	fgets(member->email, 50, stdin);
	if(strcmp(member->email, "\n") == 0)
	{
		strcpy(member->email, "");
	}
	else
	{
		strtok(member->email, "\n");
	}
}

int main(int argc, char const *argv[])
{
	printf("===================================\n");
	printf("====== auto generate ssh key ======\n");
	printf("===================================\n");

	char user_login[20];
	char ssh_cmd[max_path] = "";
	FILE *file;

	uid_t uid = geteuid();
  	struct passwd *pw = getpwuid(uid);
  	strcpy(user_login, pw->pw_name);

	struct keyInfoUser member;

	// check exits key file
	sprintf(path_config, "/home/%s/%s", user_login, config_file);
	if(check_file_exist(path_config))
	{
		file = fopen(path_config, "a+");
	}
	else
	{
		printf("Key file config not exits\n");
		printf("Create key file config...\n");
		file = fopen(path_config, "w");
	}

	// input data
	input_info(&member);

	sprintf(identity_file, "/home/%s/.ssh/private_keys/%s", user_login, member.hostname);
	
	// write data to config
	printf("Start writing the configuration file to file config...\n");
	fprintf(file, "\n# add info %s", member.hostname);
	fprintf(file, "\nHost\t%s\n", member.host);
	fprintf(file, "\tHostname\t%s\n", member.hostname);
	fprintf(file, "\tPort\t\t%s\n", member.port);
	fprintf(file, "\tUser\t\t%s\n", member.user);
	fprintf(file, "\tIdentityFile\t%s\n", identity_file);
	fclose(file);

	// check exits folder key
	sprintf(path_folder, "/home/%s/.ssh/private_keys", user_login);

	if(!check_folder_exits(path_folder))
	{
		printf("Key folder config not exits\n");
		printf("Create folder...\n");
		mkdir(path_folder, 0700);
	}

	sprintf(ssh_cmd, "ssh-keygen -t rsa -b %d -P \"%s\" -C \"%s\" -f \"%s/%s\"", 
						ckey, member.passphrase, member.email, path_folder, member.hostname);
	system(ssh_cmd);
	return 0;
}
