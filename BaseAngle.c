#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<math.h>

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
	int DownBaseSurf,;	/* �x�[�X�Ɖ����̋��E�� */
	int UpBaseSurf;		/* �x�[�X�Ə㗬�̋��E�� */
	int tr;	/* track number */
	int pl;	/* plate number */
	double X[2], Y[2], Z[2];	/* �ǂݍ��ݍ��W */
	double dxdz, dydz;	/* X, Y���ʂɑ΂���tan */
	double Xdeg, Ydeg;	/* �p�xdeg */
	double Xrad, Yrad;	/* �p�xrad */
	double lay;	/* layer number */
	char fname[100][256];	/* File�� */
	char linebuffer[2048];	/* �ǂݎ��s */
	struct LOADPATH Load[100];	/* �ǂݎ��File�̃t���p�X */
	struct dirent *entry;	/* �f�B���N�g������\����dirent */
	DIR *trk;	/* Directory�|�C���^ */
	FILE *tid, *read, *out;	/* File�|�C���^ */
	
	/* �����J�n�錾 */
	printf("*** BaseAngle.exe starts ... ***\n");
	
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
	
	/* �o��File���̌��� */
	if(ans == 1)
		sprintf(linebuffer, "%svtx/BaseAngle.dat", Load[select].Path);
	else
		sprintf(linebuffer, "%sBeam/B%d/BaseAngle.dat", Load[select].Path, pl);
	
	/* �o��File�uBaseAngle.dat�v���J�� */
	if((out = fopen(linebuffer,"w"))==NULL){
		printf("Output File[BaseAngle.dat] not open!!\n");
		exit(1);
	}
	
	/* �o��File����\�� */
	printf("\n >> The result is written in file :\n  %s\n", linebuffer);
	
	/* ���ږ����o�� */
	fprintf(out,"tr# pl#\tdxdz\tXrad\tXdeg\tdydz\tYrad\tYdeg\n");
	
	/* s�����܂ŃX�g���[�����ړ� */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		if(linebuffer[0] == 's')
			break;
	}
	
	/* �ǂݎ��s�ԍ��̏����� */
	DownBaseSurf = 0;
	UpBaseSurf = 0;
	
	/* ��Ճf�[�^�̓ǂݍ��� */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		
		/* #�����̂Ƃ��͎���2�s��� */
		if(linebuffer[0] == '#'){
			fgets(linebuffer,sizeof(linebuffer),read);
			continue;
		}
		
		/* �e��ԍ����擾 */
		sscanf(linebuffer,"%d %d %lf %*lf",&tr,&pl,&lay);
		
		/* �x�[�X�Ɖ����̋��E */
		if((int (lay*10.)) == 20){
			
			/* ���W���擾 */
			sscanf(linebuffer,"%*d %*d %*lf %lf %lf %lf",&X[0],&Y[0],&Z[0]);
			
			/* ���E�ʔF�������J�E���g */
			DownBaseSurf++;
		}
		else if((int (lay*10.)) == 10){
			
			/* ���W���擾 */
			sscanf(linebuffer,"%*d %*d %*lf %lf %lf %lf",&X[1],&Y[1],&Z[1]);
			
			/* ���E�ʔF�������J�E���g */
			UpBaseSurf++;
		}
		
		/* �J�E���g������� */
		if(DownBaseSurf > 0 && UpBaseSurf > 0){
			
			/* �p�x(tan)�����߂� */
			dxdz = (X[1]-X[0])/(Z[1]-Z[0]);
			dydz = (Y[1]-Y[0])/(Z[1]-Z[0]);
			
			/* radian�ɕϊ� */
			Xrad = atan(dxdz);
			Yrad = atan(dydz);
			
			/* degree�ɕϊ� */
			Xdeg= 180.*Xrad/(acos(-1));
			Ydeg= 180.*Yrad/(acos(-1));
			
			/* �t�@�C���ɏo�� */
			fprintf(out,"%.0d %.0d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",tr,pl,dxdz,Xrad,Xdeg,dydz,Yrad,Ydeg);
			
			/* �J�E���g�������� */
			DownBaseSurf = 0;
			UpBaseSurf = 0;
		}
		
	}	/* while [��Ճf�[�^�̓ǂݍ���] end */
	
	/* File����� */
	fclose(read);
	fclose(out);
	
	/* �����I���������� */
	printf("\n*** End BaseAngle.exe ... ***\n");
	
	/* main() �߂�l 0 (����) */
	return 0;
	
}

