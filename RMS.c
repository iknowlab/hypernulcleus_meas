#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

/* �֐���`main()�֐��ȍ~�ɋL�q */
void CalcRMS(int, char*);

int main(int argc, char *fname[]){
	int select;	/* ���[�U�I�� */
	char ID[64];	/* File ID */
	
	/* �v�Z�Ώۂɐ�����I�� */
	printf("Select a coodinates :  \n");
	printf("\t1) X'-palne\t2) Y'-palne\n");
	printf("\t3) X' & Y'-plane both\n");
	scanf("%d", &select);
	
	/* ���͐��l�̃`�F�b�N */
	if(select > 3 || select <1){
		printf("Please input by number 1, 2 or 3!!\n");
		exit(1);
	}
	
	/* RMS�̌v�Z���� */
	CalcRMS(select-1, fname[1]);
	
	/* main() �߂�l0 (����) */
	return 0;
	
}

void CalcRMS(int mode, char *file){
	long int Max;	/* 2���ψʂ̍ő吔 */
	double deltaX;	/* 2���ψ�X'-plane */
	double deltaY;	/* 2���ψ�Y'-plane */
	double R;	/* Range */
	double SUM;	/* 2���ψʂ̂Q��l�̍��v */
	double RMS;	/* RMS */
	char linebuffer[2048];	/* �ǂݍ��ݍs */
	FILE *read, *out;	/* File�|�C���^ */
	
	/* �ǂݍ���File���J�� */
	if ((read = fopen(file,"r")) == NULL) {
		printf("FILE[%s] not open!!\n", file);
		exit(1);
	}
	
	/* �o��File���̌��� */
	if(mode == 0)
		sprintf(linebuffer, "RMSx.txt");
	else if(mode == 0)
		sprintf(linebuffer, "RMSy.txt");
	else
		sprintf(linebuffer, "RMSxy.txt");
	
	/* �o��File���A�y���h���[�h�ŊJ�� */
	if ((out = fopen(linebuffer,"a")) == NULL) {
		printf("output_file[%s] not open\n", linebuffer);
		exit(1);
	}
	
	/* �����N���A */
	SUM = 0;
	Max = 0;
	
	/* 2���ψʂ̓ǂݍ��� */
	while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
		sscanf(linebuffer," %*s %lf %lf %lf %*lf", &R, &deltaX, &deltaY);
		
		/* X'�����I���̏ꍇ */
		if(mode == 0){
			SUM = SUM + deltaX*deltaX;
			Max++;
		}
		/* Y'�����I���̏ꍇ */
		else if(mode == 1){
			SUM = SUM + deltaY*deltaY;
			Max++;
		}
		/* X' Y'�������I���̏ꍇ */
		else{
			SUM = SUM + deltaY*deltaY + deltaX*deltaX;
			Max += 2;
		}
		
	}
	
	/* RMS�����߂� */
	RMS = sqrt(SUM/((double) Max));
	
	/* �v�Z���ꂽ�l��\�� */
	printf("N : %d\tRMS = %lf\n", Max, RMS);
	
	/* �o��File�ւ̏������� */
	fprintf(out,"%.0f\t%d\t%lf\n", R, Max, RMS);
	
	/* File����� */
	fclose(read);
	fclose(out);
	
}

