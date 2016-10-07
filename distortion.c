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

/* �\����TRNUM : tr#�i�[ */
struct TRNUM{
	int curr;	/* ���݂�tr# */
	int prev;	/* ��O�̍s��tr# */
	int dist;	/* distortion.vtx��tr# */
};

/* �s�X�g���[����1�s�߂�(������VTX FILE�̂ݓK�p��) */
void BackStreem(FILE *fp){
	char linebuffer[2048];
	fseek(fp, -90, SEEK_CUR);
	fgets(linebuffer,sizeof(linebuffer),fp);
}

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

int main(){
	int loop;	/* ���[�v�J�E���^ */
	int count;	/* ���ڐ� */
	int select;	/* ���[�U�[�I�� */
	int selfile;	/* ���[�U�[�w��File */
	int flag;	/* �����ے��̕��� used like Boolean */
	int result;	/* �֐��������� */
	char linebuffer[2048];	/* �s�̊i�[ */
	char fname[100][256];	/* File�� */
	char command[1024];	/* �O���R�}���h������(Unix�̂ݎg�p�\) */
	struct LOADPATH Load[100];	/* �ǂݎ��File�̃t���p�X */
	struct dirent *entry;	/* �f�B���N�g������\����dirent */
	DIR *beam;	/* Directory�|�C���^ */
	FILE *tid;	/* File�|�C���^ */
		
	/* �����J�n�錾 */
	printf("*** distortion.exe starts ... ***\n");
	
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
	
	/* �����ߒ��ϐ��������� */
	flag = 1;
	
	/* plate�������[�v */
	for(loop=12; loop>1; loop--){
		
		/* �p�X�����` */
		sprintf(linebuffer, "%s/Beam/B%d/", Load[select].Path, loop);
		
		/* �p�X����directory���J�� */
		if((beam = opendir(linebuffer)) == NULL){
			
			/* �J���Ȃ������Ȃ玟��plate�� */
			continue;
		}
		
		/* ���ڐ�Index�������� */
		count = 1;
		
		/* �w�肵��directory����File������*/
		printf("\n[pl#%d] :  Found these Files : \n", loop);
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
		
		/* ���[�U�[��File�ԍ���I�������� */
		printf("Select number of the beam track file  :  ");
		scanf("%d", &selfile);
		
		/* �I��ԍ��Ɣz��ԍ������킹�� */
		selfile--;
		
		/* ���͂��ꂽ�l�̃`�F�b�N */
		if(selfile >= count || selfile < 0){
			printf("[Error] :  Invalid number was selected\n");
			
			/* ������x�ꗗ�\���A�I�������� */
			loop++;
			continue;
		}
		
		/* directory����� */
		closedir(beam);
		
		/* �����󋵂�\�� */
		printf("\nexecute ... \n\n");
		
		/* 1plate���ɘc�݂��擾���Adistortion.vtx�𐶐� */
		result = ProcessFrame(fname[selfile], linebuffer);
		if(result == 1){
			printf("[Error] :  Invalid File Format in '%s' !!\nexit soon ...\n", fname[selfile]);
			exit(0);
		}
		else if(result == -1)
			exit(1);

		/* �����ϐ��ɂ�镪�� */
		/* �R�����g�Fflag=1�Ȃ�΁A��������File�����ʑΏۂ̔�Ղ�����directory�ɃR�s�[ */
		/* �@�@�@�@�@flag=0�Ȃ�΁A�A�y���h���Ă��� */
		if(flag == 1)
			sprintf(command, "cp %sdistortion.vtx %s/vtx/\n", linebuffer, Load[select].Path);
		else
			sprintf(command, "cat %sdistortion.vtx >> %s/vtx/distortion.vtx\n",linebuffer, Load[select].Path);
		
		/* �O���R�}���h�Ƃ��Ď��s */
		system(command);
		
		/* �����ߒ���ύX */
		flag = 0;
	}	/* for [plate�������[�v] end */
	
	/* �����I���������� */
	printf("\n*** End distortion.exe ... ***\n");
	
	/* main() �߂�l 0�i����I���j */
	return 0;
}

int ProcessFrame(char *fname, char *path){
	int loop;	/* ���[�v�J�E���^ */
	int line;	/* �ǂݎ��s�� */
	int tNum;	/* Beam track�{�� */
	int Max;	/* ���[�v�I���l */
	int lay;	/* �o�͍s�ԍ� */
	int Zset;	/* �o��Z���W�i��m�P�ʁj */
	
	// ����file�̃`�F�b�N�p�ϐ�
	int SCheck[4];	/* �\�ʔF���p�ϐ� used like Boolean */
	double ThickEMu;	/* Up-Gel�̌��� */
	double ThickEMd;	/* Down-Gel�̌��� */
	double SFu;	/* Up-Gel�̐L�k�� */
	double SFd;	/* Down-Gel�̐L�k�� */
	double BaseValue;	/* �x�[�X���� */
	double fx[1000];	/* �ǂݎ��X���W */
	double fy[1000];	/* �ǂݎ��Y���W */
	double fz[1000];	/* �ǂݎ��Z���W */
	double sz[1000];	/* SF�␳���Z���W */
	double surface[3][4];	/* emulsin layers : Vtx File�Ɠ���  surface[a][b] */
							/* a:0=x,1=y,2=z */
							/* b=0 : �ŏ㗬�� */
							/*   1 : �x�[�X�㗬�� */
							/*   2 : �x�[�X������ */
							/*   3 : �ŉ����� */
	double Bx0;	/* �ŉ����\�ʂł�X���W */
	double By0;	/* �ŉ����\�ʂł�X���W */
	double dxdz;	/* Base Angle of X-plane */
	double dydz;	/* Base Angle of Y-plane */
	char linebuffer[2048];	/* �ǂݎ��s�i�[ */
	
	// �֐��ɓn���ϐ�
	struct TRNUM tr;	/* tr# */
	int pl;	/* pl# */
	double pos;	/* Vtx File��lay# */
	double x;	/* �o�͗pX�����̘c�� */
	double y;	/* �o�͗pY�����̘c�� */
	double z;	/* �o�͗pZ�����̘c�� */
	double meanx[1003];	/* X�����̘c�݂̑��a */
	double meany[1003];	/* Y�����̘c�݂̑��a */
	double meanz[1003];	/* Z�����̘c�݂̑��a */
	
	FILE *read, *out;	/* File�|�C���^ */
	
	/* �c�݊i�[�ϐ��̏����� */
	for(loop=0; loop<1003; loop++){
		meanx[loop] = 0.;
		meany[loop] = 0.;
	}
	
	/* �t���p�X��File�����` */
	sprintf(linebuffer,"%s%s", path, fname);
	
	/* �ǂݍ���File���J�� */
	if((read = fopen(linebuffer,"r"))==NULL){
		printf("Input file \"%s\" not open!!\n",linebuffer);
		return -1;
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
	if(loop != 3){
		fclose(read);
		return 1;
	}
	
	/* File�X�g���[����߂� */
	fseek(read, 0, SEEK_SET);
	
	/* �o��File�������� */
	sprintf(linebuffer,"%sdistortion.vtx",path);
	
	/* �o��File���J�� */
	if((out = fopen(linebuffer,"w"))==NULL){
		printf("Input file \"%s\" not open!!\n",linebuffer);
		return -1;
	}
	
	/* VTX FILE���̂�scan��񕔕�a�`s��ǂݔ�΂� */
	/* �R�����g�Fs��F������܂ŃX�g���[�����ړ� */
	while(linebuffer[0] != 's')
		fgets(linebuffer,sizeof(linebuffer),read);
	
	/* ����񐔂̏����� */
	tNum = 0;
	
	/* ���W�̓ǂݎ��J�n */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		
		/* tr#�̎擾 */
		sscanf(linebuffer, "%d %*d ", &tr.prev);

		/* �s�̓�������#(VTXFILE�̕�������)�܂œǂݔ�΂� */
		if(linebuffer[0] != '#')
			continue;
		
		/* �\�ʃ`�F�b�N�p�ϐ��̏����� */
		for(loop=0; loop<4; loop++)
			SCheck[loop] = 0;
		
		/* ���[�v�J�E���^�������� */
		loop = 0;
		
		/* File�̍ŏI�s������W�̓ǂݎ����n�߂�(#�����͊܂܂Ȃ�) */
		while(pos < 3.0){
			
			/* �X�g���[����1�s��O�ɖ߂� */
			BackStreem(read);
			
			/* �P�s��ǂݎ�� */
			fgets(linebuffer,sizeof(linebuffer),read);
			sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.curr,&pl,&pos,&fx[loop],&fy[loop],&fz[loop]);
			
			/* s�����܂���tr#���ω������� */
			if(linebuffer[0] == 's' || tr.curr != tr.prev){
				
				/* �J�E���^�����Z */
				loop--;
				
				/* File�`�������̍��W���擾(Scan��񕔕�a�`s�܂ł͊܂܂Ȃ�) */
				fgets(linebuffer,sizeof(linebuffer),read);
				fgets(linebuffer,sizeof(linebuffer),read);
				
				/* tr#�ɍ��ق�����΂�����s�ړ� */
				if(tr.curr != tr.prev)
					fgets(linebuffer,sizeof(linebuffer),read);

				sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.curr,&pl,&pos,&fx[loop], &fy[loop], &fz[loop]);
			}
						
			/* �ŏ㗬�ʂ̍��W�擾 */
			if(((int) (pos*10.)) == 0){
				surface[0][0] = fx[loop];
				surface[1][0] = fy[loop];
				surface[2][0] = fz[loop];
				
				/* �\�ʔF�� */
				SCheck[0] += 1;
			}
			
			/* �㗬���܂ƃx�[�X�̋��E���W���擾 */
			else if(((int) (pos*10.)) == 10){
				surface[0][1] = fx[loop];
				surface[1][1] = fy[loop];
				surface[2][1] = fz[loop];
				
				/* �\�ʔF�� */
				SCheck[1] += 1;
			}
			
			/* �������܂ƃx�[�X�̋��E���W���擾 */
			else if(((int) (pos*10.)) == 20){
				surface[0][2] = fx[loop];
				surface[1][2] = fy[loop];
				surface[2][2] = fz[loop];
				
				/* �\�ʔF�� */
				SCheck[2] += 1;
			}
			
			/* �ŉ����ʂ̍��W�擾 */
			else if(((int) (pos*10.)) == 30){
				surface[0][3] = fx[loop];
				surface[1][3] = fy[loop];
				surface[2][3] = fz[loop];
				
				/* �\�ʔF�� */
				SCheck[3] += 1;
			}
			
			/* ���[�v�J�E���^�ǉ� */
			loop++;
		}
		
		/* ���[�v�I���l */
		Max = loop;
		
		/* Error Check */
		for(loop=0; loop<Max; loop++)
			
			/* Z�����ւ̐܂�Ԃ�������Βʒm */
			if(fz[loop] >= fz[loop-1])
				printf("  Error in tr#%d - pl#%d, -> [fz[%d]=%.5f] is bigger than [fz[%d]=%.5f] !!\n", tr.curr, pl, loop, fz[loop], loop-1, fz[loop-1]);
		
		/* Error Check */
		for(loop=0; loop<4; loop++)
			
			/* �Q�x�ȏ㓯���\�ʂ�����ꍇ���ʒm */
			if(SCheck[loop] > 1)
				printf("  Error in Surface Identification -> tr#%d - pl#%d !!\n", tr.curr, pl);
		
		
		/* �ĂуX�g���[�����ŏI�s�Ɉړ�������(while loop�I���𑣂�) */
		while(linebuffer[0] != '#')
			fgets(linebuffer, sizeof(linebuffer), read);
		
		/* �㉺Gel�̌��� */
		ThickEMu = sqrt((surface[2][0]-surface[2][1])*(surface[2][0]-surface[2][1]));
		ThickEMd = sqrt((surface[2][2]-surface[2][3])*(surface[2][2]-surface[2][3]));
		
		/* Shurinkage Factor & Base Thickness */
		SFu = 0.5/ThickEMu;
		SFd = 0.5/ThickEMd;
		BaseValue = sqrt((surface[2][1]-surface[2][2])*(surface[2][1]-surface[2][2]));
		
		/* Base track�����l */
		Bx0 = surface[0][3];
		By0 = surface[1][3];
		
		/* Base Angle */
		dxdz = (surface[0][1]-surface[0][2])/BaseValue;
		dydz = (surface[1][1]-surface[1][2])/BaseValue;
		
		/* sz�̌v�Z */
		for(loop=0; loop<Max; loop++){
			if(fz[loop] >= surface[2][3] && fz[loop] <= surface[2][2])	// if in the DownGel
				sz[loop] = (fz[loop]-surface[2][3])*SFd;
			else if(fz[loop] >= surface[2][1] && fz[loop] <=surface[2][0])	// if in the UpGel
				sz[loop] = 0.5 + BaseValue + (fz[loop]-surface[2][1])*SFu;
		}
		
		/* distortion�����߂�B */
		for(loop=0; loop<Max; loop++){
			fx[loop] = fx[loop] - (dxdz*(sz[loop]-sz[Max-1])+Bx0);
			fy[loop] = fy[loop] - (dydz*(sz[loop]-sz[Max-1])+By0);
		}
		
		/* ���[�v�I���l(�ŏI�s#�͊܂܂Ȃ�) */
		line = Max-1;
		
		/* �e��l�������� */
		lay = 0;
		z = 0.;		//�ŉ����ʂ̈ʒu
		
		/* �o��Z���W�� SF�␳���Z���W�����������ɂ���� */
		while(((int) (z*1000.)) <= ((int) (sz[0]*1000.))){
			
			/* �o�͈ʒu�̘c�݂����߂� */
			for(loop=line; loop>=0; loop--)
				if((z-sz[loop])*(z-sz[loop]) < (sz[loop-1]-sz[loop])*(sz[loop-1]-sz[loop])){
					meanx[lay] = meanx[lay] + fx[loop] + (fx[loop-1]-fx[loop])*(z-sz[loop])/(sz[loop-1]-sz[loop]);
					meany[lay] = meany[lay] + fy[loop] + (fy[loop-1]-fy[loop])*(z-sz[loop])/(sz[loop-1]-sz[loop]);
					meanz[lay] = z;
					
					break;
				}
			
				/* Z���W��um�P�ʂɕϊ� */
			Zset = ((int) (z*1000.));
			
			/* �o�͍s�������Z */
			lay++;
			
			/* Z���W��1um���� */
			if(Zset == 500)
				z += BaseValue;
			else
				z += 0.001;
		}
		
		/* lay#��0.0�ɂ��� */
		pos = 0.0;
		
		/* tr#�̍X�V */
		tr.prev = tr.curr;

		/* track���ǉ� */
		tNum++;
		
	}	/* while [���W�̓ǂݎ��J�n] end */
	
	/* File�ւ̏o�͏��� */
	for(loop=0; loop<1002; loop++){
		
		/* �c�݂̕��ς��Z�o */
		/* �R�����g�F���肵��Beam track�������ł���� tNum > 1�ƂȂ� */
		x = meanx[loop]/((double) tNum);
		y = meany[loop]/((double) tNum);
		
		/* Z���W�͂��̂܂� */
		z = meanz[loop];
		
		/* Output */
		if(loop == 0)
			output(out, tNum, pl, 3.0, x, y, z);
		else if(loop > 0 && loop < 500)
			output(out, tNum, pl, 2.5, x, y, z);
		else if(loop == 500)
			output(out, tNum, pl, 2.0, x, y, z);
		else if(loop == 501)
			output(out, tNum, pl, 1.0, x, y, z-BaseValue);
		else if(loop > 500 && loop < 1001)
			output(out, tNum, pl, 0.5, x, y, z-BaseValue);
		else if(loop == 1001)
			output(out, tNum, pl, 0.0, x, y, z-BaseValue);
		
	}
	
	/* File����� */
	fclose(read);
	fclose(out);
	
	/* ProcessFrame() �߂�l0�i����I���j */
	return 0;
	
}	/* �֐��I�� */
