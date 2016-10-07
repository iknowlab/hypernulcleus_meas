#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

/* �\����LOADPATH : �t���p�X�� */
struct LOADPATH{
	char pName[64];	/* ��Ֆ� */
	char Path[1024];	/* �t���p�X */
};

int main(){
	int ans;	/* ���[�U�ԓ� */
	int loop;	/* ���[�v�J�E���^ */
	int count;	/* ���ڐ� */
	int select;	/* ���[�U�[�I�� */
	int selfile;	/* ���[�U�[�w��File */
	int fnum;	/* File�ԍ� */
	int div;	/* File�̍ő啪���� */
	int pl;	/* plate number */
	int tr;	/* track number */
	char fname[100][256];	/* File�� */
	char linebuffer[2048];	/* �ǂݎ��s */
	struct LOADPATH Load[100];	/* �ǂݎ��File�̃t���p�X */
	struct dirent *entry;	/* �f�B���N�g������\����dirent */
	DIR *trk;	/* Directory�|�C���^ */
	FILE *tid, *read, *out;	/* File�|�C���^ */
	
	/* �����J�n�錾 */
	printf("*** splitVTX.exe starts ... ***\n");
	
	/* ���ʑΏۂ̔�Ղ�Beam track��I�� */
	printf("\nWhich base angle do you want to calculate :\n");
	printf("\t1) Track,\t2) Beam track\n");
	printf("Select by number :  ");
	scanf("%d", &ans);
	
	/* 1�A�܂���2�ȊO�̐������͂��ꂽ�ꍇ */
	if(ans > 2 && ans <= 0){
		printf("Please input by 1 or 2, only!!\n");
		exit(1);
	}
	
	/* �������[�v */
	while(1){
		
		/* ��Ֆ��ɒ�`�����p�XFile���J�� */
		if((tid = fopen("ID.txt", "r")) == NULL){
			printf("Load File[ID.txt] not open!!\n\n");
			exit(1);
		}
		
		/* ������ */
		count = 0;
		
		/* ������������Ս��ڂ����� */
		while(fgets(linebuffer, sizeof(linebuffer), tid) != NULL){
			
			/* ���[���F100�ȏ�̓G���[�Ƃ��� */
			if(count > 100){
				printf("[Error] :  Too many Items to read!! -> Exit soon ...\n");
				exit(1);
			}
			
			/* ��Ֆ��A�t���p�X���擾 */
			sscanf(linebuffer, "%s %s", Load[count].pName, Load[count].Path);
			
			/* ���ڐ����Z */
			count++;
		}
		
		/* File����� */
		fclose(tid);
		
		/* �������瓾��ꂽ��Ֆ���\�� */
		printf("\nFound These Items :\n");
		for(loop=0; loop<count; loop++){
			if((loop % 2) != 0)
				printf("\t\t%d) %s\n", loop+1, Load[loop].pName);
			else
				printf("\t%d) %s", loop+1, Load[loop].pName);
		}
		
		/* ���s���� */
		if((loop % 2) != 0)
			printf("\n");
		
		/* ���[�U�[�I�� */
		/* �R�����g�F���[�U�[�ɏ��������Ղ�I�������� */
		printf("[Select a number to carry on] :  ");
		scanf("%d", &select);
		
		/* �\�������ԍ��Ɣz��ԍ������킹�� */
		select--;
		
		/* ���͂��ꂽ�l�̃`�F�b�N */
		if(select >= count || select < 0){
			printf("[Error] :  Invalid number was selected\n\nRestart soon...\n\n");
			
			/* �ĕ\�� */
			continue;
		}
		
		/* �P�x�����������ă��[�v�I�� */
		break;
		
	} /* while [�������[�v] end */
	
	/* Beam track��I�������ꍇ�Apl#����� */
	if(ans == 2){
		printf("Input the plate number :  ");
		scanf("%d", &pl);
	}
	
	/* �������[�v */
	while(1){
		
		/* �I��������Ղ̂���f�B���N�g���̃p�X�����` */
		if(ans == 1)
			sprintf(linebuffer, "%svtx/", Load[select].Path);
		else
			sprintf(linebuffer, "%sBeam/B%d/", Load[select].Path, pl);
		
		/* �p�X����directory���J�� */
		if((trk = opendir(linebuffer)) == NULL){
			printf("\n[Error] :  Invalid PATH NAME!!\n");
			
			/* �J���Ȃ������Ȃ�G���[�I�� */
			exit(1);
		}
		
		/* ���ڐ�Index�������� */
		count = 1;
		
		/* �w�肵��directory����File������*/
		printf("\n [ Found these Files ]  : \n");
		
		/* �\��directory������ */
		printf(" [ Directory ... \' %s \' ]\n", linebuffer);
		
		while(1){
			
			/* File�����擾�A���s�����猟���I�� */
			if((entry = readdir(trk))==NULL)
				break;
			
			/* �J�����gdirectory�Ɛedirectory�͖������� */
			if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)
				continue;
			
			/* ������File����ԍ���Y���ĕ\�� */
			printf("\t%d) %s", count, entry->d_name);
			
			/* �X��File�����i�[ */
			strcpy(fname[count-1], entry->d_name);
			
			/* ���ڐ������Z */
			count++;
			
			/* 1000�ȏ�File��������ΏI�� */
			if(count > 1000){
				printf("\nFound Over 100 Files in this directory!!\nexit soon ...\n");
				exit(0);
			}
			
			/* File�����擾�A���s�����猟���I�� */
			if((entry = readdir(trk))==NULL){
				printf("\n");
				break;
			}
			
			/* �J�����gdirectory�Ɛedirectory�͖������� */
			if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)
				continue;
			
			/* ������File����ԍ���Y���ĕ\�� */
			printf("\t\t%d) %s\n", count, entry->d_name);
			
			/* �X��File�����i�[ */
			strcpy(fname[count-1], entry->d_name);
			
			/* ���ڐ������Z */
			count++;
			
			/* 1000�ȏ�File��������ΏI�� */
			if(count > 1000){
				printf("Found Over 100 Files in this directory!!\nexit soon ...\n");
				exit(0);
			}
		}
		
		/* ���[�U�[��File�ԍ���I�������� */
		printf("Select number of the track file  :  ");
		scanf("%d", &selfile);
		
		/* �I��ԍ��Ɣz��ԍ������킹�� */
		selfile--;
			
		/* directory����� */
		closedir(trk);
		
		/* ���͂��ꂽ�l�̃`�F�b�N */
		if(selfile >= count || selfile < 0){
			printf("[Error] :  Invalid number was selected\n");
			
			/* ������x�ꗗ�\���A�I�������� */
			continue;
		}
		
		/* �P�x�����������ă��[�v�I�� */
		break;
		
	} /* while [�������[�v] end */
	
	/* �ǂݍ���File�������� */
	if(ans == 1)
		sprintf(linebuffer, "%svtx/%s", Load[select].Path, fname[selfile]);
	else
		sprintf(linebuffer, "%sBeam/B%d/%s", Load[select].Path, pl, fname[selfile]);
	
	/* �ǂݍ���File���J�� */
	if((read = fopen(linebuffer,"r"))==NULL){
		printf("Input_File[%s] not open!!\n", linebuffer);
		exit(1);
	}
	
	/* loop �����ɏ�������Ɏg�p */
	loop = 0;
	
	/* �ǂݍ���File�̌`���`�F�b�N */
	while(fgets(linebuffer, sizeof(linebuffer), read)!=NULL){
		
		/* Scan�̏ڍו��������邩���߂ɒ��ׂ� */
		if(linebuffer[0] == 'a' && loop == 0)
			loop++;
		
		/* ����Scan�̏ڍו����̏I������s�����邩���ׂ� */
		else if(linebuffer[0] == 's' && loop == 1)
			loop++;
		
		/* �Ō��#�ŏI����Ă��邩���ׂ� */
		else if(strncmp(linebuffer, "# approximate range ", 20) == 0 && loop == 2){
			if(fgets(linebuffer,sizeof(linebuffer), read) == NULL)
					loop++;
			
		}
		
	}
	
	/* �R�łȂ���Ό`���G���[ */
	if(loop != 3){
		fclose(read);
		
		printf("Error! : Input FIle Format Invalid\n");
		exit(0);
	}
	
	/* File�X�g���[����߂� */
	fseek(read, 0, SEEK_SET);
	
	/* Track���̋L�q����(a�`s�����܂�)��ǂݔ�΂� */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		if(linebuffer[0] == 's')
			break;
	}
	
	/* �ő啪������������ */
	div = 0;
	
	/* ����񐔂𒲂ׂ� */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		
		/* �ŏI�s(# approximate range ...�̍s) �𐔂��� */
		if(linebuffer[0] == '#')
			div++;
	}
	
	/* ����񐔂ɑ΂��镪���̊J�n */
	for(fnum=1;fnum<=div;fnum++){
		
		/* �o��File�������� */
		if(ans == 1)
			sprintf(linebuffer, "%svtx/%s.%.0d", Load[select].Path, fname[selfile], fnum);
		else
			sprintf(linebuffer, "%sBeam/B%d/%s.%.0d", Load[select].Path, pl, fname[selfile], fnum);
		
		/* �o��File���J�� */
		if((out = fopen(linebuffer,"w"))==NULL){
			printf("Output File[%s] not open!!\n",linebuffer);
			exit(1);
		}
		
		/* �ǂݍ���File�̃X�g���[���������� */
		fseek(read, 0, SEEK_SET);
		
		/* Track���̋L�q����(a�`s�����܂�)���������� */
		while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
			fprintf(out,"%s",linebuffer);
			
			if(linebuffer[0] == 's')
				break;
		}
		
		/* ���s�������o�� */
		fgets(linebuffer,sizeof(linebuffer),read);
		fprintf(out,"%s",linebuffer);
		
		/* ���W�L�q�������������� */
		while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
			
			/* tr#���擾 */
			sscanf(linebuffer,"%d %*d",&tr);
			
			/* ���݂̏����Ώ�tr#�ƈ�v����s�ł���� */
			if(tr == fnum){
				
				/* �s���o�� */
				fprintf(out,"%s",linebuffer);
				
				/* �s�̐擪������#�Ȃ�Ώ������ݏI�� */
				if(linebuffer[0] == '#')
					break;
			}
		}
		
		/* �������̏o��File����� */
		fclose(out);
		
	}	/* for [����񐔂ɑ΂��镪���̊J�n] end */
	
	/* �ǂݍ���File����� */
	fclose(read);
	
	/* �����I���������� */
	printf("\n*** End splitVTX.exe ... ***\n");
	
	/* main() �߂�l 0 (����) */
	return 0;
	
}
