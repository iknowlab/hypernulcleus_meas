#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *para[]){
	int pljoint;	/* plate�ڑ����� */
	int Cut;	/* Cut����Range */
	double Range;	/* Range */
	char fname[256];	/* �ǂݍ���File�� */
	char linebuffer[2048];	/* �ǂݍ��ݍs */
	FILE *read, *out;	/* File�|�C���^ */
	
	/* Cut����Range����� */
	printf("Input the cut range :  ");
	scanf("%d", &Cut);
	
	/* �ǂݍ���File���J�� */
	if((read = fopen("sample_tr.dlt", "r"))==NULL){
		printf("Input File [sample_tr.dlt] not open!!\n");
		exit(1);
	}
	
	/* �o��File���J�� */
	if((out = fopen("rcut.dlt", "w"))==NULL){
		printf("Output File [rcut.dlt] not open!!\n");
		exit(1);
	}
	
	/* �f�[�^�̓ǂݍ��� */
	while(fgets(linebuffer, sizeof(linebuffer), read)!=NULL){
		
		/* ���������ł���Ώo�͂��Ď��̍s�� */
		if(strncmp(linebuffer,"--------------------------------------------------",40) == 0){
			fprintf(out, "%s", linebuffer);
			continue;
		}
		
		/* Range��plate�ڑ��F���R�[�h�̎擾 */
		sscanf(linebuffer, "%*s %lf %*lf %*lf %*lf %d", &Range, &pljoint);
		
		/* �w��Range�ȉ��ł���Ύ��̍s�� */
		if(((int) Range) < Cut)
			continue;
		
		/* ��P������"-e"�̎w�肪����A����plate�ڑ������ł���Ύ��̍s�� */
		if(para[1] != NULL && strcmp(para[1],"-e") == 0){
			
			if(pljoint > 100)
				continue;
			
		}
		
		/* �擾�����s���o�� */
		fprintf(out, "%s", linebuffer);
		
	}
	
	/* File����� */
	fclose(read);
	fclose(out);
	
	/* �߂�l 0 (����) */
	return 0;
	
}
