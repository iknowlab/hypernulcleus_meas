/* 2016.6.7.inoh remake */
/* calc theta_plane */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

/* �\����SecondDifference : �ʒu��2���ψ� */
struct SecondDifference{
	double x;
	double y;
	double z;
	/* calc theta */
	double theta;
};

int main(int argc, char *para[])
{
	int loop;	/* ���[�v�ϐ� */
	int Global;	/* �P��Ւ���2���ψʂ̐� */
	int Inner;	/* plate�ڑ��������܂܂Ȃ�2���ψʂ̐� */
	int Pnum;	/* ���q�� */
	int pljoint[3];	/* plate�ڑ����� */
	int sel;	/* ���[�U�w��̔ԍ� */
	double R[3];	/* Range */
	double x[3];	/* X'���W */
	double y[3];	/* Y'���W */
	double z[3];	/* Z'���W */
	double Xpre;	/* �\��X'���W*/
	double Ypre;	/* �\��Y'���W*/
	double Zpre;	/* �\��Z'���W*/
	char linebuffer[2048];	/* �ǂݎ��s�i�[ */
	char ID[512];	/* particle ID */
	struct SecondDifference Diff;	/* 2���ψ� */
	struct SecondDifference Dave;	/* 2���ψʂ̂P��Օ��̕��ϒl */
	struct SecondDifference Drms;	/* �P��Ղ��瓾����2���ψʂ�RMS�l */
	FILE *read, *smp, *log;	/* File�|�C���^ */
	
	/* �\��������e�̐ݒ� */
	printf("Select a number from below :\n");
	printf("\t1) Display Average of second differences\n");
	printf("\t2) Display RMS value of second differences\n");
	printf("The number :  ");
	scanf("%d", &sel);
	
	/* ���͐��l�̊m�F */
	if(sel < 1 || sel > 2){
		printf("Please input 1 or 2!!\n");
		exit(1);
	}
	
	/* �����N���A */
	for(loop=0;loop<3;loop++)
		pljoint[loop] = 5;
	
	/* �ǂݍ���File���J�� */
	if ((read = fopen("dataTR.stprr", "r")) == NULL) {
		printf("input_file[dataTR.stprr] not open\n");
		exit(1);
	}
	
	/* Sample file(�o��File�Ƃ���)���쐬 */
	if ((smp = fopen("smpl_tr.dlt", "w")) == NULL) {
		printf("output_file[sample_tr.dlt] not open\n");
		exit(1);
	}
	
	/* Log file���쐬 */
	if ((log = fopen("delta_v1.log", "w")) == NULL) {
		printf("output_file[delta_v1.log] not open\n");
		exit(1);
	}
	
	/* ���q���������� */
	Pnum = 0;
	
	/* �ǂݍ��݊J�n */
	while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
		
		/* ������ */
		Global = 0;
		Inner = 0;
		Dave.x = 0;
		Dave.y = 0;
		Drms.x = 0;
		Drms.y = 0;
		
		/* 1�_�ڂ��擾 */
		sscanf(linebuffer," %s %lf %lf %lf %lf %d",ID,&R[0],&x[0],&y[0],&z[0],&pljoint[0]);
		
		/* 2�_�ڂ��擾 */
		fgets(linebuffer,sizeof(linebuffer),read);
		sscanf(linebuffer," %s %lf %lf %lf %lf %d",ID,&R[1],&x[1],&y[1],&z[1],&pljoint[1]);
		
		/* ���������܂Łi�P��Օ��j�擾���Ă��� */
		while(1){
			fgets(linebuffer,sizeof(linebuffer),read);
			
			/* ���������ł���Ύ��̗��q�̏����� */
			if(strncmp(linebuffer,"--------------------------------------------------",40) == 0)
				break;
			
			/* 3�_�ڂ��擾 */
			sscanf(linebuffer," %s %lf %lf %lf %lf %d",ID,&R[2],&x[2],&y[2],&z[2],&pljoint[2]);
			
			/* �\��X'���W���Z�o */
			Xpre = x[1] + (x[1]-x[0])*(R[2]-R[1])/(R[1]-R[0]);
			
			/* X'������2���ψ� */
			Diff.x = Xpre - x[2];
			
			/* �\��Y'���W���Z�o */
			Ypre = y[1] + (y[1]-y[0])*(R[2]-R[1])/(R[1]-R[0]);
			
			/* Y'������2���ψ� */
			Diff.y = Ypre - y[2];
			
			/* �\��Z'���W���Z�o */
			Zpre = z[1] + (z[1]-z[0])*(R[2]-R[1])/(R[1]-R[0]);
			
			/* Z'������2���ψ� */
			Diff.z = Zpre - z[2];

			/* calc theta_plane (only X'-axis) */
			
			Diff.theta = atan( (x[2]-x[0])/(z[2]-z[0]) );

//			printf("debug mode: %lf",z[2]-z[0]);//debug

			/* plate�����ł���ꍇ */
			if(pljoint[1] == 0 && pljoint[2] == 0){
				
				/* Sample File�ɏo�� */
				fprintf(smp,"%s\t%lf\t%lf\t%lf\t%lf\t%lf\t000\n",ID,R[2],Diff.x,Diff.y,Diff.z,Diff.theta);
				
				/* 2���ψʂ�2������Z */
				Drms.x = Drms.x + Diff.x*Diff.x;
				Drms.y = Drms.y + Diff.y*Diff.y;
				
				/* 2���ψʂ̐������Z(plate�����̂�) */
				Inner++;
			}
			
			/* �ڑ��������܂ޏꍇ */
			else{
				
				/* Sample File�ɏo�͂��邾�� */
				fprintf(smp,"%s\t%lf\t%lf\t%lf\t%lf\t%lf\t111\n",ID,R[2],Diff.x,Diff.y,Diff.z,Diff.theta);
			}
			
			/* 2���ψʂ̐�Βl�����Z */
			Dave.x = Dave.x + sqrt(Diff.x*Diff.x);
			Dave.y = Dave.y + sqrt(Diff.y*Diff.y);
			
			/* 2���ψʂ̐������Z */
			Global++;
			
			/* 1�_�ڂ�2�_�ڂ��� */
			R[0] = R[1];
			x[0] = x[1];
			y[0] = y[1];
			z[0] = z[1];
			pljoint[0] = pljoint[1];
			
			/* 2�_�ڂ�3�_�ڂ��� */
			R[1] = R[2];
			x[1] = x[2];
			y[1] = y[2];
			z[1] = z[2];
			pljoint[1] = pljoint[2];
		}
		
		/* ������"-nl"������΁A�����������������܂Ȃ� */
		if(para[1] == NULL || strcmp(para[1],"-nl")!=0)
			fprintf(smp,"--------------------------------------------------\n");
		
		/* 2���ψʂ̕��ϒl�����߂� */
		Dave.x = Dave.x/(double)Global;
		Dave.y = Dave.y/(double)Global;
		
		/* 2�����̕��ϒl�̘a */
		Dave.z = Dave.x + Dave.y;
		
		/* 2���ψʂ̂P��Ղ��瓾����RMS�����߂� */
		Drms.x = sqrt(Drms.x/(double)Inner);
		Drms.y = sqrt(Drms.y/(double)Inner);
		
		/* �\�����e�̕��� */
		if(sel != 1){
			
			/* RMS�l��\�� */
			printf("rmsSD(%s) :\t%.9f\t%.9f\t%d\n", ID, Drms.x, Drms.y, Inner);
			fprintf(log, "%s\t%.9f\t%.9f\t%d\n", ID, Drms.x, Drms.y, Inner);
			
		}
		else{
			
			/* ���ϒl��\������ꍇ */
			printf("aveSD(%s) :\t%.9f\t%.9f\t%.9f\n", ID, Dave.x, Dave.y, Dave.z);
			fprintf(log, "%s\t%.9f\t%.9f\t%.9f\n", ID, Dave.x, Dave.y, Dave.z);
			
		}
		
		/* ���q�������Z */
		Pnum++;
		
	}	/* while [���������܂Łi�P��Օ��j�擾���Ă���] end */
	
	/* File����� */
	fclose(read);
	fclose(smp);
	
	/* main() �߂�l 0(����I��) */
	return 0;
}

