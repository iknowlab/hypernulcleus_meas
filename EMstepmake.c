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

/* �\����PL : pl#�̔F�� */
struct PL{
	int curr;	/* ���݂�pl# */
	int prev;	/* �P��O�s��pl# */
};

/* �\����COODINATE : ���W */
struct COODINATE{
	double X;
	double Y;
	double Z;
	double SZ;	/* �L�k���␳���Z���W */
};

/* �\����ShurinkageFactor : �L�k�� */
struct ShurinkageFactor{
	double up;
	double down;
	double Base;	/* �x�[�X�̌��� */
};

/* �֐���`main()�֐��ȍ~�ɋL�q */
int ProcessFrame(char*, char*, char*);
int StepMakingProcess(char*, FILE*, FILE*, char*, double*, double);

int main()
{
	int loop;	/* ���[�v�J�E���^ */
	int count;	/* ���ڐ� */
	int select;	/* ���[�U�[�I�� */
	int selfile;	/* ���[�U�[�w��File */
	int result;	/* �֐��������� */
	char linebuffer[2048];	/* �s�̊i�[ */
	char fname[100][256];	/* File�� */
	struct LOADPATH Load[100];	/* �ǂݎ��File�̃t���p�X */
	struct dirent *entry;	/* �f�B���N�g������\����dirent */
	DIR *beam;	/* Directory�|�C���^ */
	FILE *tid;	/* File�|�C���^ */
	
	/* �ȑO�̒��ԃt�@�C����S�č폜���� */
	for(loop=0; loop<5; loop++){
		switch(loop){
			case 0:	strcpy(fname[1], "dataTR.stp");
				break;
			case 1:	strcpy(fname[1], "dataTR.stpr");
				break;
			case 2:	strcpy(fname[1], "dataTR.stprr");
				break;
			case 3:	strcpy(fname[1], "dataTR.stprrn");
				break;
			case 4: strcpy(fname[1], "sample_tr.dlt");
				break;
			default:
				break;
		}
		
		if((tid = fopen(fname[1],"r"))!=NULL){
			fclose(tid);
			
			printf("... Deleting file \"%s\".\n", fname[1]);
			sprintf(linebuffer, "rm -f %s\n", fname[1]);
			system(linebuffer);
		}
	}
	
	printf("\n");

	/* �����J�n�錾 */
	printf("*** EMstepmake.exe starts ... ***\n");
	
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
		printf("Found These Items :\n");
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
	
	/* �p�X�����` */
	sprintf(linebuffer, "%svtx/", Load[select].Path);
	
	/* �������[�v...Read File�̑I�� */
	while(1){
		/* �p�X����directory���J�� */
		if((beam = opendir(linebuffer)) == NULL){
			printf("\n[Error] :  Invalid PATH NAME!!\n");
			
			/* �J���Ȃ������Ȃ�I�� */
			exit(1);
		}
		
		/* ���ڐ�Index�������� */
		count = 1;
		
		/* �w�肵��directory����File������*/
		printf("\nFound these Files : \n", loop);
		while(1){
			
			/* File�����擾�A���s�����猟���I�� */
			if((entry = readdir(beam))==NULL)
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
			if((entry = readdir(beam))==NULL){
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
		
		/* directory����� */
		closedir(beam);
		
		/* ���[�U�[��File�ԍ���I�������� */
		printf("Select number of the track file  :  ");
		scanf("%d", &selfile);
		
		/* �I��ԍ��Ɣz��ԍ������킹�� */
		selfile--;
		
		/* ���͂��ꂽ�l�̃`�F�b�N */
		if(selfile >= count || selfile < 0){
			printf("[Error] :  Invalid number was selected\n\nRestart soon...\n\n");
			
			/* �ĕ\�� */
			continue;
		}
		
		/* �P�x�����������ă��[�v�I�� */
		break;
		
	}	/* while [�������[�v...Read File�̑I��] end */
	
	/* ����ړ����ɐ������u����␳���� */
	result = ProcessFrame(fname[selfile], linebuffer, Load[select].pName);
	if(result == 1){
		printf("[Error] :  Invalid File Format in '%s' !!\nexit soon ...\n", fname[selfile]);
		exit(0);
	}
	else if(result == -1)
		exit(1);
	
	/* �����I���錾 */
	printf("\n*** End EMstepmake.exe ... ***\n");
	
	/* main() �߂�l0�i����I���j */
	return 0;
	
}

int ProcessFrame(char *fname, char *path, char *pName){
	int loop;	/* for���[�v�J�E���^ */
	int counter;	/* �������� : �ψ�܂��͕s�ψ�Step�̑I�� */
	int ident;	/* �s�̐擪�������i�[ */
	int setevent;	/* ��������p�ϐ�(plate��) Used like Boolean */
	int TNum;	/* ������Ղ̑���� */
	double Slength;	/* ���[�U�[�w��̋ψ�Step�� */
	double sdrms;	/* �s�ψ�Step�̃p�����[�^�I�� */
	double sdrmsRead;	/* step.dat����ǂݍ��񂾃p�����[�^ */
	double step[1000];	/* step���i�[ */
	double Rfinish;	/* �Î~�_����̏����͈� */
	char pNameIndex[32];
	char linebuffer[2048];	/* �P�s���i�[ */
	FILE *read, *out, *stp;	/* File�|�C���^ */
	
	/* �t���p�X��File�����` */
	sprintf(linebuffer,"%s%s", path, fname);
	
	/* �ǂݎ��File���J�� */
	printf("\n... Focus On \"%s\" in process\n", linebuffer);
	if ((read = fopen(linebuffer, "r")) == NULL) {
		printf("File[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* ����񐔃J�E���g��0�ɂ��� */
	TNum = 0;
	
	/* setevent �����ɏ�������Ɏg�p */
	setevent = 0;
	
	/* �ǂݍ���File�̌`���`�F�b�N */
	while(fgets(linebuffer, sizeof(linebuffer), read)!=NULL){
		ident = linebuffer[0];
		
		/* Scan�̏ڍו��������邩���߂ɒ��ׂ� */
		if(ident == 'a' && setevent == 0)
			setevent++;
		
		/* ����Scan�̏ڍו����̏I������s�����邩���ׂ� */
		else if(ident == 's' && setevent == 1)
			setevent++;
		
		/* �Ō��#�ŏI����Ă��邩���ׂ� */
		else if(strncmp(linebuffer, "# approximate range ", 20) == 0 && setevent == 2){
			if(fgets(linebuffer,sizeof(linebuffer), read) == NULL)
					setevent++;
			
			/* ����񐔂����Z */
			TNum++;
			
		}
		
	}
	
	/* �R�łȂ���Ό`���G���[ */
	if(setevent != 3){
		fclose(read);
		return 1;
	}
	
	/* File�X�g���[����߂� */
	fseek(read, 0, SEEK_SET);
	
	/* �o��File���J�� */
	printf("... Focus On \"dataTR.stp\" in process\n\n");
	if ((out = fopen("dataTR.stp", "w")) == NULL) {
		printf("File[dataTR.stp] not open\n");
		return -1;
	}
	
	/* �ψ�Step���A�܂��͕s�ψ�Step���̑I�� */
	/* ���[���F�s�ψ�Steo���I���̏ꍇ�Astep.dat��p�ӂ��Ă��� */
	printf("\nWhitch do you sellect?\n\t1) Interval step \t 2) Irregular step\n\n");
	printf("[Answer by number(1, or 2) ] : ");
	scanf("%d",&counter);
	
	/* �ψ�Step���I���̏ꍇ */
	if(counter == 1){
		printf("\nInput the step length (um) : ");
		scanf("%lf",&Slength);
		
		for(loop=0; loop<1000; loop++)
			step[loop] = Slength;
		
		printf("\nAnd ");
	}
	
	/* �s�ψ�Step���I���̏ꍇ */
	else if(counter == 2){
		
		/* step.dat�̌Ăяo�� */
		if ((stp = fopen("step.dat", "r")) == NULL) {
			printf("\t[step.dat] not open\n");
			exit(1);
		}
		
		/* �p�����[�^���� */
		printf("\nInput the SD(rms) Value : ");
		scanf("%lf",&sdrms);
		
		/* Step�������[�h */
		loop = 0;
		while(fgets(linebuffer,sizeof(linebuffer),stp)!=NULL){
			    sscanf(linebuffer,"%lf %lf",&sdrmsRead, &step[loop]);
				if(sdrms == sdrmsRead){
			            printf("%lf %lf\n",sdrmsRead, step[loop]);
				        loop++;
			    }
		}
		
		fclose(stp);
		
		printf("\n");
	}
	
	/* 1, 2�ȊO�̐������͂��ꂽ�ꍇ */
	else{
		printf("Invalid parameter inputed !!\nexit soon...\n");
		exit(1);
	}
	
	/* �f�t�H���g=6mm */
	Rfinish = 6000.;
	
	/* �����͈͂��w�肳���� */
	printf("Input the length from the stop point : \n");
	printf("(org. R = %lf )\tZ = ",Rfinish);
	scanf("%lf",&Rfinish);
	
	/* ����s�̍s�܂ŃX�g���[�����ړ������� */
	while(linebuffer[0] != 's')
		fgets(linebuffer, sizeof(linebuffer), read);
	
	/* ���q�����` */
	strcpy(pNameIndex, pName);
	
	/* ����񐔕������� */
	for(loop=0;loop<TNum;loop++){
		
		/* ��ՂP�{���̘c�ݕ␳���� */
		if(StepMakingProcess(pNameIndex, read, out, path, step, Rfinish) == -1)
			return -1;
		
		/* 2��ڈȍ~�͗��q����Index������ */
		sprintf(pNameIndex, "%s%02d", pName, loop+1);
		
	}
	
	/* ���ǂݍ��݃t�@�C������� */
	fclose(read);
	
	/* �o�̓t�@�C������� */
	fclose(out);
	
	/* ProcessFrame() �߂�l0(����I��) */
	return 0;
	
}
int StepMakingProcess(char *pName, FILE *read, FILE *out, char *path, double step[], double Rfinish){
	int loop;	/* for���[�v�J�E���^ */
	int ident;	/* �s�̐擪�������i�[ */
	int line;	/* �s���J�E���^ */
	int surf;	/* 1plate���ɂ�����\�ʐ�(�ő��4�܂�) */
	int setevent;	/* ��������p�ϐ�(plate��) Used like Boolean */
	int offset;	/* ��������p�ϐ�(1track��) Used like Boolean */
	int pos;	/* Step���I��pIndex Number */
	int PLchange;	/* plate�ڑ������̔F�� Used like Boolean */
	double Position[10000];	/* vtx file����lay# */
	double R[10000];	/* plate���ŉ����_����̒��� */
	double Rlength;	/* plate�ŉ����_����̒���(step���Ԋu) */
	double Range;	/* Range : �Î~�_����̋��� */
	char linebuffer[2048];	/* �P�s���i�[ */
	struct PL pl;			/* vtx file����pl# */
	struct COODINATE In[10000];	/* �ǂݍ��ݍ��W */
	struct COODINATE Out;		/* �o�͍��W */
	struct COODINATE em[4];	/* �\�ʂ̍��W */
	struct COODINATE joint;	/* plate�ڑ��p���W */
	struct COODINATE set;	/* �Î~�_��X, Y���W�AZ, SZ�͕s�g�p */
	struct ShurinkageFactor SF;	/*�L�k���iMC�̂��ߊ�{��UP,DOWN=1.0�ABase��0.05mm�j*/
	FILE *sf;	/* File�|�C���^ */
	
	/* ���s�����̎��փX�g���[�����ڂ� */
	fgets(linebuffer, sizeof(linebuffer), read);
	
	/* �����N���A */
	line = 0;
	pos = 0;
	surf = 0;
	setevent = 0;
	offset = 0;
	Rlength = 0.;
	Range = 0.;
	PLchange = 0;
	joint.Z = 0;
	
	/* ����#�̍s�܂Łi�܂�P��Օ��j�������� */
	while(linebuffer[0] != '#'){
		
		/* ���W�̎擾 */
		fgets(linebuffer, sizeof(linebuffer), read);
		sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",
						   &ident, &pl.curr, &Position[line], &In[line].X, &In[line].Y, &In[line].Z);
		
		/* line=0�͐Î~�_�A��O�s��pl#(pl.prev)�Ɍ��i�K��pl#���i�[���Ă��� */
		if(line == 0)
			pl.prev = pl.curr;
		
		/* �ŏI�s�łȂ����ƁA����pl#�������ł���Ԃ͍��W���擾�������� */
		if(linebuffer[0] != '#' && pl.curr == pl.prev){
			line++;
			continue;
		}
		
		/* �R�����g�F���̒i�K��1plate���̍��W������ꂽ */
		/* �@�@�@�@�@�܂�In[line]�ɂ͎�plate�̏��߂̍s���i�[����Ă��邽�� */
		/* �@�@�@�@�@for���[�v�̏I���l��line-1�܂ł͈̔͂ɂ��� */
		
		/* plate�̕\�ʂ�F�����Ă��� */
		for(loop=0; loop<line; loop++){
			
			/* �ŉ����ʂȂ�� */
			if(((int) (Position[loop]*10.)) == 30){
				surf++;
				em[3].Z = In[loop].Z;
			}
			
			/* �������܂�Base�̋��E�ʂȂ�� */
			else if(((int) (Position[loop]*10.)) == 20){
				surf++;
				em[2].Z = In[loop].Z;
				em[2].X = In[loop].X;
				em[2].Y = In[loop].Y;
			}
			
			/* �㗬���܂�Base�̋��E�ʂȂ�� */
			else if(((int) (Position[loop]*10.)) == 10){
				surf++;
				em[1].Z = In[loop].Z;
				em[1].X = In[loop].X;
				em[1].Y = In[loop].Y;
			}
			
			/* �ŏ㗬�ʂȂ�� */
			else if(((int) (Position[loop]*10.)) == 0){
				surf++;
				em[0].Z = In[loop].Z;
			}
		}
		
		/* �L�k���̌v�Z */
		/* track��plate���ђʂ��Ă����ꍇ */
		if(surf >= 4){
			SF.up = 0.5/sqrt((em[0].Z-em[1].Z)*(em[0].Z-em[1].Z));
			SF.down = 0.5/sqrt((em[2].Z-em[3].Z)*(em[2].Z-em[3].Z));
			SF.Base = sqrt((em[1].Z-em[2].Z)*(em[1].Z-em[2].Z));
		}
		
		/* track�̐Î~�_�������ł���ꍇ */
		else if(surf >= 2 && surf < 4){
			SF.up = 0.5/sqrt((em[0].Z-em[1].Z)*(em[0].Z-em[1].Z));
			SF.Base = sqrt((em[1].Z-em[2].Z)*(em[1].Z-em[2].Z));
			
			/* SF���i�[����Ă���File���J�� */
			sprintf(linebuffer,"%s/SF_pl%d.txt",path,pl.prev);
			if ((sf = fopen(linebuffer, "r")) == NULL) {
				printf("input_file[%s] not open\n",linebuffer);
				return -1;
			}
			
			/* ����SF�̂݃��[�h */
			fgets(linebuffer,sizeof(linebuffer),sf);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SF.down);
			
			/* ���� */
			fclose(sf);
		}
		
		/* track�̐Î~�_���㗬�Ȃ�� */
		else{
			
			/* SF���i�[����Ă���File���J�� */
			sprintf(linebuffer,"%s/SF_pl%d.txt",path,pl.prev);
			if ((sf = fopen(linebuffer, "r")) == NULL) {
				printf("input_file[%s] not open\n",linebuffer);
				return -1;
			}
			
			/* ��Gel��SF�A�܂�Base���������[�h */
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SF.up);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SF.down);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SF.Base);
			
			/* ���� */
			fclose(sf);
		}
		
//		/* Base Angle���v�Z���� */
//		angle = atan(sqrt((em[1].X-em[2].X)*(em[1].X-em[2].X) + (em[1].Y-em[2].Y)*(em[1].Y-em[2].Y))
//					/sqrt((em[1].Z-em[2].Z)*(em[1].Z-em[2].Z)));
//		printf("  > [Track angle in pl#%d] : %lf\n", pl.prev, angle*((double) 180.0/acos(-1)));
		
		/* �����N���A */
		In[0].SZ = 0.;
		R[0] = 0.;
		
		/* �R�����g�Fline�Ԗڂ͎�plate�̎n�_�Ȃ̂ŁA0�ɂ��Ă��� */
		In[line].SZ = 0.;
		R[line] = 0.;
		
		/* �L�k����␳���� */
		for(loop=1; loop<line; loop++){
			
			/* �������ܕ��� */
			if(((int) (Position[loop-1]*10.)) == 25 || ((int) (Position[loop-1]*10.)) == 30)
				In[loop].SZ = In[loop-1].SZ + (In[loop].Z - In[loop-1].Z)*SF.down;
			
			/* �x�[�X���� */
			else if(((int) (Position[loop-1]*10.)) == 20)
				In[loop].SZ = In[loop-1].SZ + SF.Base;
			
			/* �㗬���ܕ��� */
			else if(((int) (Position[loop-1]*10.)) == 5 || ((int) (Position[loop-1]*10.)) == 10)
				In[loop].SZ = In[loop-1].SZ + (In[loop].Z - In[loop-1].Z)*SF.up;
			
			/* �ŉ����_����̋������v�Z */
			R[loop] = R[loop-1] + sqrt((In[loop].X-In[loop-1].X)*(In[loop].X-In[loop-1].X)
								+ (In[loop].Y-In[loop-1].Y)*(In[loop].Y-In[loop-1].Y)
								+ (In[loop].SZ-In[loop-1].SZ)*(In[loop].SZ-In[loop-1].SZ));
		}
		
		/* �R�����g�Fsetevent�ɂ��� */
		/* �@�@�@�@�@0�̏ꍇ��plate�����ł̏�����\�� */
		/* �@�@�@�@�@1�̏ꍇ��plate�ڑ�����̏�����\�� */
		/* �@�@�@�@�@2�̏ꍇ�͎w��͈͂𒴂����ꍇ�̏��� */
		
		/* plate�ڑ���͔�Ղ��Ȃ� */
		if(setevent == 1){
			
			/* �Oplate�̔�ՂƂȂ��� */
			for(loop=1; loop<line; loop++){
				In[loop].X = In[loop].X - (In[0].X - joint.X);
				In[loop].Y = In[loop].Y - (In[0].Y - joint.Y);
			}
			
			/* �������W�͑Oplate�̍ŏ㗬�_�ɂ��킹�� */
			In[0].X = joint.X;
			In[0].Y = joint.Y;
			
			/* plate�̓��������Ɉڂ� */
			setevent = 0;
		}
		
		/* plate���������̎菇 */
		for(loop=0; loop<line; loop++){
			
			/* Step���̈ʒu��܂���ߎ��ɂ��擾���� */
			if(Rlength >= R[loop] && Rlength < R[loop+1] && setevent == 0){
				Out.X = In[loop].X + (In[loop+1].X - In[loop].X) * (Rlength - R[loop]) / (R[loop+1] - R[loop]);
				Out.Y = In[loop].Y + (In[loop+1].Y - In[loop].Y) * (Rlength - R[loop]) / (R[loop+1] - R[loop]);
				Out.Z = joint.Z + In[loop].SZ + (In[loop+1].SZ - In[loop].SZ) * (Rlength - R[loop]) / (R[loop+1] - R[loop]);
				
				/* �R�����g�Foffset�ɂ��āE�E�EBoolean�Ƃ��Ċ��p */
				/* �@�@�@�@�@0�͏�����������\�� */
				/* �@�@�@�@�@1�͂���ȍ~�̏������e��\�� */
				
				/* ���������Ȃ�΁A�Î~�_��X, Y���W���擾���� */
				if(offset == 0){
					set.X = Out.X;
					set.Y = Out.Y;
					
					/* ���������̏I�� */
					offset = 1;
				}
				
				/* plate�ڑ���̍��W�ɂ�111��ǋL */
				if(PLchange == 1){
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n", pName, Range, (Out.X-set.X)*1000, (Out.Y-set.Y)*1000, Out.Z*1000);
					PLchange = 0;
				}
				
				/* plate������������000 */
				else
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n", pName, Range, (Out.X-set.X)*1000, (Out.Y-set.Y)*1000, Out.Z*1000);
				
				/* �w��͈͂𒴂����ꍇ�͏����I�� */
				if(Range >= Rfinish){
					setevent = 2;
					break;
				}
				
				/* step����ǉ� */
				Rlength = Rlength + step[pos]/1000;
				Range = Range + step[pos];
				pos++;
				
				/* ����step�����������ȓ��̏ꍇ�͍ċA */
				if(Rlength < R[loop+1])
					loop--;
				
				setevent = 0;
			}
			
			/* plate���̍ő勗���𒴂�����A�����I�� */
			else if(Rlength >= R[line-1] && setevent == 0){
				
				/* plate�̐ڑ��葱�� */
				Rlength = Rlength - R[line-1];
				joint.X = In[line-1].X;
				joint.Y = In[line-1].Y;
				joint.Z = In[line-1].SZ + joint.Z;
				setevent = 1;
				
				break;
			}
		} /* for [plate���������̎菇] end */
		
		/* ����plate���W��ǂݍ��ނ��߂̎葱�� */
		if(pl.curr != pl.prev){
			PLchange = 1;
			
			/* pl#�̍X�V */
			pl.prev = pl.curr;
			
			/* line�Ԗڂ̒l��0�ԖڂɊi�[ */
			Position[0] = Position[line];
			In[0].X = In[line].X;
			In[0].Y = In[line].Y;
			In[0].Z = In[line].Z;
			
			/* 0�Ԗڂ����łɓǂݍ��܂�Ă���̂ŁA1�������l�ɂȂ� */
			line = 1;
			
			/* �\�ʔF�������N���A */
			surf = 0;
			
			continue;
		}
	}	/* while [����#�̍s�܂Łi�܂�P��Օ��j��������] end */

	/* ����������ǉ� */
	fprintf(out,"----------------------------------------\n");
	
	/* �N���A */
	PLchange = 0;
	offset = 0;
	setevent = 0;
	
	/* ProcessFrame() �߂�l0(����I��) */
	return 0;
	
}
