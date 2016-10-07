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

/* �\����PLATENUM : pl#�i�[ */
struct PLATENUM{
	int curr;	/* ���݂�pl# */
	int prev;	/* ��O��pl# */
};

/* �\����COODINATE : ���W */
struct COODINATE{
	double X;
	double Y;
	double Z;
};

/* �o�͍s�̃t�H�[�}�b�g���w�� */
void output(FILE *fp, int tr, int pl, double pos, double posx, double posy, double posz){
	char Value[32];
	sprintf(Value,"%02.5f",posx);
	if(strcmp(Value,"-0.00000")==0)
		posx = 0.;
	
	sprintf(Value,"%02.5f",posy);
	if(strcmp(Value,"-0.00000")==0)
		posy = 0.;
	
	sprintf(Value,"%02.5f",posz);
	if(strcmp(Value,"-0.00000")==0)
		posz = 0.;
	
	if(posx >= 0. && posy >=0. && posz >= 0.)
		fprintf(fp,"%d %2.d %1.1f   %02.5f   %02.5f   %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx < 0. && posy >=0. && posz >= 0.) 
		fprintf(fp,"%d %2.d %1.1f  %02.5f   %02.5f   %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx < 0. && posy <0. && posz >= 0.) 
		fprintf(fp,"%d %2.d %1.1f  %02.5f  %02.5f   %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx < 0. && posy <0. && posz < 0.) 
		fprintf(fp,"%d %2.d %1.1f  %02.5f  %02.5f  %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx >= 0. && posy <0. && posz >= 0.) 
		fprintf(fp,"%d %2.d %1.1f   %02.5f  %02.5f   %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx >= 0. && posy >=0. && posz < 0.) 
		fprintf(fp,"%d %2.d %1.1f   %02.5f   %02.5f  %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx >= 0. && posy <0. && posz < 0.) 
		fprintf(fp,"%d %2.d %1.1f   %02.5f  %02.5f  %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx < 0. && posy >=0. && posz < 0.) 
		fprintf(fp,"%d %2.d %1.1f  %02.5f   %02.5f  %.5f\n",tr,pl,pos,posx,posy,posz);
}

/* �֐���`main()�֐��ȍ~�ɋL�q */
int ProcessFrame(char*, char*);
int TrackConnection(FILE*, FILE*, FILE*, char*);

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
	
	/* �����J�n�錾 */
	printf("*** reducedVTX.exe starts ... ***\n");
	
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
	result = ProcessFrame(fname[selfile], linebuffer);
	if(result == 1){
		printf("[Error] :  Invalid File Format in '%s' !!\nexit soon ...\n", fname[selfile]);
		exit(0);
	}
	else if(result == -1)
		exit(1);
	
	/* �����I���錾 */
	printf("\n*** reducedVTX.exe End ... ***\n");
	
	/* main() �߂�l0�i����I���j */
	return 0;
	
}

int ProcessFrame(char *fname, char *path){
	int pos;	/* �����ʒu */
	int loop;	/* ���[�v�J�E���^ */
	int TNum;	/* ������Ղ̑���� */
	char linebuffer[2048];	/* �ǂݎ��s�i�[ */
	char string[2048];	/* ������i�[ */
	char cp[4];	/* �P�����i�[ */
	FILE *read, *out, *log;	/* File�|�C���^ */
	
	/* �����ʒu�A������������� */
	pos = 0;
	strcpy(string, "");
	
	/* File����_d��O�܂ł̕�������擾 */
	while(fname[pos] != '_'){
		sprintf(cp, "%c", fname[pos]);
		strcat(string, cp);
		pos++;
	}
	
	/* �t���p�X��File�����` */
	sprintf(linebuffer,"%s%s_d.vtx", path, string);
	
	/* �ǂݍ���File���J�� */
	printf("\n... Focus On \"%s\" in process\n", linebuffer);
	if ((read = fopen(linebuffer, "r")) == NULL) {
		printf("input_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* ����񐔃J�E���g��0�ɂ��� */
	TNum = 0;
	
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
			
			/* ����񐔂����Z */
			TNum++;
			
		}
		
	}
		
	/* �R�łȂ���΃G���[ */
	if(loop != 3){
		fclose(read);
		return 1;
	}
	
	/* File�X�g���[����߂� */
	fseek(read, 0, SEEK_SET);
	
	/* �o��File�������� */
	sprintf(linebuffer,"%s%s_r.vtx", path, string);
	
	/* �o��File���J�� */
	printf("... Focus On \"%s\" in process\n", linebuffer);
	if ((out = fopen(linebuffer, "w")) == NULL) {
		printf("output_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* ���OFile�������� */
	sprintf(linebuffer,"%sTrConnect.log", path);
	
	/* ���OFile���J�� */
	printf("... Focus On \"%s\" in process\n\n", linebuffer);
	if ((log = fopen(linebuffer, "w")) == NULL) {
		printf("Log_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* ���OFile�ɓǂݎ��File�����L�q */
		fprintf(log, "FILE = \"%s.vtx\"\n\n", string);

	/* �s�̓ǂݎ��J�n */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		
		/* Scan��񕔕��ia�`s�j���o�� */
		fprintf(out,"%s",linebuffer);
		
		/* s�����ɓ��B������A���[�v�I�� */
		if(linebuffer[0] == 's')
			break;
	}
	
	/* ����񐔕������� */
	for(loop=0;loop<TNum;loop++){
		
		/* ���O�ɏ����Ώۂ̔�Տ����L�q */
		fprintf(log, "TR# : %d --\n", loop+1);
		fprintf(log, "pl#,\tZ (mm),\tdX(um),tdY(um)\tn\n");
		
		/* ��ՂP�{���Ɏ���ړ��O���ڑ����Ă��� */
		if(TrackConnection(read, out, log, path) == -1)
			return -1;
		
	}
	
	/* File�|�C���^����� */
	fclose(read);
	fclose(out);
	fclose(log);
	
	/* ProcessFrame() �߂�l0�i����I���j */
	return(0);
	
}

int TrackConnection(FILE *read, FILE *out, FILE *log, char *path){
	int line;	/* �ǂݎ��s�� */
	int move;	/* ����ړ��� */
	int tr;	/* tr# */
	double Position[10000];	/* lay# */
	struct PLATENUM pl;	/* pl# */
	struct COODINATE Track[10000];	/* �ǂݎ����W */
	struct COODINATE Med;	/* ����ړ��ɂ�萶�������W�� */
	char linebuffer[2048];	/* �ǂݎ��s�i�[ */
	
	/* �s���������� */
	line = 0;
	
	/* ����ړ����������� */
	move = 0;
	
	/* ���s���� */
	fprintf(out,"\n");
	
	/* ���߂̍��W���擾 */
	fgets(linebuffer,sizeof(linebuffer),read);
	fgets(linebuffer,sizeof(linebuffer),read);
	sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr,&pl.curr,&Position[line],&Track[line].X,&Track[line].Y,&Track[line].Z);
	
	/* �������W�̏o�� */
	fprintf(out,"%s",linebuffer);
	
	/* ��O��pl#�ɓǂݎ����pl#��ۑ� */
	pl.prev = pl.curr;
	
	/* �s�����Z */
	line++;
	
	/* ����ړ��ɂ�鍷�������� */
	Med.X = 0;
	Med.Y = 0;
	
	/* �ȍ~�̍s��ǂ�ł��� */
	while(1){
		
		/* ���W���擾 */
		fgets(linebuffer,sizeof(linebuffer),read);
		sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr,&pl.curr,&Position[line],&Track[line].X,&Track[line].Y,&Track[line].Z);
		
		/* �ŏI�s(# approximate range ...�̕���)�ł���΁A�ŏI�s���o�͂��ďI�� */
		if(linebuffer[0] == '#'){
			fprintf(out,"%s",linebuffer);
			break;
		}
		
		/* ����ړ��̕�����F������ */
		/* ���[���F�O��Z���W�Ƃ̊Ԋu���}0.05um�͈͓̔��Ȃ�΁A����ړ������Ƃ݂Ȃ� */
		/* �@�@�@�@�����ɂ�������ړ�������Z���W����0.05um�ȏ�ɂȂ��Ă����ꍇ�́A*/
		/* �@�@�@�@���̃v���O���������s����O�ɁA����0.00um�ɂ��Ă��� */
		if(Track[line].Z > Track[line-1].Z-0.0005 && Track[line].Z < Track[line-1].Z+0.0005){
			
			/* ���W�����擾���� */
			Med.X = Med.X + Track[line].X - Track[line-1].X;
			Med.Y = Med.Y + Track[line].Y - Track[line-1].Y;
			
			/* ����ړ��������Z */
			move++;
			
			/* �ړ��ɂ�萶�������W����log�ɏ������� */
			fprintf(log, "%d\t%.4f\t%.4f\t%.4f\t%d\n", pl.curr, Track[line].Z, (Track[line].X-Track[line-1].X)*1000 ,(Track[line].Y-Track[line-1].Y)*1000 ,move);
			
			/* ���̍s�� */
			continue;
		}
		
		/* ����plate���ł���΍��W���o�͂��Ă��� */
		if(pl.curr == pl.prev){
			output(out, tr, pl.curr, Position[line], Track[line].X-Med.X, Track[line].Y-Med.Y, Track[line].Z);
			
			/* �s�����Z */
			line++;
			
		}
		
		/* ����plate�ֈڂ�Ƃ��� */
		else{
			
			/* pl#��ۑ� */
			pl.prev = pl.curr;
			
			/* lay#��ۑ� */
			Position[0] = Position[line];
			
			/* �������W�̈����p�� */
			Track[0].X = Track[line].X;
			Track[0].Y = Track[line].Y;
			Track[0].Z = Track[line].Z;
			
			/* ���������W�����N���A���Ă��� */
			Med.X = 0;
			Med.Y = 0;
			
			/* ����plate�̏����ʒu���o�� */
			output(out, tr, pl.curr, Position[line], Track[line].X-Med.X, Track[line].Y-Med.Y, Track[line].Z);
			
			/* �s���������� */
			line = 1;
		}
		
	}	/* while [�ȍ~�̍s��ǂ�ł���] end */
	
	/* TrackConnection() �߂�l0�i����I���j */
	return(0);
}
