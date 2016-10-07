#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

/* �\����LOADPATH : �t���p�X�� */
struct LOADPATH{
	char pName[64];	/* ��Ֆ� */
	char Path[1024];	/* �t���p�X */
};

/* �\����TRNUM : tr#�i�[ */
struct TRNUM{
	int curr;	/* ���݂�tr# */
	int prev;	/* ��O�̍s��tr# */
	int dist;	/* distortion.vtx��tr# */
};

/* �\����PLATENUM : pl#�i�[ */
struct PLATENUM{
	int curr;	/* ���݂�pl# */
	int prev;	/* ��O��pl# */
	int dist;	/* �c��File�p */
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
int DistortionRevision(FILE*, FILE*, FILE*, char*);

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
	printf("*** distortionVTX.exe starts ... ***\n");
	
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
	sprintf(linebuffer, "%s/vtx/", Load[select].Path);
	
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
	
	
	/* �c��File[distortion.vtx]���Q�Ƃ��A��Ղ���c�݂���菜�� */
	result = ProcessFrame(fname[selfile], linebuffer);
	if(result == 1){
		printf("[Error] :  Invalid File Format in '%s' !!\nexit soon ...\n", fname[selfile]);
		exit(0);
	}
	else if(result == -1)
		exit(1);
	
	/* �����I���錾 */
	printf("*** End distortionVTX.exe ... ***\n");
	
	/* main() �߂�l0�i����I���j */
	return 0;
	
}

int ProcessFrame(char *fname, char *path){
	int loop;	/* ���[�v�J�E���^ */
	int TNum;	/* ������Ղ̑���� */
	char linebuffer[2048];	/* �s�ǂݎ��ϐ� */
	char c[8];	/* tr# */
	char string[2048];	/* ������̊i�[ */
	FILE *read, *out, *dst;	/* File�|�C���^ */
	
	/* �t���p�X��File�����` */
	sprintf(linebuffer,"%s%s", path, fname);
	
	/* �ǂݍ���File���J�� */
	printf("... Focus On \"%s\" in process\n", linebuffer);
	if ((read = fopen(linebuffer, "r")) == NULL) {
		printf("File[%s] not open\n", linebuffer);
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
	if(loop != 3){
		fclose(read);
		return 1;
	}
	
	/* File�X�g���[����߂� */
	fseek(read, 0, SEEK_SET);
	
	/* �o��File�������� */
	sprintf(string, "%c", fname[0]);
	loop = 0;
	strcpy(c, "");
	while(strcmp(string,".") != 0){
		strcat(c,string);
		loop++;
		sprintf(string, "%c", fname[loop]);
	}
	sprintf(linebuffer, "%s_d.vtx", c);
	
	/* �o��File���J�� */
	sprintf(linebuffer,"%sfin_d.vtx", path);
	printf("... Focus On \"%s\" in process\n", linebuffer);
	if ((out = fopen(linebuffer, "w")) == NULL) {
		printf("output_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* distortion.vtx���J�� */
	sprintf(linebuffer,"%sdistortion.vtx", path);
	printf("... Focus On \"%s\" in process\n\n", linebuffer);
	if ((dst = fopen(linebuffer, "r")) == NULL) {
		printf("input_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* Scan���ia�`s�܂Łj��ǂݔ�΂� */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		fprintf(out,"%s",linebuffer);
		
		/* summary�܂ŗ����烋�[�v�I�� */
		if(linebuffer[0] == 's')
			break;
		
	}
	
	/* ����񐔕������� */
	for(loop=0;loop<TNum;loop++){
		
		/* ��ՂP�{���̘c�ݕ␳���� */
		if(DistortionRevision(read, dst, out, path) == -1)
			return -1;
		
	}
	
	/* File����� */
	fclose(read);
	fclose(out);
	fclose(dst);
	
	/* ProcessFrame() �߂�l0�i����I���j */
	return 0;
	
}

int DistortionRevision(FILE *read, FILE *dst, FILE *out, char *path){
	int loop;	/* ���[�v�J�E���^ */
	int Tline;	/* Track File�p�ǂݎ��s�ԍ� */
	int Dline;	/* distortion.vtx�p�ǂݎ��s�ԍ�*/
	int Max;	/* ���[�v�I���l */
	int surf;	/* �\�ʃJ�E���^ */
	double Position[300];	/* lay# */
	double Z[300];	/* SF�␳���Z���W */
	double Dpos[1003];	/* distortion.vtx��lay# */
	double surface[3][4];	/* �\�ʍ��W */
	double SFu, SFd;	/* �L�k��(Shurinkage Factor) */
	struct TRNUM tr;	/* tr# */
	struct PLATENUM pl;	/* pl# */
	struct COODINATE Track[300];	/* Track���W */
	struct COODINATE Distr[1003];	/* distrtion */
	struct COODINATE Out;	/* �c�ݕ␳��̍��W */
	struct COODINATE Med;	/* ���ԍ��W */
	char linebuffer[2048];	/* �s�ǂݎ��ϐ� */
	char string[2048];	/* ������̊i�[ */
	FILE *sf;	/* File�|�C���^ */
	
	/* �s�ԍ��������� */
	Tline = 0;
	
	/* ���s���� */
	fprintf(out,"\n");
	
	/* �������W���擾 */
	fgets(linebuffer,sizeof(linebuffer),read);
	fgets(linebuffer,sizeof(linebuffer),read);
	sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.curr,&pl.curr,&Position[Tline],&Track[Tline].X,&Track[Tline].Y,&Track[Tline].Z);
	
	/* tr#���i�[ */
	tr.prev = tr.curr;
	
	/* pl#���i�[ */
	pl.prev = pl.curr;
	
	/* �s�ԍ��ǉ� */
	Tline++;
	
	/* track�܂��Adistortion���W��ǂݎ�� */
	while(1){
		
		/* track���W�̓ǂݎ��(1plate��) */
		while(linebuffer[0] != '#' && pl.curr == pl.prev){
			fgets(linebuffer,sizeof(linebuffer),read);
			sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.curr,&pl.curr,&Position[Tline],&Track[Tline].X,&Track[Tline].Y,&Track[Tline].Z);
			
			/* �s�ԍ������Z */
			Tline++;
		}
		
		/* �ŏI�s("# approximate range ... "�Ƃ�������)��ۑ� */
		strcpy(string, linebuffer);
			
		/* �ő�s����ۑ�(�ŏI�s�͏���) */
		Max = Tline-1;
		
		/* �\�ʔF���ϐ��̏����� */
		surf = 0;
		
		/* �e�\�ʂ̍��W���擾 */
		for(Tline=0;Tline<Max;Tline++){
			
			/* �ŏ㗬�� */
			if(((int) (Position[Tline]*10.)) == 0){
				surface[0][0] = Track[Tline].X;
				surface[1][0] = Track[Tline].Y;
				surface[2][0] = Track[Tline].Z;
				surf++;
			}
			
			/* �x�[�X�Ə㗬���܂̋��� */
			else if(((int) (Position[Tline]*10.)) == 10){
				surface[0][1] = Track[Tline].X;
				surface[1][1] = Track[Tline].Y;
				surface[2][1] = Track[Tline].Z;
				surf++;
			}
			
			/* �x�[�X�Ɖ������܂̋��� */
			else if(((int) (Position[Tline]*10.)) == 20){
				surface[0][2] = Track[Tline].X;
				surface[1][2] = Track[Tline].Y;
				surface[2][2] = Track[Tline].Z;
				surf++;
			}
			
			/* �ŉ����� */
			else if(((int) (Position[Tline]*10.)) == 30){
				surface[0][3] = Track[Tline].X;
				surface[1][3] = Track[Tline].Y;
				surface[2][3] = Track[Tline].Z;
				surf++;
			}
		}
		
		/* track��plate���ђʂ��Ă����ꍇ */
		if(surf >= 4){
			SFu = 0.5/sqrt((surface[2][0]-surface[2][1])*(surface[2][0]-surface[2][1]));
			SFd = 0.5/sqrt((surface[2][2]-surface[2][3])*(surface[2][2]-surface[2][3]));
		}
		
		/* track�̐Î~�_���㗬�ł���ꍇ */
		else if(surf >= 2 && surf < 4){
			SFu = 0.5/sqrt((surface[2][0]-surface[2][1])*(surface[2][0]-surface[2][1]));
			
			/* SF���i�[����Ă���File���J�� */
			sprintf(linebuffer,"%s/SF_pl%d.txt",path,pl.prev);
			if ((sf = fopen(linebuffer, "r")) == NULL) {
				printf("input_file[%s] not open\n",linebuffer);
				return -1;
			}
			
			/* ����SF�̂݃��[�h */
			fgets(linebuffer,sizeof(linebuffer),sf);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SFd);
			
			/* ���� */
			fclose(sf);
		}
		
		/* track�̐Î~�_�������Ȃ�� */
		else{
			
			/* SF���i�[����Ă���File���J�� */
			sprintf(linebuffer,"%s/SF_pl%d.txt",path,pl.prev);
			if ((sf = fopen(linebuffer, "r")) == NULL) {
				printf("input_file[%s] not open\n",linebuffer);
				return -1;
			}
			
			/* ��Gel��SF�����[�h */
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SFu);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SFd);
			
			/* ���� */
			fclose(sf);
		}
		
		/* SF�␳���Z���W���擾 */
		for(Tline=0;Tline<Max;Tline++){
			if(((int) (Position[Tline]*10.)) >= 0 && ((int) (Position[Tline]*10.)) <= 10)
				Z[Tline] = 1. - (Track[Max-1].Z - Track[Tline].Z)*SFu;
			else if(((int) (Position[Tline]*10.)) >= 20 && ((int) (Position[Tline]*10.)) <= 30)
				Z[Tline] = 0.5 - (surface[2][2] - Track[Tline].Z)*SFd;
		}
		
		/* �c��File�̓ǂݎ��s�ԍ��������� */
		Dline = 0;
		
		/* distortion.vtx�̍��W��ǂݎ�� */
		while(fgets(linebuffer,sizeof(linebuffer),dst)!=NULL){
			sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.dist,&pl.dist,&Dpos[Dline],&Distr[Dline].X,&Distr[Dline].Y,&Distr[Dline].Z);
			
			/* ����pl#�̍��W�̂ݎ擾 */
			if(pl.dist == pl.prev)
				Dline++;
		}
		
		/* �X�g���[����File�̓��ɖ߂� */
		fseek(dst, 0, SEEK_SET);
		
		/* (��1) �ŉ����ʂ�Z���W����0.05um���� */
		Distr[0].Z = Distr[0].Z - 0.00005;
		
		/* (��2) �ŏ㗬�ʂ�Z���W��1um���Z */
		Distr[Dline].Z = Distr[Dline-1].Z + 0.001;
		
		/* (��3) �ŏ㗬�ʂ̍��W��1��̔z��ɃR�s�[���Ă��� */
		Distr[Dline].X = Distr[Dline-1].X;
		Distr[Dline].Y = Distr[Dline-1].Y;
		
		/* �R�����g�F(��)�̏������R */
		/* �@�@�@�@�@�ȉ��̃��[�v�����ɂ����āAif()�̏����ɑΉ������邽�߁B */
		/* �@�@�@�@�@1) ��{�I�ɕ��������_���܂ޏ������̓i���Z���X�B���x�������B */
		/* �@�@�@�@�@2) loop=Max(���[�v�I���l)�̎��Aloop+1�͎���plate�̍ŉ����_(�܂�Z=0.)�B
		/* �@�@�@�@�@   ���̏�Ԃł�if()�̏������N���A�ł����A�ŏ㗬�_�̂ݖ������ɂȂ��Ă��܂�����1um�����ǉ������B */
		/* �@�@�@�@�@3) loop=Max(���[�v�I���l)�̎��Aloop+1�͎���plate�̍ŉ����_(�܂�X,Y�Ƃ���0)�B */
		/* �@�@�@�@�@   ���̂܂܂���Med.X�����Med.Y�����������߂��Ȃ����߁B���̕��@�͂��܂蓾��ł͂Ȃ��B */
		
		/* track���̃��[�v���� */
		for(Tline=0;Tline<Max;Tline++){
			
			/* �c��File���̃��[�v���� */
			for(loop=0;loop<Dline;loop++){
				
				/* track��Z���W�ɑΉ�����c�݂�T�� */
				if(Z[Tline] >= Distr[loop].Z && Z[Tline] < Distr[loop+1].Z){
					
					/* track�Ɠ���Z���W�ł̘c�݂����߂�(�܂���ߎ�) */
					Med.X = Distr[loop].X + (Distr[loop+1].X-Distr[loop].X)*(Z[Tline]-Distr[loop].Z)/(Distr[loop+1].Z-Distr[loop].Z);
					Med.Y = Distr[loop].Y + (Distr[loop+1].Y-Distr[loop].Y)*(Z[Tline]-Distr[loop].Z)/(Distr[loop+1].Z-Distr[loop].Z);
					
					/* �c�݂�␳ */
					Out.X = Track[Tline].X - Med.X;
					Out.Y = Track[Tline].Y - Med.Y;
					
					/* �o�� */
					output(out, tr.prev, pl.prev, Position[Tline], Out.X, Out.Y, Track[Tline].Z);
				}
			}
		}
		
		/* �ŏI�s�Ȃ�΃��[�v�I�� */
		if(string[0] == '#')
			break;
		
		/* track���̎�plate�ŉ����_���A�ŏ��̔z��Ɋi�[ */
		Track[0].X = Track[Max].X;
		Track[0].Y = Track[Max].Y;
		Track[0].Z = Track[Max].Z;
		
		/* lay#�̈����p�� */
		Position[0] = Position[Max];
		
		/* �s�ԍ��̏����� */
		Tline = 1;
		
		/* pl#�̈����p�� */
		pl.prev = pl.curr;
		
		/* tr#�̈����p�� */
		tr.prev = tr.curr;
		
	}	/* while [track�܂��Adistortion���W��ǂݎ��] end */
	
	/* vtx file�̍ŏI�s���o�� */
	fprintf(out,"%s",string);
	
	/* DistortionRevision �߂�l 0(����I��) */
	return 0;
	
}	/* �֐��I�� */
